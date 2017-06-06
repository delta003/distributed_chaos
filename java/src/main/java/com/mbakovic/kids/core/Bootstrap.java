package com.mbakovic.kids.core;

import com.mbakovic.kids.model.IPAndPort;

public final class Bootstrap extends Server {

    private int uuidCounter;
    private IPAndPort lastNode;

    private static Bootstrap ourInstance = new Bootstrap();

    public static Bootstrap getInstance() {
        return ourInstance;
    }

    private Bootstrap() {
        super();
        uuidCounter = 0;
        lastNode = null;
    }

    public int getUUID() {
        return uuidCounter++;
    }

    public IPAndPort getLastNode() {
        return lastNode;
    }

    public void setLastNode(IPAndPort lastNode) {
        this.lastNode = lastNode;
    }

    public void reset() {
        uuidCounter = 0;
        lastNode = null;
    }
}
