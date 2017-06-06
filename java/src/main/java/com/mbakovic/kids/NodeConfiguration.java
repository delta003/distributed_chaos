package com.mbakovic.kids;

import com.fasterxml.jackson.annotation.JsonProperty;
import org.hibernate.validator.constraints.NotEmpty;

public final class NodeConfiguration {

    @NotEmpty
    private String type;

    @NotEmpty
    private String bootstrapIP;

    @NotEmpty
    private String bootstrapPort;

    @JsonProperty
    public String getType() {
        return type;
    }

    @JsonProperty
    public void setType(String type) {
        this.type = type;
    }

    @JsonProperty
    public String getBootstrapIP() {
        return bootstrapIP;
    }

    @JsonProperty
    public void setBootstrapIP(String bootstrapIP) {
        this.bootstrapIP = bootstrapIP;
    }

    @JsonProperty
    public String getBootstrapPort() {
        return bootstrapPort;
    }

    @JsonProperty
    public void setBootstrapPort(String bootstrapPort) {
        this.bootstrapPort = bootstrapPort;
    }

    public boolean isBootstrap() {
        return type.equals("bootstrap");
    }
}
