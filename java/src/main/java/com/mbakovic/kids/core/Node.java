package com.mbakovic.kids.core;

import com.mbakovic.kids.background.NetworkJoiner;
import com.mbakovic.kids.model.*;
import com.mbakovic.kids.response.JobDataResponse;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.stream.Collectors;

public final class Node extends Server {

    private IPAndPortAndUUID myself;
    private IPAndPort bootstrap;
    private List<Edge> edges;
    private List<JobExecution> jobs;

    private ServerStatus jobServerStatus;

    private static Node ourInstance = new Node();

    public static Node getInstance() {
        return ourInstance;
    }

    private Node() {
        super();
        myself = null;
        bootstrap = null;
        edges = new ArrayList<>();
        jobServerStatus = ServerStatus.OK;
        jobs = new ArrayList<>();
    }

    public IPAndPortAndUUID getMyself() {
        return myself;
    }

    public void setMyself(IPAndPortAndUUID myself) {
        this.myself = myself;
    }

    public IPAndPort getBootstrap() {
        return bootstrap;
    }

    public void setBootstrap(IPAndPort bootstrap) {
        this.bootstrap = bootstrap;
    }

    public List<Edge> getEdges() {
        return edges;
    }

    public void setEdges(List<Edge> edges) {
        this.edges = edges;
    }

    public List<JobExecution> getJobs() {
        return jobs;
    }

    public void setJobs(List<JobExecution> jobs) {
        this.jobs = jobs;
    }

    public Edge getEdgeByType(EdgeType type) {
        for (Edge e : edges) {
            if (e.getType() == type) {
                return e;
            }
        }
        return null;
    }

    public Edge replaceEdge(Edge edge) {
        Edge oldedge = getEdgeByType(edge.getType());
        if (oldedge == null) {
            edges.add(edge);
            return null;
        }
        edges.remove(oldedge);
        edges.add(edge);
        return oldedge;
    }

    public List<Edge> reset() {
        List<Edge> ret = edges;
        joinNetwork();
        return ret;
    }

    public List<Edge> getEdgesAndClear() {
        List<Edge> ret = new ArrayList<>();
        ret.addAll(edges);
        for (int i = 0; i < 3; i++) {
            removeChild();
        }
        return ret;
    }

    private void removeChild() {
        int id = -1;
        for (int i = edges.size() - 1; i >= 0; i--) {
            if (edges.get(i).getType() == EdgeType.CHILD) {
                id = i;
                break;
            }
        }
        if (id != -1) {
            edges.remove(id);
        }
    }

    public void joinNetwork() {
        edges = new ArrayList<>();
        new Thread(new NetworkJoiner()).start();
    }

    public long getChildrenCount() {
        return edges.stream().filter(x -> x.getType() == EdgeType.CHILD).count();
    }

    public synchronized boolean jobLock() {
        if (jobServerStatus == ServerStatus.OK) {
            jobServerStatus = ServerStatus.BLOCKED;
            return true;
        }
        return false;
    }

    public void jobLockRelease() {
        jobServerStatus = ServerStatus.OK;
    }

    public void addJob(JobExecution newJob) {
        jobs.add(newJob);
        Collections.sort(jobs);
    }

    public List<JobWithUUID> getJobsWithUUID() {
        return jobs.stream().map(JobExecution::getJob).collect(Collectors.toList());
    }

    public List<String> getJobIds() {
        return jobs.stream().map(JobExecution::getJobUuid).collect(Collectors.toList());
    }

    public JobExecution jobWithId(String jobid) {
        for (JobExecution e : jobs) {
            if (e.getJob().getUuid().equals(jobid)) {
                return e;
            }
        }
        return null;
    }

    public void backUpPoint(String jobid, String uuid, Point point) {
        JobExecution e = jobWithId(jobid);
        if (e == null) {
            return;  // This should never happen
        }
        e.backUpPoint(uuid, point);
    }

    public boolean checkJobId(String jobid) {
        return jobWithId(jobid) != null;
    }

    public void removeJobWithId(String jobid) {
        JobExecution e = jobWithId(jobid);
        jobs.remove(e);
    }
}
