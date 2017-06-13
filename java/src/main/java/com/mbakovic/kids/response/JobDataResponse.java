package com.mbakovic.kids.response;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.google.api.client.util.Key;
import com.mbakovic.kids.model.Edge;
import com.mbakovic.kids.model.NodeJobExecution;
import com.mbakovic.kids.model.Point;
import com.mbakovic.kids.model.Status;

import java.util.List;

@JsonInclude(JsonInclude.Include.NON_NULL)
public class JobDataResponse extends StatusResponse {

    @Key
    private String uuid;  // This is node uuid

    @Key
    private List<Point> points;

    @Key
    private List<NodeJobExecution> backup;

    @Key
    private List<Edge> edges;

    public JobDataResponse() {
    }

    public JobDataResponse(String uuid, List<Point> points, List<NodeJobExecution> backups, List<Edge> edges) {
        super(Status.OK, null);
        this.uuid = uuid;
        this.points = points;
        this.backup = backups;
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

    public List<NodeJobExecution> getBackup() {
        return backup;
    }

    public void setBackup(List<NodeJobExecution> backup) {
        this.backup = backup;
    }

    public List<Edge> getEdges() {
        return edges;
    }

    public void setEdges(List<Edge> edges) {
        this.edges = edges;
    }

    public boolean valid() {
        return (uuid != null && points != null && backup != null && edges != null);
    }
}
