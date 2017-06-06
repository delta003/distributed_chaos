package com.mbakovic.kids.api;

import com.mbakovic.kids.request.JobRequest;
import com.mbakovic.kids.response.StatusResponse;

import javax.ws.rs.*;
import javax.ws.rs.core.MediaType;

@Path("/jobs")
@Produces(MediaType.APPLICATION_JSON)
public interface JobsService {

    @POST
    @Path("/create/{jobid}")
    StatusResponse create(@PathParam("jobid") String jobid, JobRequest request);

    @POST
    @Path("/new")
    StatusResponse newJob(JobRequest request);

    @GET
    @Path("/remove/{jobid}")
    StatusResponse remove(@PathParam("jobid") String jobid);

    @GET
    @Path("/kill/{jobid}")
    StatusResponse kill(@PathParam("jobid") String jobid);

    @GET
    @Path("/ids")
    StatusResponse ids();

    @GET
    @Path("/data/{jobid}")
    StatusResponse data(@PathParam("jobid") String jobid);

    @GET
    @Path("/visualize/{jobid}")
    StatusResponse visualize(@PathParam("jobid") String jobid);
}
