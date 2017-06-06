package com.mbakovic.kids.core;

import com.mbakovic.kids.model.Edge;
import com.mbakovic.kids.model.EdgeType;
import com.mbakovic.kids.model.IPAndPort;

import java.util.List;

public final class Node extends Server {

    private int uuid;
    private IPAndPort myself;
    private IPAndPort bootstrap;
    private List<Edge> edges;

    private static Node ourInstance = new Node();

    public static Node getInstance() {
        return ourInstance;
    }

    private Node() {
        super();
    }

    public int getUuid() {
        return uuid;
    }

    public void setUuid(int uuid) {
        this.uuid = uuid;
    }

    public IPAndPort getMyself() {
        return myself;
    }

    public void setMyself(IPAndPort myself) {
        this.myself = myself;
    }

    public IPAndPort getBootstrap() {
        return bootstrap;
    }

    public void setBootstrap(IPAndPort bootstrap) {
        this.bootstrap = bootstrap;
    }

    public List<Edge> getEdges() {
        return edges;
    }

    public void setEdges(List<Edge> edges) {
        this.edges = edges;
    }

    public Edge getEdgeByType(EdgeType type) {
        for (Edge e : edges) {
            if (e.getType() == type) {
                return e;
            }
        }
        return null;
    }
}
