package com.mbakovic.kids.resources;

import com.mbakovic.kids.api.JobsService;
import com.mbakovic.kids.core.Node;
import com.mbakovic.kids.helper.HttpHelper;
import com.mbakovic.kids.model.*;
import com.mbakovic.kids.request.JobBackUpRequest;
import com.mbakovic.kids.request.JobRequest;
import com.mbakovic.kids.response.*;
import org.apache.log4j.Logger;

import java.util.*;

public class JobsResource implements JobsService {
    private static Logger log = Logger.getLogger(NetworkResource.class);

    private static String lockErrorMsg = "Failed to take job lock.";

    @Override
    public StatusResponse add(String jobid, JobRequest request) {
        if (!request.valid()) {
            return StatusResponse.ofError("Invalid new job request.");
        }
        if (!Node.getInstance().jobLock() || !Node.getInstance().lock()) {
            log.warn(lockErrorMsg);
            return StatusResponse.ofWait();
        }
        JobExecution newJob = new JobExecution(request.getWidth(), request.getHeight(),
                request.getP(), request.getPoints());
        Node.getInstance().addJob(newJob);
        log.info("New job added with UUID: " + newJob.getJobUuid());
        EdgesResponse response = new EdgesResponse(Node.getInstance().getEdges());
        Node.getInstance().jobLockRelease();
        Node.getInstance().lockRelease();
        return response;
    }

    @Override
    public StatusResponse newJob(JobRequest request) {
        if (!request.valid()) {
            return StatusResponse.ofError("Invalid new job request.");
        }
        if (!Node.getInstance().jobLock() || !Node.getInstance().lock()) {
            log.warn(lockErrorMsg);
            return StatusResponse.ofWait();
        }
        JobExecution newJob = new JobExecution(request.getWidth(), request.getHeight(),
                request.getP(), request.getPoints());
        Node.getInstance().addJob(newJob);
        log.info("New job created with UUID: " + newJob.getJobUuid());
        broadcastNewJob(newJob.getJobUuid(), request);
        JobIDResponse response = new JobIDResponse(newJob.getJobUuid());
        Node.getInstance().jobLockRelease();
        Node.getInstance().lockRelease();
        return response;
    }

