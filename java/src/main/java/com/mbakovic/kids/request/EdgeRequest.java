package com.mbakovic.kids.request;

import com.google.api.client.util.Key;
import com.mbakovic.kids.model.Edge;

public class EdgeRequest {

    @Key
    private Edge edge;

    public EdgeRequest() {
    }

    public EdgeRequest(Edge edge) {
        this.edge = edge;
    }

    public Edge getEdge() {
        return edge;
    }

    public void setEdge(Edge edge) {
        this.edge = edge;
    }

    @Override
    public String toString() {
        return "EdgeRequest{" +
                "edge=" + edge +
                '}';
    }
}
