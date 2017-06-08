package com.mbakovic.kids.resources;

import com.mbakovic.kids.core.Bootstrap;
import com.mbakovic.kids.api.BootstrapService;
import com.mbakovic.kids.model.IPAndPortAndUUID;
import com.mbakovic.kids.request.IPAndPortRequest;
import com.mbakovic.kids.response.IPAndPortAndUUIDResponse;
import com.mbakovic.kids.response.StatusResponse;
import org.apache.log4j.Logger;

public final class BootstrapResource implements BootstrapService {
    private static Logger log = Logger.getLogger(BootstrapResource.class);

    private static String lockErrorMsg = "Failed to take lock.";

    @Override
    public StatusResponse hello(IPAndPortRequest request) {
        if (!Bootstrap.getInstance().lock()) {
            log.warn(lockErrorMsg);
            return StatusResponse.ofWait();
        }
        IPAndPortAndUUID lastNode = Bootstrap.getInstance().getLastNode();
        IPAndPortAndUUID newNode = new IPAndPortAndUUID(request.getIp(), request.getPort(),
                String.valueOf(Bootstrap.getInstance().getUUID()));
        IPAndPortAndUUIDResponse response = new IPAndPortAndUUIDResponse(
                lastNode.getIp(), lastNode.getPort(), newNode.getUuid());
        Bootstrap.getInstance().setLastNode(newNode);
        Bootstrap.getInstance().release();
        log.info(String.format("New node %s:%s joined with UUID %s",
                newNode.getIp(), newNode.getPort(), newNode.getUuid()));
        return response;
    }

    @Override
    public StatusResponse reset() {
        if (!Bootstrap.getInstance().lock()) {
            log.warn(lockErrorMsg);
            return StatusResponse.ofWait();
        }
        Bootstrap.getInstance().reset();
        Bootstrap.getInstance().release();
        log.info("Bootstrap restarted.");
        return StatusResponse.ofOk();
    }
}
