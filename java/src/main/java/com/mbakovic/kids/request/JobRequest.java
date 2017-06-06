package com.mbakovic.kids.request;

import com.mbakovic.kids.model.Job;
import com.mbakovic.kids.model.Point;

import java.util.List;

public class JobRequest extends Job {

    public JobRequest(String width, String height, String p, List<Point> points) {
        super(width, height, p, points);
    }
}
