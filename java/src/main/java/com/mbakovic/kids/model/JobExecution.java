package com.mbakovic.kids.model;

import java.util.*;
import java.util.stream.Collectors;

public class JobExecution implements Comparable<JobExecution> {

    private JobWithUUID job;

    private List<JobBackup> backups;

    private Double dWidth;

    private Double dHeight;

    private Double dp;

    private List<PointDouble> startingPoints;

    private List<PointDouble> computedPoints;

    public JobExecution(String width, String height, String p, List<Point> points) {
        job = new JobWithUUID(width, height, p, points);
        dWidth = Double.parseDouble(width);
        dHeight = Double.parseDouble(height);
        dp = Double.parseDouble(p);
        startingPoints = new ArrayList<>();
        startingPoints.addAll(points.stream().map(PointDouble::fromPoint).collect(Collectors.toList()));
        computedPoints = new ArrayList<>();
        backups = new ArrayList<>();
    }

    public JobWithUUID getJob() {
        return job;
    }

    public void setJob(JobWithUUID job) {
        this.job = job;
    }

    public List<JobBackup> getBackups() {
        return backups;
    }

    public void setBackups(List<JobBackup> backups) {
        this.backups = backups;
    }

    public Double getdWidth() {
        return dWidth;
    }

    public void setdWidth(Double dWidth) {
        this.dWidth = dWidth;
    }

    public Double getdHeight() {
        return dHeight;
    }

    public void setdHeight(Double dHeight) {
        this.dHeight = dHeight;
    }

    public Double getDp() {
        return dp;
    }

    public void setDp(Double dp) {
        this.dp = dp;
    }

    public List<PointDouble> getStartingPoints() {
        return startingPoints;
    }

    public void setStartingPoints(List<PointDouble> startingPoints) {
        this.startingPoints = startingPoints;
    }

    public List<PointDouble> getComputedPoints() {
        return computedPoints;
    }

    public void setComputedPoints(List<PointDouble> computedPoints) {
        this.computedPoints = computedPoints;
    }

    private PointDouble randomStaringPoint() {
        Random r = new Random();
        int id = r.nextInt(startingPoints.size());
        return startingPoints.get(id);
    }

    private PointDouble randomPoint() {
        Random r = new Random();
        int x = r.nextInt(dWidth.intValue() + 1);
        int y = r.nextInt(dHeight.intValue() + 1);
        return new PointDouble((double) x, (double) y);
    }

    public PointDouble computePoint() {
        if (computedPoints.isEmpty()) {
            PointDouble newPoint = randomPoint();
            computedPoints.add(newPoint);
            return newPoint;
        }
        PointDouble last = computedPoints.get(computedPoints.size() - 1);
        PointDouble randomPoint = randomPoint();
        PointDouble newPoint = new PointDouble(
                last.x() + dp * (randomPoint.x() - last.x()),
                last.y() + dp * (randomPoint.y() - last.y()));
        computedPoints.add(newPoint);
        return newPoint;
    }

    public List<Point> getPoints() {
        return computedPoints.stream().map(PointDouble::toPoint).collect(Collectors.toList());
    }

    @Override
    public int compareTo(JobExecution o) {
        return job.getUuid().compareTo(o.getJob().getUuid());
    }

    public JobBackup backupWithUUID(String uuid) {
        for (JobBackup backup : backups) {
            if (backup.getUuid().equals(uuid)) {
                return backup;
            }
        }
        return null;
    }

    public void backUpPoint(String uuid, Point point) {
        JobBackup backup = backupWithUUID(uuid);
        if (backup == null) {
            backup = new JobBackup(uuid);
            backups.add(backup);
        }
        backup.getPoints().add(point);
    }

    public String getJobUuid() {
        if (job == null) {
            return null;  // This should never happen
        }
        return job.getUuid();
    }
}