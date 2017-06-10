package com.mbakovic.kids.api;

import com.mbakovic.kids.request.IPAndPortRequest;
import com.mbakovic.kids.response.StatusResponse;

import javax.ws.rs.*;
import javax.ws.rs.core.MediaType;

@Path("/")
@Produces(MediaType.APPLICATION_JSON)
@Consumes(MediaType.APPLICATION_JSON)
public interface BootstrapService {

    @POST
    @Path("/hello")
    StatusResponse hello(IPAndPortRequest request);

    @GET
    @Path("/reset")
    StatusResponse reset();

    @GET
    @Path("/reset_done")
    StatusResponse resetDone();
}
