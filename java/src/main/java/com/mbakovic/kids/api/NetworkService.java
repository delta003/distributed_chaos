package com.mbakovic.kids.api;

import com.mbakovic.kids.request.AdoptRequest;
import com.mbakovic.kids.request.EdgeRequest;
import com.mbakovic.kids.request.EdgeTypeRequest;
import com.mbakovic.kids.response.StatusResponse;

import javax.ws.rs.GET;
import javax.ws.rs.POST;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;
import javax.ws.rs.core.MediaType;

@Path("/network")
@Produces(MediaType.APPLICATION_JSON)
public interface NetworkService {

    @GET
    @Path("/edges")
    StatusResponse edges();

    @POST
    @Path("/get_edge")
    StatusResponse getEdge(EdgeTypeRequest request);

    @POST
    @Path("/set_edge")
    StatusResponse setEdge(EdgeRequest request);

    @POST
    @Path("/adopt")
    StatusResponse adopt(AdoptRequest request);

    @GET
    @Path("/reset")
    StatusResponse reset();

    @GET
    @Path("/visualize")
    StatusResponse visualize();
}
