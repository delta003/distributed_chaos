package com.mbakovic.kids.model;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.google.api.client.util.Key;

public class DirectedEdge {

    @Key
    @JsonProperty("start_uuid")
    private String startUUID;

    @Key
    @JsonProperty("end_uuid")
    private String endUUID;

    private EdgeType type;

    public DirectedEdge() {
    }

    public DirectedEdge(String startUUID, String endUUID, EdgeType type) {
        this.startUUID = startUUID;
        this.endUUID = endUUID;
        this.type = type;
    }

    public String getStartUUID() {
        return startUUID;
    }

    public void setStartUUID(String startUUID) {
        this.startUUID = startUUID;
    }

    public String getEndUUID() {
        return endUUID;
    }

    public void setEndUUID(String endUUID) {
        this.endUUID = endUUID;
    }

    public EdgeType getType() {
        return type;
    }

    public void setType(EdgeType type) {
        this.type = type;
    }

    @Override
    public String toString() {
        return "DirectedEdge{" +
                "startUUID='" + startUUID + '\'' +
                ", endUUID='" + endUUID + '\'' +
                ", type=" + type +
                '}';
    }
}
