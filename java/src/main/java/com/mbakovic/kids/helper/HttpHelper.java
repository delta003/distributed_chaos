package com.mbakovic.kids.helper;

import com.google.api.client.http.*;
import com.google.api.client.http.javanet.NetHttpTransport;
import com.google.api.client.json.JsonFactory;
import com.google.api.client.json.JsonObjectParser;
import com.google.api.client.json.jackson.JacksonFactory;
import com.mbakovic.kids.model.Edge;
import com.mbakovic.kids.model.IPAndPort;
import com.mbakovic.kids.model.Status;
import com.mbakovic.kids.response.EdgesResponse;
import com.mbakovic.kids.response.StatusResponse;
import org.apache.log4j.Logger;

import java.util.List;

public final class HttpHelper {
    private static Logger log = Logger.getLogger(HttpHelper.class);

    private static final HttpTransport HTTP_TRANSPORT = new NetHttpTransport();
    private static final JsonFactory JSON_FACTORY = new JacksonFactory();

    private static HttpHelper ourInstance = new HttpHelper();

    public static HttpHelper getInstance() {
        return ourInstance;
    }

    private HttpRequestFactory requestFactory;

    private HttpHelper() {
        requestFactory =
            HTTP_TRANSPORT.createRequestFactory(new HttpRequestInitializer() {
                @Override
                public void initialize(HttpRequest request) {
                    request.setParser(new JsonObjectParser(JSON_FACTORY));
                }
            });
    }

    public HttpRequestFactory getRequestFactory() {
        return requestFactory;
    }

    // API wrappers

    public StatusResponse bootstrapReset(IPAndPort ipAndPort) {
        try {
            HttpRequest httpRequest = HttpHelper.getInstance().getRequestFactory().buildGetRequest(
                    new GenericUrl("http://" + ipAndPort.toString() + "/api/reset")
            );
            return httpRequest.execute().parseAs(StatusResponse.class);
        } catch (Exception e) {
            log.error("bootstrapReset failed: " + e.getMessage());
            return null;
        }
    }

    public StatusResponse basicOk(IPAndPort ipAndPort) {
        try {
            HttpRequest httpRequest = HttpHelper.getInstance().getRequestFactory().buildGetRequest(
                    new GenericUrl(ipAndPort.getIp() + ":" + ipAndPort.getPort() + "/api/basic/ok")
            );
            return httpRequest.execute().parseAs(StatusResponse.class);
        } catch (Exception e) {
            log.error("basicOk failed: " + e.getMessage());
            return null;
        }
    }

    public EdgesResponse networkEdgesWithRepeat(IPAndPort ipAndPort) {
        EdgesResponse response;
        do {
            try {
                HttpRequest httpRequest = HttpHelper.getInstance().getRequestFactory().buildGetRequest(
                        new GenericUrl(ipAndPort.getIp() + ":" + ipAndPort.getPort() + "/api/network/edges")
                );
                response = httpRequest.execute().parseAs(EdgesResponse.class);
            } catch (Exception e) {
                log.error("networkEdgesWithRepeat failed: " + e.getMessage());
                return null;
            }
        } while (response.getStatus() == Status.WAIT);
        return response;
    }
}
