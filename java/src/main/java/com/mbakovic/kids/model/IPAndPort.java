package com.mbakovic.kids.model;

import com.google.api.client.util.Key;

public class IPAndPort {

    @Key
    private String ip;

    @Key
    private String port;

    public IPAndPort(String ip, String port) {
        this.ip = ip;
        this.port = port;
    }

    public String getIp() {
        return ip;
    }

    public void setIp(String ip) {
        this.ip = ip;
    }

    public String getPort() {
        return port;
    }

    public void setPort(String port) {
        this.port = port;
    }

    @Override
    public String toString() {
        return ip + ":" + port;
    }
}
