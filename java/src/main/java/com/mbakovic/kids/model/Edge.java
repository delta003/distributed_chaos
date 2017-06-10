package com.mbakovic.kids.model;

import com.google.api.client.util.Key;
import com.mbakovic.kids.core.Node;

public class Edge extends IPAndPort {

    @Key
    private String uuid;

    @Key
    private EdgeType type;

    public Edge() {
    }

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

    public static Edge meAs(EdgeType type) {
        return new Edge(
                Node.getInstance().getMyself().getIp(),
                Node.getInstance().getMyself().getPort(),
                Node.getInstance().getMyself().getUuid(),
                type
        );
    }

    @Override
    public String toString() {
        return "Edge{" +
                "uuid='" + uuid + '\'' +
                ", type=" + type +
                ", IpAndPort=" + super.toString() +
                '}';
    }
}
