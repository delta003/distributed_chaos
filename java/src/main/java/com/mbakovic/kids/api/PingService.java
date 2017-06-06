package com.mbakovic.kids.api;

import javax.ws.rs.GET;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;
import javax.ws.rs.core.MediaType;

@Path("/")
@Produces(MediaType.TEXT_PLAIN)
public interface PingService {

    @GET
    @Path("/ping")
    String ping();
}
