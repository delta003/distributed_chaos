package com.mbakovic.kids.core;

import com.mbakovic.kids.model.ServerStatus;

public abstract class Server {

    private ServerStatus status;

    public Server() {
        this.status = ServerStatus.OK;
    }

    public synchronized boolean lock() {
        if (status == ServerStatus.OK) {
            status = ServerStatus.BLOCKED;
            return true;
        }
        return false;
    }

    public void lockRelease() {
        status = ServerStatus.OK;
    }
}
