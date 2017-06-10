package com.mbakovic.kids.request;

import com.google.api.client.util.Key;
import com.mbakovic.kids.model.EdgeType;

public class EdgeTypeRequest {

    @Key
    private EdgeType type;

    public EdgeTypeRequest() {
    }

    public EdgeTypeRequest(EdgeType type) {
        this.type = type;
    }

    public EdgeType getType() {
        return type;
    }

    public void setType(EdgeType type) {
        this.type = type;
    }

    @Override
    public String toString() {
        return "EdgeTypeRequest{" +
                "type=" + type +
                '}';
    }
}
