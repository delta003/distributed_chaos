package com.mbakovic.kids.response;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.google.api.client.util.Key;
import com.mbakovic.kids.model.Edge;
import com.mbakovic.kids.model.Status;

@JsonInclude(JsonInclude.Include.NON_NULL)
public class OldEdgeResponse extends StatusResponse {

    @Key
    private Edge oldedge;

    public OldEdgeResponse(Edge edge) {
        super(Status.OK, null);
        this.oldedge = edge;
    }

    public Edge getOldedge() {
        return oldedge;
    }

    public void setOldedge(Edge edge) {
        this.oldedge = edge;
    }
}
