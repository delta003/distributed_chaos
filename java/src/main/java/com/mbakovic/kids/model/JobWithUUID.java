package com.mbakovic.kids.model;

import com.google.api.client.util.Key;

import java.util.List;
import java.util.UUID;

public class JobWithUUID extends Job {

    @Key
    private String uuid;

    public JobWithUUID() {
    }

    public JobWithUUID(String width, String height, String p, List<Point> points) {
        super(width, height, p, points);
        uuid = UUID.randomUUID().toString();
    }

    public JobWithUUID(String width, String height, String p, List<Point> points, String uuid) {
        super(width, height, p, points);
        this.uuid = uuid;
    }

    public String getUuid() {
        return uuid;
    }

    public void setUuid(String uuid) {
        this.uuid = uuid;
    }
}
