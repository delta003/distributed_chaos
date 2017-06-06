package com.mbakovic.kids.request;

import com.mbakovic.kids.model.EdgeType;

public class EdgeTypeRequest {

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
}
