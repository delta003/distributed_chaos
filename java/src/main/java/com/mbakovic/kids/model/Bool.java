package com.mbakovic.kids.model;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonValue;
import com.google.api.client.util.Value;

import java.util.HashMap;
import java.util.Map;

public enum Bool {
    @Value("true")
    TRUE,

    @Value("false")
    FALSE;

    private static Map<String, Bool> namesMap = new HashMap<>(2);

    static {
        namesMap.put("true", TRUE);
        namesMap.put("false", FALSE);
    }

    @JsonCreator
    public static Bool forValue(String value) {
        return namesMap.get(value.toLowerCase());
    }

    @JsonValue
    public String toValue() {
        for (Map.Entry<String, Bool> entry : namesMap.entrySet()) {
            if (entry.getValue() == this)
                return entry.getKey();
        }
        return null; // or fail
    }

    public boolean toBoolean() {
        return (this == TRUE);
    }
}
