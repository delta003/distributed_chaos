package com.mbakovic.kids.background;

import com.mbakovic.kids.core.Node;
import com.mbakovic.kids.helper.HttpHelper;
import com.mbakovic.kids.model.*;
import com.mbakovic.kids.request.JobBackUpRequest;
import com.mbakovic.kids.response.StatusResponse;
import org.apache.log4j.Logger;

public class JobExecutor implements Runnable {
    private static final long EXECUTOR_SLEEP = 1000;
    private static final long LONG_EXECUTOR_SLEEP = 5000;
    private static Logger log = Logger.getLogger(JobExecutor.class);

    @Override
    public void run() {
        // Sleep to let node join the network and fetch jobs
        sleep(LONG_EXECUTOR_SLEEP);
        while (true) {
            executeJob();
            sleep(EXECUTOR_SLEEP);
        }
    }

    private void executeJob() {
        // Take job lock
        while (!Node.getInstance().jobLock()) {
            sleep(1000);
        }

        // Compute point
        int uuid = Integer.parseInt(Node.getInstance().getMyself().getUuid());
        int jobIndex = uuid % Node.getInstance().getJobs().size();
        JobExecution je = Node.getInstance().getJobs().get(jobIndex);
        PointDouble newPoint = je.computePoint();

        // Backup point to next
        Edge next = Node.getInstance().getEdgeByType(EdgeType.NEXT);
        if (next != null) {
            StatusResponse backupStatus = HttpHelper.getInstance().jobsBackup(
                    new IPAndPort(next.getIp(), next.getPort()),
                    new JobBackUpRequest(
                            Node.getInstance().getMyself().getUuid(), je.getJobUuid(), newPoint.toPoint()));
            if (backupStatus == null) {
                log.warn("Next is unreachable for backup.");
            } else if (backupStatus.getStatus() == Status.ERROR) {
                log.warn("Next responded with error on backup: " + backupStatus.getMessage());
            }
        } else {
            log.warn("Next is null in execute job to backup.");
        }

        // Backup point to prev
        Edge prev = Node.getInstance().getEdgeByType(EdgeType.PREV);
        if (prev != null) {
            StatusResponse backupStatus = HttpHelper.getInstance().jobsBackup(
                    new IPAndPort(prev.getIp(), prev.getPort()),
                    new JobBackUpRequest(
                            Node.getInstance().getMyself().getUuid(), je.getJobUuid(), newPoint.toPoint()));
            if (backupStatus == null) {
                log.warn("Prev is unreachable for backup.");
            } else if (backupStatus.getStatus() == Status.ERROR) {
                log.warn("Prev responded with error on backup: " + backupStatus.getMessage());
            }
        } else {
            log.warn("Prev is null in execute job to backup.");
        }

        // Release job lock
        Node.getInstance().jobLockRelease();
    }

    private void sleep(long duration) {
        try {
            Thread.sleep(duration);
        } catch (InterruptedException e) {
            log.warn("JobExecutor sleep interrupted.");
        }
    }
}
