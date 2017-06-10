package com.mbakovic.kids.request;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.google.api.client.util.Key;
import com.mbakovic.kids.core.Node;
import com.mbakovic.kids.model.Bool;
import com.mbakovic.kids.model.Edge;
import com.mbakovic.kids.model.EdgeType;

public class AdoptRequest {

    @Key
    private Edge edge;

    @Key("can_redirect")
    @JsonProperty("can_redirect")
    private Bool canRedirect;

    public AdoptRequest() {
    }

    public AdoptRequest(Edge edge, Bool canRedirect) {
        this.edge = edge;
        this.canRedirect = canRedirect;
    }

    public static AdoptRequest adoptMeWithRedirect() {
        return new AdoptRequest(new Edge(
                Node.getInstance().getMyself().getIp(),
                Node.getInstance().getMyself().getPort(),
                Node.getInstance().getMyself().getUuid(),
                EdgeType.CHILD
        ), Bool.TRUE);
    }

    public static AdoptRequest adoptMeWithoutRedirect() {
        return new AdoptRequest(new Edge(
                Node.getInstance().getMyself().getIp(),
                Node.getInstance().getMyself().getPort(),
                Node.getInstance().getMyself().getUuid(),
                EdgeType.CHILD
        ), Bool.FALSE);
    }

    public Edge getEdge() {
        return edge;
    }

    public void setEdge(Edge edge) {
        this.edge = edge;
    }

    public Bool getCanRedirect() {
        return canRedirect;
    }

    public void setCanRedirect(Bool canRedirect) {
        this.canRedirect = canRedirect;
    }

    @Override
    public String toString() {
        return "AdoptRequest{" +
                "edge=" + edge +
                ", canRedirect=" + canRedirect +
                '}';
    }
}
