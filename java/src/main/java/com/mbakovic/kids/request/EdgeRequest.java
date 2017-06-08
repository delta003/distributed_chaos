package com.mbakovic.kids.request;

import com.mbakovic.kids.model.Edge;

public class EdgeRequest {

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
}
