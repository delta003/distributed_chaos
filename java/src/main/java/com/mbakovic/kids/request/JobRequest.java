package com.mbakovic.kids.request;

import com.mbakovic.kids.model.Job;
import com.mbakovic.kids.model.Point;

import java.util.List;

public class JobRequest extends Job {

    public JobRequest() {
    }

    public JobRequest(String width, String height, String p, List<Point> points) {
        super(width, height, p, points);
    }

    @Override
    public String toString() {
        return "JobRequest{}";
    }

    public boolean valid() {
        if (getWidth() == null || getHeight() == null || getP() == null || getPoints() == null) {
            return false;
        }
        if (getPoints().size() == 0) {
            return false;
        }
        Double pd = Double.parseDouble(getP());
        if (pd <= 0.0 || pd >= 1.0) {
            return false;
        }
        Double dw = Double.parseDouble(getWidth());
        double dh = Double.parseDouble(getHeight());
        for (Point p : getPoints()) {
            Double dx = Double.parseDouble(p.getX());
            Double dy = Double.parseDouble(p.getY());
            if (dx < 0.0 || dy < 0.0 || dx > dw || dy > dh) {
                return false;
            }
        }
        return true;
    }
}
