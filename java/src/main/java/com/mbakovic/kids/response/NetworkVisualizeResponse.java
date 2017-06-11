package com.mbakovic.kids.response;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.google.api.client.util.Key;
import com.mbakovic.kids.model.DirectedEdge;
import com.mbakovic.kids.model.IPAndPortAndUUID;
import com.mbakovic.kids.model.Status;

import java.util.List;

@JsonInclude(JsonInclude.Include.NON_NULL)
public class NetworkVisualizeResponse extends StatusResponse {

    @Key
    List<IPAndPortAndUUID> nodes;

    @Key
    List<DirectedEdge> edges;

    public NetworkVisualizeResponse() {
    }

    public NetworkVisualizeResponse(List<IPAndPortAndUUID> nodes, List<DirectedEdge> edges) {
        super(Status.OK, null);
        this.nodes = nodes;
        this.edges = edges;
    }

    public List<IPAndPortAndUUID> getNodes() {
        return nodes;
    }

    public void setNodes(List<IPAndPortAndUUID> nodes) {
        this.nodes = nodes;
    }

    public List<DirectedEdge> getEdges() {
        return edges;
    }

    public void setEdges(List<DirectedEdge> edges) {
        this.edges = edges;
    }

    @Override
    public String toString() {
        return "NetworkVisualizeResponse{" +
                "nodes=" + nodes +
                ", edges=" + edges +
                '}';
    }
}
