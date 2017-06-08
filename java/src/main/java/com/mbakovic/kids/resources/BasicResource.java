package com.mbakovic.kids.resources;

import com.mbakovic.kids.model.Bool;
import com.mbakovic.kids.helper.HttpHelper;
import com.mbakovic.kids.core.Node;
import com.mbakovic.kids.api.BasicService;
import com.mbakovic.kids.model.IPAndPort;
import com.mbakovic.kids.request.IPAndPortRequest;
import com.mbakovic.kids.response.CheckResponse;
import com.mbakovic.kids.response.IPAndPortAndUUIDResponse;
import com.mbakovic.kids.response.StatusResponse;
import org.apache.log4j.Logger;

public final class BasicResource implements BasicService {
    private static Logger log = Logger.getLogger(BasicResource.class);

    @Override
    public StatusResponse ok() {
        return StatusResponse.ofOk();
    }

    @Override
    public StatusResponse info() {
        return new IPAndPortAndUUIDResponse(Node.getInstance().getMyself());
    }

    @Override
    public StatusResponse check(IPAndPortRequest request) {
        if (request.getIp() == null || request.getPort() == null) {
            return StatusResponse.ofError("IP or port is null.");
        }
        StatusResponse response = HttpHelper.getInstance().basicOk(new IPAndPort(request.getIp(), request.getPort()));
        log.info(String.format("Check for node %s:%s", request.getIp(), request.getPort()));
        if (response == null) {
            return new CheckResponse(Bool.FALSE);
        }
        return new CheckResponse(Bool.TRUE);
    }
}
