package com.mbakovic.kids.background;

import com.mbakovic.kids.core.Node;
import com.mbakovic.kids.helper.HttpHelper;
import com.mbakovic.kids.model.*;
import com.mbakovic.kids.response.BootstrapResetResponse;
import com.mbakovic.kids.response.CheckResponse;
import com.mbakovic.kids.response.EdgesResponse;
import com.mbakovic.kids.response.StatusResponse;
import org.apache.log4j.Logger;

import java.util.*;

public class NodeHealthcheck implements Runnable {
    private static final long HEALTHCHECK_SLEEP = 1000;
    private static final long LONG_HEALTHCHECK_SLEEP = 5000;
    private static Logger log = Logger.getLogger(NodeHealthcheck.class);

    private int failures = 0;

    @Override
    public void run() {
        // Sleep to let node join the network
        sleep(LONG_HEALTHCHECK_SLEEP);
//        while (true) {
//
//            boolean alive = checkAlive();
//
//            if (!alive) {
//                resetNetwork();
//                sleep(LONG_HEALTHCHECK_SLEEP);
//            } else {
//                sleep(HEALTHCHECK_SLEEP);
//            }
//        }
    }

    private boolean checkAlive() {
        Edge next = Node.getInstance().getEdgeByType(EdgeType.NEXT);
        if (next == null) {
            log.warn("Next is null in healthcheck.");
            return true;
        }
        if (next.getUuid().equals(Node.getInstance().getMyself().getUuid())) {
            return true;
        }

        // Ping next
        StatusResponse ping = HttpHelper.getInstance().basicOk(
                new IPAndPort(next.getIp(), next.getPort()));

        // If not responding
        if (ping == null) {
            failures++;

            if (failures < 5) {
                return true;
            }

            // Not responding for 5s, ask prev to check it
            Edge prev = Node.getInstance().getEdgeByType(EdgeType.PREV);
            if (prev == null) {
                log.warn("Prev is null in healthcheck.");
                return true;
            }

            CheckResponse check = HttpHelper.getInstance().basicCheck(
                    new IPAndPort(prev.getIp(), prev.getPort()));

            if (check != null && check.getStatus() == Status.OK && check.getAlive().toBoolean()) {
                log.warn("Prev confirmed it's alive in healthcheck.");
                failures = 0;
                return true;
            }

            // Next is not alive
            return false;
        }
        return true;
    }

    private void resetNetwork() {
        IPAndPort bootstrap = Node.getInstance().getBootstrap();
        BootstrapResetResponse resetResponse = HttpHelper.getInstance().bootstrapReset(bootstrap);

        if (resetResponse == null) {
            log.error("Bootstrap unreachable for reset.");
            return;
        }
        if (resetResponse.getStatus() == Status.ERROR) {
            log.error("Bootstrap responded with error on reset: " + resetResponse.getMessage());
            return;
        }

        if (!resetResponse.getCanReset().toBoolean()) {
            log.warn("Reset in progress...");
            return;
        }

        // Start broadcasting reset
        broadcastReset();

        // Sleep before reset done
        sleep(HEALTHCHECK_SLEEP);

        StatusResponse resetDoneResponse = HttpHelper.getInstance().bootstrapResetDone(bootstrap);
        if (resetDoneResponse == null) {
            log.error("Bootstrap unreachable for reset done.");
        }
    }

    private void broadcastReset() {
        log.info("Broadcast reset started...");

        while (!Node.getInstance().lock()) {
            sleep(100);
        }

        Map<String, Boolean> bfsMap = new HashMap<>();
        Queue<Edge> bfsQueue = new LinkedList<>();

        // Add this
        bfsMap.put(Node.getInstance().getMyself().getUuid(), Boolean.TRUE);
        Node.getInstance().getEdges().stream().filter(e -> !bfsMap.containsKey(e.getUuid())).forEach(e -> {
            bfsMap.put(e.getUuid(), Boolean.TRUE);
            bfsQueue.add(e);
        });

        while (!bfsQueue.isEmpty()) {
            Edge e = bfsQueue.remove();

            EdgesResponse edgesReponse = HttpHelper.getInstance().networkResetWithRetry(
                    new IPAndPort(e.getIp(), e.getPort()));
            if (edgesReponse == null) {
                String msg = String.format("Node unreachable for reset %s:%s (%s)",
                        e.getIp(), e.getPort(), e.getUuid());
                log.error(msg);
                continue;
            }
            if (edgesReponse.getStatus() == Status.ERROR) {
                String msg = String.format("Node failed when reset %s:%s (%s) with error %s",
                        e.getIp(), e.getPort(), e.getUuid(), edgesReponse.getMessage());
                log.error(msg);
                continue;
            }
            if (edgesReponse.getEdges() == null) {
                String msg = String.format("Node reset returned null edges from %s:%s (%s)",
                        e.getIp(), e.getPort(), e.getUuid());
                log.error(msg);
                continue;
            }
            List<Edge> nextEdges = edgesReponse.getEdges();

            nextEdges.stream().filter(nextE -> !bfsMap.containsKey(nextE.getUuid())).forEach(nextE -> {
                bfsMap.put(nextE.getUuid(), Boolean.TRUE);
                bfsQueue.add(nextE);
            });
        }

        // This will lockRelease lock on this node and rejoin node to new network
        Node.getInstance().joinNetwork();
        log.info("Broadcast reset completed.");
    }

    private void sleep(long duration) {
        try {
            Thread.sleep(duration);
        } catch (InterruptedException e) {
            log.warn("NodeHealthcheck sleep interrupted.");
        }
    }
}
