package com.mbakovic.kids.request;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.mbakovic.kids.model.Bool;
import com.mbakovic.kids.model.Edge;

public class AdoptRequest {

    private Edge edge;

    @JsonProperty("can_redirect")
    private Bool canRedirect;

    public AdoptRequest() {
    }

    public AdoptRequest(Edge edge, Bool canRedirect) {
        this.edge = edge;
        this.canRedirect = canRedirect;
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
}
