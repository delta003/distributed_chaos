package com.mbakovic.kids.resources;

import com.mbakovic.kids.core.Bootstrap;
import com.mbakovic.kids.api.BootstrapService;
import com.mbakovic.kids.model.IPAndPort;
import com.mbakovic.kids.request.IPAndPortRequest;
import com.mbakovic.kids.response.IPAndPortAndUUIDResponse;
import com.mbakovic.kids.response.StatusResponse;

public final class BootstrapResource implements BootstrapService {

    public BootstrapResource() {

    }

    @Override
    public StatusResponse hello(IPAndPortRequest request) {
        if (Bootstrap.getInstance().lock()) {
            IPAndPort lastNode = Bootstrap.getInstance().getLastNode();
            IPAndPortAndUUIDResponse response = new IPAndPortAndUUIDResponse(lastNode.getIp(), lastNode.getPort(),
                    String.valueOf(Bootstrap.getInstance().getUUID()));
            Bootstrap.getInstance().setLastNode(new IPAndPort(request.getIp(), request.getPort()));
            Bootstrap.getInstance().release();
            return response;
        } else {
            return StatusResponse.ofWait();
        }
    }

    @Override
    public StatusResponse reset() {
        if (Bootstrap.getInstance().lock()) {
            Bootstrap.getInstance().reset();
            Bootstrap.getInstance().release();
            return StatusResponse.ofOk();
        } else {
            return StatusResponse.ofWait();
        }

    }
}
