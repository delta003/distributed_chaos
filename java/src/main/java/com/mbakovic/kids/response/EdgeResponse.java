package com.mbakovic.kids.response;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.google.api.client.util.Key;
import com.mbakovic.kids.model.Edge;
import com.mbakovic.kids.model.Status;

@JsonInclude(JsonInclude.Include.NON_NULL)
public class EdgeResponse extends StatusResponse {

    @Key
    private Edge edge;

    public EdgeResponse() {
    }

    public EdgeResponse(Edge edge) {
        super(Status.OK, null);
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
        return "EdgeResponse{" +
                "edge=" + edge +
                '}';
    }
}
