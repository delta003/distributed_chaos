package com.mbakovic.kids.model;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonValue;
import com.google.api.client.util.Value;

import java.util.HashMap;
import java.util.Map;

public enum EdgeType {
    @Value("parent")
    PARENT,

    @Value("prev")
    PREV,

    @Value("next")
    NEXT,

    @Value("child")
    CHILD;

    private static Map<String, EdgeType> namesMap = new HashMap<>(4);

    static {
        namesMap.put("parent", PARENT);
        namesMap.put("prev", PREV);
        namesMap.put("next", NEXT);
        namesMap.put("child", CHILD);
    }

    @JsonCreator
    public static EdgeType forValue(String value) {
        return namesMap.get(value.toLowerCase());
    }

    @JsonValue
    public String toValue() {
        for (Map.Entry<String, EdgeType> entry : namesMap.entrySet()) {
            if (entry.getValue() == this)
                return entry.getKey();
        }
        return null; // or fail
    }
}
