package com.mbakovic.template.resources;

import com.mbakovic.template.api.BasicService;
import com.mbakovic.template.core.StatusResponse;

public final class BasicResource implements BasicService {

    public BasicResource() {

    }

    @Override
    public StatusResponse ok() {
        return new StatusResponse("OK", null);
    }
}
