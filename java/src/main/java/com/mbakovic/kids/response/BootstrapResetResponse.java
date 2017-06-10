package com.mbakovic.kids.response;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.google.api.client.util.Key;
import com.mbakovic.kids.model.Bool;
import com.mbakovic.kids.model.Status;

public class BootstrapResetResponse extends StatusResponse {

    @Key("can_reset")
    @JsonProperty("can_reset")
    private Bool canReset;

    public BootstrapResetResponse() {
    }

    public BootstrapResetResponse(Bool canReset) {
        super(Status.OK, null);
        this.canReset = canReset;
    }

    public Bool getCanReset() {
        return canReset;
    }

    public void setCanReset(Bool canReset) {
        this.canReset = canReset;
    }

    public static BootstrapResetResponse approved() {
        return new BootstrapResetResponse(Bool.TRUE);
    }

    public static BootstrapResetResponse denied() {
        return new BootstrapResetResponse(Bool.FALSE);
    }

    @Override
    public String toString() {
        return "BootstrapResetResponse{" +
                "canReset=" + canReset +
                '}';
    }
}
