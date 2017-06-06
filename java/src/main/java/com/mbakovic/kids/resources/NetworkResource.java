package com.mbakovic.kids.resources;

import com.mbakovic.kids.api.NetworkService;
import com.mbakovic.kids.core.Node;
import com.mbakovic.kids.model.EdgeType;
import com.mbakovic.kids.request.AdoptRequest;
import com.mbakovic.kids.request.EdgeRequest;
import com.mbakovic.kids.request.EdgeTypeRequest;
import com.mbakovic.kids.response.EdgeResponse;
import com.mbakovic.kids.response.EdgesResponse;
import com.mbakovic.kids.response.StatusResponse;

public class NetworkResource implements NetworkService {
    @Override
    public StatusResponse edges() {
        if (!Node.getInstance().lock()) {
            return StatusResponse.ofWait();
        }
        EdgesResponse response = new EdgesResponse(Node.getInstance().getEdges());
        Node.getInstance().release();
        return response;
    }

    @Override
    public StatusResponse getEdge(EdgeTypeRequest request) {
        if (request.getType() == EdgeType.CHILD) {
            return StatusResponse.ofError("Child type not supported for getEdge.");
        }
        if (!Node.getInstance().lock()) {
            return StatusResponse.ofWait();
        }
        EdgeResponse response = new EdgeResponse(Node.getInstance().getEdgeByType(request.getType()));
        Node.getInstance().release();
        return response;
    }

    @Override
    public StatusResponse setEdge(EdgeRequest request) {
        return null;
    }

    @Override
    public StatusResponse adopt(AdoptRequest request) {
        return null;
    }

    @Override
    public StatusResponse reset() {
        return null;
    }

    @Override
    public StatusResponse visualize() {
        return null;
    }
}