    private void broadcastNewJob(String uuid, JobRequest request) {
        log.info("Broadcasting new job with uuid: " + uuid);

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

            EdgesResponse edgesReponse = HttpHelper.getInstance().jobsCreateWithRetry(
                    new IPAndPort(e.getIp(), e.getPort()), uuid, request);
            if (edgesReponse == null) {
                String msg = String.format("Node unreachable for jobs create %s:%s (%s)",
                        e.getIp(), e.getPort(), e.getUuid());
                log.error(msg);
                continue;
            }
            if (edgesReponse.getStatus() == Status.ERROR) {
                String msg = String.format("Node failed when create jobs %s:%s (%s) with error %s",
                        e.getIp(), e.getPort(), e.getUuid(), edgesReponse.getMessage());
                log.error(msg);
                continue;
            }
            if (edgesReponse.getEdges() == null) {
                String msg = String.format("Node jobs create returned null edges from %s:%s (%s)",
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

        log.info("Broadcast of new job completed.");
    }

    @Override
    public StatusResponse all() {
        if (!Node.getInstance().jobLock()) {
            log.warn(lockErrorMsg);
            return StatusResponse.ofWait();
        }
        AllJobsResponse response = new AllJobsResponse(Node.getInstance().getJobsWithUUID());
        Node.getInstance().jobLockRelease();
        return response;
    }

    @Override
    public StatusResponse backup(String jobid, JobBackUpRequest request) {
        if (request.getJobid() == null || request.getUuid() == null) {
            return StatusResponse.ofError("JobId or UUID is null.");
        }
        if (!Node.getInstance().jobLock()) {
            log.warn(lockErrorMsg);
            return StatusResponse.ofWait();
        }
        if (!Node.getInstance().checkJobId(request.getJobid())) {
            log.error("Backup requested with unknown jobid: " + request.getJobid());
            Node.getInstance().jobLockRelease();
            return StatusResponse.ofError("Unknown jobid: " + request.getJobid());
        }
        Node.getInstance().backUpPoint(request.getJobid(), request.getUuid(), request.getPoint());
        Node.getInstance().jobLockRelease();
        return StatusResponse.ofOk();
    }

    @Override
    public StatusResponse remove(String jobid) {
        if (!Node.getInstance().jobLock() || !Node.getInstance().lock()) {
            log.warn(lockErrorMsg);
            return StatusResponse.ofWait();
        }
        if (!Node.getInstance().checkJobId(jobid)) {
            log.error("Remove requested with unknown jobid: " + jobid);
            Node.getInstance().jobLockRelease();
            Node.getInstance().lockRelease();
            return StatusResponse.ofError("Unknown jobid: " + jobid);
        }
        Node.getInstance().removeJobWithId(jobid);
        log.info("Removed job with jobid: " + jobid);
        EdgesResponse response = new EdgesResponse(Node.getInstance().getEdges());
        Node.getInstance().jobLockRelease();
        Node.getInstance().lockRelease();
        return response;
    }

    @Override
    public StatusResponse kill(String jobid) {
        if (!Node.getInstance().jobLock() || !Node.getInstance().lock()) {
            log.warn(lockErrorMsg);
            return StatusResponse.ofWait();
        }
        if (!Node.getInstance().checkJobId(jobid)) {
            log.error("Kill requested with unknown jobid: " + jobid);
            Node.getInstance().jobLockRelease();
            Node.getInstance().lockRelease();
            return StatusResponse.ofError("Unknown jobid: " + jobid);
        }
        Node.getInstance().removeJobWithId(jobid);
        log.info("Killed job with jobid: " + jobid);
        broadcastKillJob(jobid);
        Node.getInstance().jobLockRelease();
        Node.getInstance().lockRelease();
        return StatusResponse.ofOk();
    }

    private void broadcastKillJob(String uuid) {
        log.info("Broadcasting kill job with uuid: " + uuid);

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

            EdgesResponse edgesReponse = HttpHelper.getInstance().jobsRemoveWithRetry(
                    new IPAndPort(e.getIp(), e.getPort()), uuid);
            if (edgesReponse == null) {
                String msg = String.format("Node unreachable for jobs remove %s:%s (%s)",
                        e.getIp(), e.getPort(), e.getUuid());
                log.error(msg);
                continue;
            }
            if (edgesReponse.getStatus() == Status.ERROR) {
                String msg = String.format("Node failed when remove jobs %s:%s (%s) with error %s",
                        e.getIp(), e.getPort(), e.getUuid(), edgesReponse.getMessage());
                log.error(msg);
                continue;
            }
            if (edgesReponse.getEdges() == null) {
                String msg = String.format("Node jobs remove returned null edges from %s:%s (%s)",
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

        log.info("Broadcast of kill job completed.");
    }

    @Override
    public StatusResponse ids() {
        if (!Node.getInstance().jobLock()) {
            log.warn(lockErrorMsg);
            return StatusResponse.ofWait();
        }
        JobIdsResponse response = new JobIdsResponse(Node.getInstance().getJobIds());
        Node.getInstance().jobLockRelease();
        return response;
    }

    @Override
    public StatusResponse data(String jobid) {
        if (!Node.getInstance().jobLock() || !Node.getInstance().lock()) {
            log.warn(lockErrorMsg);
            return StatusResponse.ofWait();
        }
        if (!Node.getInstance().checkJobId(jobid)) {
            log.error("Data requested with unknown jobid: " + jobid);
            Node.getInstance().jobLockRelease();
            Node.getInstance().lockRelease();
            return StatusResponse.ofError("Unknown jobid: " + jobid);
        }
        JobExecution e = Node.getInstance().jobWithId(jobid);
        if (e == null) {
            // This should never happen
            log.error("Something is wrong, job disappeared when fetching data. Unknown jobid: " + jobid);
            Node.getInstance().jobLockRelease();
            Node.getInstance().lockRelease();
            return StatusResponse.ofError("Unknown jobid: " + jobid);
        }
        JobDataResponse response = new JobDataResponse(
                e.getJobUuid(), e.getPoints(), e.getBackups(),
                Node.getInstance().getEdges());
        Node.getInstance().jobLockRelease();
        Node.getInstance().lockRelease();
        return response;
    }

    @Override
    public StatusResponse visualize(String jobid) {
        return null;
    }
}
