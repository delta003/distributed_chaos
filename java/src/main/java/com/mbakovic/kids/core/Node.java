package com.mbakovic.kids.core;

import com.mbakovic.kids.model.IPAndPort;

public final class Node extends Server {

    private int uuid;
    private IPAndPort myself;
    private IPAndPort bootstrap;

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
}
