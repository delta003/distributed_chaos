package com.mbakovic.kids.core;

import com.mbakovic.kids.model.IPAndPortAndUUID;

public final class Bootstrap extends Server {

    private int uuidCounter;
    private IPAndPortAndUUID lastNode;

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

    public IPAndPortAndUUID getLastNode() {
        return lastNode;
    }

    public void setLastNode(IPAndPortAndUUID lastNode) {
        this.lastNode = lastNode;
    }

    public void reset() {
        uuidCounter = 0;
        lastNode = null;
    }
}
