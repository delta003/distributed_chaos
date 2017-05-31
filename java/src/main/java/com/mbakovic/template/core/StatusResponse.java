package com.mbakovic.template.core;

public final class StatusResponse {

    private String status;
    private String error;

    public StatusResponse() {
    }

    public StatusResponse(String status, String error) {
        this.status = status;
    }

    public String getStatus() {
        return status;
    }

    public void setStatus(String status) {
        this.status = status;
    }

    public String getError() {
        return error;
    }

    public void setError(String error) {
        this.error = error;
    }
}
