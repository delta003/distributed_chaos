package com.mbakovic.kids.resources;

import com.mbakovic.kids.api.NetworkService;
import com.mbakovic.kids.core.Node;
import com.mbakovic.kids.helper.HttpHelper;
import com.mbakovic.kids.model.*;
import com.mbakovic.kids.request.AdoptRequest;
import com.mbakovic.kids.request.EdgeRequest;
import com.mbakovic.kids.request.EdgeTypeRequest;
import com.mbakovic.kids.response.*;
import org.apache.log4j.Logger;

import java.util.*;

public class NetworkResource implements NetworkService {
    private static Logger log = Logger.getLogger(NetworkResource.class);

    private static String lockErrorMsg = "Failed to take lock.";

    @Override
    public StatusResponse edges() {
        if (!Node.getInstance().lock()) {
            log.warn(lockErrorMsg);
            return StatusResponse.ofWait();
        }
        EdgesResponse response = new EdgesResponse(Node.getInstance().getEdges());
        Node.getInstance().release();
        return response;
    }

    @Override
    public StatusResponse getEdge(EdgeTypeRequest request) {
        if (request.getType() == EdgeType.CHILD) {
            String msg = "Child type not supported for getEdge.";
            log.error(msg);
            return StatusResponse.ofError(msg);
        }
        if (!Node.getInstance().lock()) {
            log.warn(lockErrorMsg);
            return StatusResponse.ofWait();
        }
        EdgeResponse response = new EdgeResponse(Node.getInstance().getEdgeByType(request.getType()));
        Node.getInstance().release();
        return response;
    }

    @Override
    public StatusResponse setEdge(EdgeRequest request) {
        if (request.getEdge().getType() == EdgeType.CHILD) {
            String msg = "Child type not supported for setEdge.";
            log.error(msg);
            return StatusResponse.ofError(msg);
        }
        if (!Node.getInstance().lock()) {
            log.warn(lockErrorMsg);
            return StatusResponse.ofWait();
        }
        Edge oldedge = Node.getInstance().replaceEdge(request.getEdge());
        OldEdgeResponse response = new OldEdgeResponse(oldedge);
        Node.getInstance().release();
        return response;
    }

    @Override
    public StatusResponse adopt(AdoptRequest request) {
        if (request.getCanRedirect() == null) {
            return StatusResponse.ofError("Invalid adopt request. Missing can_redirect.");
        }
        if (request.getEdge().getType() != EdgeType.CHILD) {
            return StatusResponse.ofError("Only child type is supported for adopt.");
        }
        if (!Node.getInstance().lock()) {
            return StatusResponse.ofWait();
        }
        if (Node.getInstance().getEdges().size() < 2 || Node.getInstance().getEdges().size() == 3) {
            Node.getInstance().getEdges().add(request.getEdge());
            Node.getInstance().release();
            return AdoptResponse.adopted();
        }
        if (Node.getInstance().getEdges().size() == 2) {
            if (request.getCanRedirect().toBoolean()) {
                Edge next = Node.getInstance().getEdgeByType(EdgeType.NEXT);
                AdoptResponse response = AdoptResponse.redirect(next);
                Node.getInstance().release();
                return response;
            } else {
                Node.getInstance().getEdges().add(request.getEdge());
                Node.getInstance().release();
                return AdoptResponse.adopted();
            }
        }
        if (Node.getInstance().getEdges().size() == 4) {
            Node.getInstance().getEdges().add(request.getEdge());
            List<Edge> edges = Node.getInstance().getEdgesAndClear();
            AdoptResponse response = AdoptResponse.adoptedWithCreateLevel(edges);
            Node.getInstance().release();
            return response;
        }
        return StatusResponse.ofError("Something is wrong. Invalid number of edges " +
                Node.getInstance().getEdges().size());
    }

    @Override
    public StatusResponse reset() {
        if (!Node.getInstance().lock()) {
            log.warn(lockErrorMsg);
            return StatusResponse.ofWait();
        }
        log.info("Node reset initiated!");
        // This do not release, NetworkJoiner will release the lock
        // Node.getInstance().release();
        return new EdgesResponse(Node.getInstance().reset());
    }

    @Override
    public StatusResponse visualize() {
        List<IPAndPortAndUUID> nodes = new ArrayList<>();
        List<DirectedEdge> edges = new ArrayList<>();
        Map<String, Boolean> bfsMap = new HashMap<>();
        Queue<Edge> bfsQueue = new LinkedList<>();

        // Add this
        nodes.add(Node.getInstance().getMyself());
        bfsMap.put(Node.getInstance().getMyself().getUuid(), Boolean.TRUE);
        for (Edge e : Node.getInstance().getEdges()) {
            edges.add(new DirectedEdge(
                    Node.getInstance().getMyself().getUuid(),
                    e.getUuid(),
                    e.getType()
            ));
            bfsMap.put(e.getUuid(), Boolean.TRUE);
            bfsQueue.add(e);
        }

        while (!bfsQueue.isEmpty()) {
            Edge e = bfsQueue.remove();
            nodes.add(new IPAndPortAndUUID(e.getIp(), e.getPort(), e.getUuid()));

            EdgesResponse nextEdgesResponse = HttpHelper.getInstance().networkEdgesWithRepeat(
                    new IPAndPort(e.getIp(), e.getPort()));
            if (nextEdgesResponse == null) {
                String msg = String.format("Failed to fetch edges from %s:%s (%s}",
                        e.getIp(), e.getPort(), e.getUuid());
                log.error(msg);
                continue;
                //return StatusResponse.ofError(msg);
            }
            if (nextEdgesResponse.getStatus() == Status.ERROR) {
                String msg = String.format("Failed to fetch edges from %s:%s (%s} with error %s",
                        e.getIp(), e.getPort(), e.getUuid(), nextEdgesResponse.getMessage());
                log.error(msg);
                continue;
                //return StatusResponse.ofError(msg);
            }
            if (nextEdgesResponse.getEdges() == null) {
                String msg = String.format("Fetched null edges from %s:%s (%s}",
                        e.getIp(), e.getPort(), e.getUuid());
                log.error(msg);
                continue;
                //return StatusResponse.ofError(msg);
            }
            List<Edge> nextEdges = nextEdgesResponse.getEdges();

            for (Edge nextE : nextEdges) {
                edges.add(new DirectedEdge(
                        e.getUuid(),
                        nextE.getUuid(),
                        nextE.getType()
                ));
                if (!bfsMap.containsKey(nextE.getUuid())) {
                    bfsMap.put(nextE.getUuid(), Boolean.TRUE);
                    bfsQueue.add(nextE);
                }
            }
        }

        return new NetworkVisualizeResponse(nodes, edges);
    }
}
