package com.mbakovic.kids.model;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.google.api.client.util.Key;

import java.util.List;
import java.util.UUID;

public class JobWithID extends Job {

    @Key("jobid")
    @JsonProperty("jobid")
    private String id;

    public JobWithID() {
    }

    public JobWithID(String width, String height, String p, List<Point> points) {
        super(width, height, p, points);
        id = UUID.randomUUID().toString();
    }

    public JobWithID(String width, String height, String p, List<Point> points, String id) {
        super(width, height, p, points);
        this.id = id;
    }

    public String getId() {
        return id;
    }

    public void setId(String id) {
        this.id = id;
    }
}
