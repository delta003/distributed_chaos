package com.mbakovic.kids.model;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonValue;
import com.google.api.client.util.Value;

import java.util.HashMap;
import java.util.Map;

public enum Status {
    @Value("ok")
    OK,

    @Value("error")
    ERROR,

    @Value("wait")
    WAIT;

    private static Map<String, Status> namesMap = new HashMap<>(3);

    static {
        namesMap.put("ok", OK);
        namesMap.put("error", ERROR);
        namesMap.put("wait", WAIT);
    }

    @JsonCreator
    public static Status forValue(String value) {
        return namesMap.get(value.toLowerCase());
    }

    @JsonValue
    public String toValue() {
        for (Map.Entry<String, Status> entry : namesMap.entrySet()) {
            if (entry.getValue() == this)
                return entry.getKey();
        }
        return null; // or fail
    }
}
