package com.mbakovic.kids.response;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.google.api.client.util.Key;
import com.mbakovic.kids.model.Bool;
import com.mbakovic.kids.model.Status;

@JsonInclude(JsonInclude.Include.NON_NULL)
public class CheckResponse extends StatusResponse {

    @Key
    private Bool alive;

    public CheckResponse(Bool alive) {
        super(Status.OK, null);
        this.alive = alive;
    }

    public Bool getAlive() {
        return alive;
    }

    public void setAlive(Bool alive) {
        this.alive = alive;
    }
}
