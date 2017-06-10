package com.mbakovic.kids.model;

import com.google.api.client.util.Key;

import java.util.List;

public class Job {

    @Key
    private String width;

    @Key
    private String height;

    @Key
    private String p;

    @Key
    private List<Point> points;

    public Job() {
    }

    public Job(String width, String height, String p, List<Point> points) {
        this.width = width;
        this.height = height;
        this.p = p;
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

    public String getP() {
        return p;
    }

    public void setP(String p) {
        this.p = p;
    }

    public List<Point> getPoints() {
        return points;
    }

    public void setPoints(List<Point> points) {
        this.points = points;
    }

    public double p() {
        return Double.parseDouble(p);
    }

    public int width() {
        return Integer.parseInt(width);
    }

    public int height() {
        return Integer.parseInt(height);
    }

    @Override
    public String toString() {
        return "Job{" +
                "width='" + width + '\'' +
                ", height='" + height + '\'' +
                ", p='" + p + '\'' +
                ", points=" + points +
                '}';
    }
}
