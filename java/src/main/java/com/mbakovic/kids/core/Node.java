package com.mbakovic.kids.core;

import com.mbakovic.kids.background.NetworkJoiner;
import com.mbakovic.kids.model.Edge;
import com.mbakovic.kids.model.EdgeType;
import com.mbakovic.kids.model.IPAndPort;
import com.mbakovic.kids.model.IPAndPortAndUUID;

import java.util.ArrayList;
import java.util.List;

public final class Node extends Server {

    private IPAndPortAndUUID myself;
    private IPAndPort bootstrap;
    private List<Edge> edges;

    private static Node ourInstance = new Node();

    public static Node getInstance() {
        return ourInstance;
    }

    private Node() {
        super();
        myself = null;
        bootstrap = null;
        edges = new ArrayList<>();
    }

    public IPAndPortAndUUID getMyself() {
        return myself;
    }

    public void setMyself(IPAndPortAndUUID myself) {
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

    public Edge replaceEdge(Edge edge) {
        Edge oldedge = getEdgeByType(edge.getType());
        if (oldedge == null) {
            edges.add(edge);
            return null;
        }
        edges.remove(oldedge);
        edges.add(edge);
        return oldedge;
    }

    public List<Edge> reset() {
        List<Edge> ret = edges;
        joinNetwork();
        return ret;
    }

    public List<Edge> getEdgesAndClear() {
        List<Edge> ret = new ArrayList<>();
        ret.addAll(edges);
        edges.subList(2, 5).clear();
        return ret;
    }

    public void joinNetwork() {
        edges = new ArrayList<>();
        new Thread(new NetworkJoiner()).start();
    }
}
