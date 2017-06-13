package com.mbakovic.kids.response;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.google.api.client.util.Key;
import com.mbakovic.kids.model.NodeJobExecution;
import com.mbakovic.kids.model.Point;
import com.mbakovic.kids.model.Status;

import java.util.List;

@JsonInclude(JsonInclude.Include.NON_NULL)
public class JobVisualizeResponse extends StatusResponse {

    @Key
    private String width;

    @Key
    private String height;

    @Key("startingpoints")
    @JsonProperty("startingpoints")
    private List<Point> startingPoints;

    @Key
    private List<NodeJobExecution> points;

    public JobVisualizeResponse() {
    }

    public JobVisualizeResponse(String width, String height, List<Point> startingPoints, List<NodeJobExecution> points) {
        super(Status.OK, null);
        this.width = width;
        this.height = height;
        this.startingPoints = startingPoints;
        this.points = points;
    }

    public List<Point> getStartingPoints() {
        return startingPoints;
    }

    public void setStartingPoints(List<Point> startingPoints) {
        this.startingPoints = startingPoints;
    }

    public List<NodeJobExecution> getPoints() {
        return points;
    }

    public void setPoints(List<NodeJobExecution> points) {
        this.points = points;
    }

    public String getWidth() {
        return width;
    }

    public void setWidth(String width) {
        this.width = width;
    }

    public String getHeight() {
        return height;
    }

    public void setHeight(String height) {
        this.height = height;
    }
}
