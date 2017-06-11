package com.mbakovic.kids.model;

import com.google.api.client.util.Key;

import java.util.ArrayList;
import java.util.List;

public class JobBackup {

    @Key
    private String uuid;  // This is node uuid

    @Key
    private List<Point> points;

    public JobBackup() {
    }

    public JobBackup(String uuid, List<Point> points) {
        this.uuid = uuid;
        this.points = points;
    }

    public JobBackup(String uuid) {
        this.uuid = uuid;
        this.points = new ArrayList<>();
    }

    public String getUuid() {
        return uuid;
    }

    public void setUuid(String uuid) {
        this.uuid = uuid;
    }

    public List<Point> getPoints() {
        return points;
    }

    public void setPoints(List<Point> points) {
        this.points = points;
    }
}
