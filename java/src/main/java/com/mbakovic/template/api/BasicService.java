package com.mbakovic.template.api;

import com.mbakovic.template.core.StatusResponse;

import javax.ws.rs.GET;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;
import javax.ws.rs.core.MediaType;

@Path("/")
@Produces(MediaType.APPLICATION_JSON)
public interface BasicService {

    @GET
    @Path("/ok")
    StatusResponse ok();
}
