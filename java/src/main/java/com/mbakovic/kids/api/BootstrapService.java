package com.mbakovic.kids.api;

import com.mbakovic.kids.request.IPAndPortRequest;
import com.mbakovic.kids.response.StatusResponse;

import javax.ws.rs.GET;
import javax.ws.rs.POST;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;
import javax.ws.rs.core.MediaType;

@Path("/")
@Produces(MediaType.APPLICATION_JSON)
public interface BootstrapService {

    @POST
    @Path("/hello")
    StatusResponse hello(IPAndPortRequest request);

    @GET
    @Path("/reset")
    StatusResponse reset();
}
