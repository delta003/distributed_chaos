package com.mbakovic.kids.model;

public class PointDouble {

    private Double x;

    private Double y;

    public PointDouble(Double x, Double y) {
        this.x = x;
        this.y = y;
    }

    public Double x() {
        return x;
    }

    public Double y() {
        return y;
    }

    public static PointDouble fromPoint(Point p) {
        Double x = Double.parseDouble(p.getX());
        Double y = Double.parseDouble(p.getY());
        return new PointDouble(x, y);
    }

    public Point toPoint() {
        String xS = String.format("%.4f", x);
        String yS = String.format("%.4f", y);
        return new Point(xS, yS);
    }
}
