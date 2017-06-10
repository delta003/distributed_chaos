package com.mbakovic.kids.api;

import com.mbakovic.kids.request.IPAndPortRequest;
import com.mbakovic.kids.response.StatusResponse;

import javax.ws.rs.*;
import javax.ws.rs.core.MediaType;

@Path("/basic")
@Produces(MediaType.APPLICATION_JSON)
@Consumes(MediaType.APPLICATION_JSON)
public interface BasicService {

    @GET
    @Path("/ok")
    StatusResponse ok();

    @GET
    @Path("/info")
    StatusResponse info();

    @POST
    @Path("/check")
    StatusResponse check(IPAndPortRequest request);
}
