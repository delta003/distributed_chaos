package com.mbakovic.kids.response;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.google.api.client.util.Key;
import com.mbakovic.kids.model.Status;

import java.util.List;

@JsonInclude(JsonInclude.Include.NON_NULL)
public class JobIdsResponse extends StatusResponse {

    @Key
    private List<String> jobids;

    public JobIdsResponse() {
    }

    public JobIdsResponse(List<String> jobids) {
        super(Status.OK, null);
        this.jobids = jobids;
    }

    public List<String> getJobids() {
        return jobids;
    }

    public void setJobids(List<String> jobids) {
        this.jobids = jobids;
    }
}
