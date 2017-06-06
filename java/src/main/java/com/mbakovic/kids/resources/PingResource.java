package com.mbakovic.kids.resources;

import com.mbakovic.kids.api.PingService;

public final class PingResource implements PingService {

    @Override
    public String ping() {
        return "pong";
    }
}
