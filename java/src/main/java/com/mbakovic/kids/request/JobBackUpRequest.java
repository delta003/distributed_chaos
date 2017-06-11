package com.mbakovic.kids.request;

import com.google.api.client.util.Key;
import com.mbakovic.kids.core.Node;
import com.mbakovic.kids.model.Point;

public class JobBackUpRequest {

    @Key
    private String uuid;

    @Key
    private String jobid;

    @Key
    private Point point;

    public JobBackUpRequest() {
    }

    public JobBackUpRequest(String uuid, String jobid, Point point) {
        this.uuid = uuid;
        this.jobid = jobid;
        this.point = point;
    }

    public String getUuid() {
        return uuid;
    }

    public void setUuid(String uuid) {
        this.uuid = uuid;
    }

    public String getJobid() {
        return jobid;
    }

    public void setJobid(String jobid) {
        this.jobid = jobid;
    }

    public Point getPoint() {
        return point;
    }

    public void setPoint(Point point) {
        this.point = point;
    }

    public static JobBackUpRequest ofPoint(String jobid, Point point) {
        return new JobBackUpRequest(Node.getInstance().getMyself().getUuid(), jobid, point);
    }
}
