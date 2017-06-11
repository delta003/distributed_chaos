package com.mbakovic.kids.response;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.google.api.client.util.Key;
import com.mbakovic.kids.model.Status;

@JsonInclude(JsonInclude.Include.NON_NULL)
public class JobIDResponse extends StatusResponse {

    @Key("jobid")
    @JsonProperty("jobid")
    private String jobID;

    public JobIDResponse() {
    }

    public JobIDResponse(String jobID) {
        super(Status.OK, null);
        this.jobID = jobID;
    }

    public String getJobID() {
        return jobID;
    }

    public void setJobID(String jobID) {
        this.jobID = jobID;
    }
}
