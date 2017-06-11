package com.mbakovic.kids.response;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.google.api.client.util.Key;
import com.mbakovic.kids.model.JobWithUUID;
import com.mbakovic.kids.model.Status;

import java.util.List;

@JsonInclude(JsonInclude.Include.NON_NULL)
public class AllJobsResponse extends StatusResponse {

    @Key
    private List<JobWithUUID> jobs;

    public AllJobsResponse() {
    }

    public AllJobsResponse(List<JobWithUUID> jobs) {
        super(Status.OK, null);
        this.jobs = jobs;
    }

    public List<JobWithUUID> getJobs() {
        return jobs;
    }

    public void setJobs(List<JobWithUUID> jobs) {
        this.jobs = jobs;
    }
}
