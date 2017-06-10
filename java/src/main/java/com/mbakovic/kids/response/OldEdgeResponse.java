package com.mbakovic.kids.response;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.google.api.client.util.Key;
import com.mbakovic.kids.model.Edge;
import com.mbakovic.kids.model.Status;

@JsonInclude(JsonInclude.Include.NON_NULL)
public class OldEdgeResponse extends StatusResponse {

    @Key("oldedge")
    @JsonProperty("oldedge")
    private Edge oldEdge;

    public OldEdgeResponse() {
    }

    public OldEdgeResponse(Edge edge) {
        super(Status.OK, null);
        this.oldEdge = edge;
    }

    public Edge getOldEdge() {
        return oldEdge;
    }

    public void setOldEdge(Edge oldEdge) {
        this.oldEdge = oldEdge;
    }

    @Override
    public String toString() {
        return "OldEdgeResponse{" +
                "oldEdge=" + oldEdge +
                '}';
    }
}
