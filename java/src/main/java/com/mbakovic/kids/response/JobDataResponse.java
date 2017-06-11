package com.mbakovic.kids.response;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.google.api.client.util.Key;
import com.mbakovic.kids.model.Edge;
import com.mbakovic.kids.model.JobBackup;
import com.mbakovic.kids.model.Point;
import com.mbakovic.kids.model.Status;

import java.util.List;

@JsonInclude(JsonInclude.Include.NON_NULL)
public class JobDataResponse extends StatusResponse {

    @Key
    private String uuid;

    @Key
    private List<Point> points;

    @Key
    private List<JobBackup> backups;

    @Key
    private List<Edge> edges;

    public JobDataResponse() {
    }

    public JobDataResponse(String uuid, List<Point> points, List<JobBackup> backups, List<Edge> edges) {
        super(Status.OK, null);
        this.uuid = uuid;
        this.points = points;
        this.backups = backups;
        this.edges = edges;
    }

    public String getUuid() {
        return uuid;
    }

    public void setUuid(String uuid) {
        this.uuid = uuid;
    }

    public List<Point> getPoints() {
        return points;
    }

    public void setPoints(List<Point> points) {
        this.points = points;
    }

    public List<JobBackup> getBackups() {
        return backups;
    }

    public void setBackups(List<JobBackup> backups) {
        this.backups = backups;
    }

    public List<Edge> getEdges() {
        return edges;
    }

    public void setEdges(List<Edge> edges) {
        this.edges = edges;
    }
}
