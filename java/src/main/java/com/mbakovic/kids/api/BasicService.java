package com.mbakovic.kids.api;

import com.mbakovic.kids.request.IPAndPortRequest;
import com.mbakovic.kids.response.StatusResponse;

import javax.ws.rs.GET;
import javax.ws.rs.POST;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;
import javax.ws.rs.core.MediaType;

@Path("/basic")
@Produces(MediaType.APPLICATION_JSON)
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
