package com.mbakovic.kids.model;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.google.api.client.util.Key;

import java.util.ArrayList;
import java.util.List;

public class NodeJobExecution {

    @Key("uuid")
    @JsonProperty("uuid")
    private String nodeUUID;

    @Key
    private List<Point> points;

    public NodeJobExecution() {
    }

    public NodeJobExecution(String nodeUUID, List<Point> points) {
        this.nodeUUID = nodeUUID;
        this.points = points;
    }

    public NodeJobExecution(String nodeUUID) {
        this.nodeUUID = nodeUUID;
        this.points = new ArrayList<>();
    }

    public String getNodeUUID() {
        return nodeUUID;
    }

    public void setNodeUUID(String nodeUUID) {
        this.nodeUUID = nodeUUID;
    }

    public List<Point> getPoints() {
        return points;
    }

    public void setPoints(List<Point> points) {
        this.points = points;
    }
}
