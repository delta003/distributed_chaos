package com.mbakovic.kids.api;

import com.mbakovic.kids.request.JobBackUpRequest;
import com.mbakovic.kids.request.JobRequest;
import com.mbakovic.kids.response.StatusResponse;

import javax.ws.rs.*;
import javax.ws.rs.core.MediaType;

@Path("/jobs")
@Produces(MediaType.APPLICATION_JSON)
@Consumes(MediaType.APPLICATION_JSON)
public interface JobsService {

    @POST
    @Path("/add/{jobid}")
    StatusResponse add(@PathParam("jobid") String jobid, JobRequest request);

    @POST
    @Path("/new")
    StatusResponse newJob(JobRequest request);

    @GET
    @Path("/all")
    StatusResponse all();

    @POST
    @Path("/backup")
    StatusResponse backup(JobBackUpRequest request);

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
