package com.mbakovic.kids.request;

import com.mbakovic.kids.model.Edge;
import com.mbakovic.kids.model.EdgeType;

public class EdgeRequest extends Edge {

    public EdgeRequest(String ip, String port, String uuid, EdgeType type) {
        super(ip, port, uuid, type);
    }
}
