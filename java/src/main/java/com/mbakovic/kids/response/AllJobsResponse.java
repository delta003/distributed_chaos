package com.mbakovic.kids.response;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.google.api.client.util.Key;
import com.mbakovic.kids.model.JobWithID;
import com.mbakovic.kids.model.Status;

import java.util.List;

@JsonInclude(JsonInclude.Include.NON_NULL)
public class AllJobsResponse extends StatusResponse {

    @Key
    private List<JobWithID> jobs;

    public AllJobsResponse() {
    }

    public AllJobsResponse(List<JobWithID> jobs) {
        super(Status.OK, null);
        this.jobs = jobs;
    }

    public List<JobWithID> getJobs() {
        return jobs;
    }

    public void setJobs(List<JobWithID> jobs) {
        this.jobs = jobs;
    }
}
