package com.mbakovic.kids.response;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.google.api.client.util.Key;
import com.mbakovic.kids.model.IPAndPortAndUUID;
import com.mbakovic.kids.model.Status;

@JsonInclude(JsonInclude.Include.NON_NULL)
public class IPAndPortAndUUIDResponse extends StatusResponse {

    @Key
    private String ip;

    @Key
    private String port;

    @Key
    private String uuid;

    public IPAndPortAndUUIDResponse(String ip, String port, String uuid) {
        super(Status.OK, null);
        this.ip = ip;
        this.port = port;
        this.uuid = uuid;
    }

    public IPAndPortAndUUIDResponse(IPAndPortAndUUID node) {
        super(Status.OK, null);
        this.ip = node.getIp();
        this.port = node.getPort();
        this.uuid = node.getUuid();
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

    public String getUuid() {
        return uuid;
    }

    public void setUuid(String uuid) {
        this.uuid = uuid;
    }
}
