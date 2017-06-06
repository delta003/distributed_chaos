package com.mbakovic.kids.response;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.google.api.client.util.Key;
import com.mbakovic.kids.model.Status;

@JsonInclude(JsonInclude.Include.NON_NULL)
public class StatusResponse {

    @Key
    private Status status;

    @Key
    private String message;

    public StatusResponse() {
    }

    public StatusResponse(Status status, String message) {
        this.status = status;
        this.message = message;
    }

    public Status getStatus() {
        return status;
    }

    public void setStatus(Status status) {
        this.status = status;
    }

    public String getMessage() {
        return message;
    }

    public void setMessage(String message) {
        this.message = message;
    }

    public static StatusResponse ofOk() {
        return new StatusResponse(Status.OK, null);
    }

    public static StatusResponse ofWait() {
        return new StatusResponse(Status.WAIT, null);
    }

    public static StatusResponse ofError(String msg) {
        return new StatusResponse(Status.ERROR, msg);
    }
}
