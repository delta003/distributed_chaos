package com.mbakovic.kids.model;

import com.google.api.client.util.Key;

public class IPAndPortAndUUID extends IPAndPort {

    @Key
    private String uuid;

    public IPAndPortAndUUID(String ip, String port, String uuid) {
        super(ip, port);
        this.uuid = uuid;
    }

    public String getUuid() {
        return uuid;
    }

    public void setUuid(String uuid) {
        this.uuid = uuid;
    }

    public int uuid() {
        return Integer.parseInt(uuid);
    }
}
