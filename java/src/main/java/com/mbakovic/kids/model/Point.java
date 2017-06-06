package com.mbakovic.kids.model;

public class Point {

    private String x;
    private String y;

    public Point(String x, String y) {
        this.x = x;
        this.y = y;
    }

    public String getX() {
        return x;
    }

    public void setX(String x) {
        this.x = x;
    }

    public String getY() {
        return y;
    }

    public void setY(String y) {
        this.y = y;
    }

    public double x() {
        return Double.parseDouble(x);
    }

    public double y() {
        return Double.parseDouble(y);
    }
}
