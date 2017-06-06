package com.mbakovic.kids.model;

public class Edge extends IPAndPort {

    private String uuid;
    private EdgeType type;

    public Edge(String ip, String port, String uuid, EdgeType type) {
        super(ip, port);
        this.uuid = uuid;
        this.type = type;
    }

    public String getUuid() {
        return uuid;
    }

    public void setUuid(String uuid) {
        this.uuid = uuid;
    }

    public EdgeType getType() {
        return type;
    }

    public void setType(EdgeType type) {
        this.type = type;
    }

    public int uuid() {
        return Integer.parseInt(uuid);
    }
}
