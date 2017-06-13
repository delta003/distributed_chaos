package com.mbakovic.kids.helper;

import com.google.api.client.http.*;
import com.google.api.client.http.javanet.NetHttpTransport;
import com.google.api.client.http.json.JsonHttpContent;
import com.google.api.client.json.JsonFactory;
import com.google.api.client.json.JsonObjectParser;
import com.google.api.client.json.jackson.JacksonFactory;
import com.mbakovic.kids.model.IPAndPort;
import com.mbakovic.kids.model.Status;
import com.mbakovic.kids.request.*;
import com.mbakovic.kids.response.*;
import org.apache.log4j.Logger;

public final class HttpHelper {
    private static final long RETRY_DELAY = 1000;
    private static Logger log = Logger.getLogger(HttpHelper.class);

    private static final HttpTransport HTTP_TRANSPORT = new NetHttpTransport();
    private static final JsonFactory JSON_FACTORY = new JacksonFactory();

    private static HttpHelper ourInstance = new HttpHelper();

    public static HttpHelper getInstance() {
        //return new HttpHelper();
        return ourInstance;
    }

    private HttpRequestFactory requestFactory;

    private HttpHelper() {
        requestFactory =
            HTTP_TRANSPORT.createRequestFactory(request -> request.setParser(new JsonObjectParser(JSON_FACTORY)));
    }

    public HttpRequestFactory getRequestFactory() {
        return requestFactory;
    }

    // API wrappers

    public IPAndPortAndUUIDResponse bootstrapHelloWithRetry(IPAndPort ipAndPort, IPAndPortRequest request) {
        IPAndPortAndUUIDResponse response;
//      java.io.OutputStream os = new FileOutputStream(new File("/Users/mbakovic/Desktop/log.json"));
//      new JsonHttpContent(JSON_FACTORY, request).writeTo(os);
        do {
            try {
                HttpRequest httpRequest = getRequestFactory().buildPostRequest(
                        new GenericUrl("http://" + ipAndPort.toString() + "/api/hello"),
                        new JsonHttpContent(JSON_FACTORY, request)
                );
                response = httpRequest.execute().parseAs(IPAndPortAndUUIDResponse.class);
            } catch (Exception e) {
                log.error("bootstrapHello failed: " + e.getMessage());
                return null;
            }
            if (response.getStatus() == Status.WAIT) {
                sleep(RETRY_DELAY);
            }
        } while (response.getStatus() == Status.WAIT);
        return response;
    }

    public BootstrapResetResponse bootstrapReset(IPAndPort ipAndPort) {
        try {
            HttpRequest httpRequest = getRequestFactory().buildGetRequest(
                    new GenericUrl("http://" + ipAndPort.toString() + "/api/reset")
            );
            return httpRequest.execute().parseAs(BootstrapResetResponse.class);
        } catch (Exception e) {
            log.error("bootstrapReset failed: " + e.getMessage());
            return null;
        }
    }

    public StatusResponse bootstrapResetDone(IPAndPort ipAndPort) {
        try {
            HttpRequest httpRequest = getRequestFactory().buildGetRequest(
                    new GenericUrl("http://" + ipAndPort.toString() + "/api/reset_done")
            );
            return httpRequest.execute().parseAs(StatusResponse.class);
        } catch (Exception e) {
            log.error("bootstrapResetDone failed: " + e.getMessage());
            return null;
        }
    }

    public StatusResponse basicOk(IPAndPort ipAndPort) {
        try {
            HttpRequest httpRequest = getRequestFactory().buildGetRequest(
                    new GenericUrl("http://" + ipAndPort.toString() + "/api/basic/ok")
            );
            return httpRequest.execute().parseAs(StatusResponse.class);
        } catch (Exception e) {
            log.error("basicOk failed: " + e.getMessage());
            return null;
        }
    }

    public IPAndPortAndUUIDResponse basicInfo(IPAndPort ipAndPort) {
        try {
            HttpRequest httpRequest = getRequestFactory().buildGetRequest(
                    new GenericUrl("http://" + ipAndPort.toString() + "/api/basic/info")
            );
            return httpRequest.execute().parseAs(IPAndPortAndUUIDResponse.class);
        } catch (Exception e) {
            log.error("basicInfo failed: " + e.getMessage());
            return null;
        }
    }

    public CheckResponse basicCheck(IPAndPort ipAndPort) {
        try {
            HttpRequest httpRequest = getRequestFactory().buildGetRequest(
                    new GenericUrl("http://" + ipAndPort.toString() + "/api/basic/check")
            );
            return httpRequest.execute().parseAs(CheckResponse.class);
        } catch (Exception e) {
            log.error("basicCheck failed: " + e.getMessage());
            return null;
        }
    }

    public EdgesResponse networkEdgesWithRetry(IPAndPort ipAndPort) {
        EdgesResponse response;
        do {
            try {
                HttpRequest httpRequest = getRequestFactory().buildGetRequest(
                        new GenericUrl("http://" + ipAndPort.toString() + "/api/network/edges")
                );
                response = httpRequest.execute().parseAs(EdgesResponse.class);
            } catch (Exception e) {
                log.error("networkEdgesWithRetry failed: " + e.getMessage());
                return null;
            }
            if (response.getStatus() == Status.WAIT) {
                sleep(RETRY_DELAY);
            }
        } while (response.getStatus() == Status.WAIT);
        return response;
    }

    public EdgeResponse networkGetEdgeWithRetry(IPAndPort ipAndPort, EdgeTypeRequest request) {
        EdgeResponse response;
        do {
            try {
                HttpRequest httpRequest = getRequestFactory().buildPostRequest(
                        new GenericUrl("http://" + ipAndPort.toString() + "/api/network/get_edge"),
                        new JsonHttpContent(JSON_FACTORY, request)

                );
                response = httpRequest.execute().parseAs(EdgeResponse.class);
            } catch (Exception e) {
                log.error("networkGetEdgeWithRetry failed: " + e.getMessage());
                return null;
            }
            if (response.getStatus() == Status.WAIT) {
                sleep(RETRY_DELAY);
            }
        } while (response.getStatus() == Status.WAIT);
        return response;
    }

    public OldEdgeResponse networkSetEdgeWithRetry(IPAndPort ipAndPort, EdgeRequest request) {
        OldEdgeResponse response;
        do {
            try {
                HttpRequest httpRequest = getRequestFactory().buildPostRequest(
                        new GenericUrl("http://" + ipAndPort.toString() + "/api/network/set_edge"),
                        new JsonHttpContent(JSON_FACTORY, request)
                );
                response = httpRequest.execute().parseAs(OldEdgeResponse.class);
            } catch (Exception e) {
                log.error("networkSetEdgeWithRetry failed: " + e.getMessage());
                return null;
            }
            if (response.getStatus() == Status.WAIT) {
                sleep(RETRY_DELAY);
            }
        } while (response.getStatus() == Status.WAIT);
        return response;
    }

    public AdoptResponse networkAdoptWithRetry(IPAndPort ipAndPort, AdoptRequest request) {
        AdoptResponse response;
        do {
            try {
                HttpRequest httpRequest = getRequestFactory().buildPostRequest(
                        new GenericUrl("http://" + ipAndPort.toString() + "/api/network/adopt"),
                        new JsonHttpContent(JSON_FACTORY, request)

                );
                response = httpRequest.execute().parseAs(AdoptResponse.class);
            } catch (Exception e) {
                log.error("networkAdoptWithRetry failed: " + e.getMessage());
                return null;
            }
            if (response.getStatus() == Status.WAIT) {
                sleep(RETRY_DELAY);
            }
        } while (response.getStatus() == Status.WAIT);
        return response;
    }

    public EdgesResponse networkResetWithRetry(IPAndPort ipAndPort) {
        EdgesResponse response;
        do {
            try {
                HttpRequest httpRequest = getRequestFactory().buildGetRequest(
                        new GenericUrl("http://" + ipAndPort.toString() + "/api/network/reset")
                );
                response = httpRequest.execute().parseAs(EdgesResponse.class);
            } catch (Exception e) {
                log.error("networkResetWithRetry failed: " + e.getMessage());
                return null;
            }
            if (response.getStatus() == Status.WAIT) {
                sleep(RETRY_DELAY);
            }
        } while (response.getStatus() == Status.WAIT);
        return response;
    }

    public EdgesResponse jobsCreateWithRetry(IPAndPort ipAndPort, String uuid, JobRequest request) {
        EdgesResponse response;
        do {
            try {
                HttpRequest httpRequest = getRequestFactory().buildPostRequest(
                        new GenericUrl("http://" + ipAndPort.toString() + "/api/jobs/add/" + uuid),
                        new JsonHttpContent(JSON_FACTORY, request)
                );
                response = httpRequest.execute().parseAs(EdgesResponse.class);
            } catch (Exception e) {
                log.error("jobsCreateWithRetry failed: " + e.getMessage());
                return null;
            }
            if (response.getStatus() == Status.WAIT) {
                sleep(RETRY_DELAY);
            }
        } while (response.getStatus() == Status.WAIT);
        return response;
    }

    public EdgesResponse jobsRemoveWithRetry(IPAndPort ipAndPort, String uuid) {
        EdgesResponse response;
        do {
            try {
                HttpRequest httpRequest = getRequestFactory().buildGetRequest(
                        new GenericUrl("http://" + ipAndPort.toString() + "/api/jobs/remove/" + uuid)
                );
                response = httpRequest.execute().parseAs(EdgesResponse.class);
            } catch (Exception e) {
                log.error("jobsRemoveWithRetry failed: " + e.getMessage());
                return null;
            }
            if (response.getStatus() == Status.WAIT) {
                sleep(RETRY_DELAY);
            }
        } while (response.getStatus() == Status.WAIT);
        return response;
    }

    public JobDataResponse jobsDataWithRetry(IPAndPort ipAndPort, String uuid) {
        JobDataResponse response;
        do {
            try {
                HttpRequest httpRequest = getRequestFactory().buildGetRequest(
                        new GenericUrl("http://" + ipAndPort.toString() + "/api/jobs/data/" + uuid)
                );
                response = httpRequest.execute().parseAs(JobDataResponse.class);
            } catch (Exception e) {
                log.error("jobsDataWithRetry failed: " + e.getMessage());
                return null;
            }
            if (response.getStatus() == Status.WAIT) {
                sleep(RETRY_DELAY);
            }
        } while (response.getStatus() == Status.WAIT);
        return response;
    }

    public AllJobsResponse jobsAllWithRetry(IPAndPort ipAndPort) {
        AllJobsResponse response;
        do {
            try {
                HttpRequest httpRequest = getRequestFactory().buildGetRequest(
                        new GenericUrl("http://" + ipAndPort.toString() + "/api/jobs/all")
                );
                response = httpRequest.execute().parseAs(AllJobsResponse.class);
            } catch (Exception e) {
                log.error("jobsAllWithRetry failed: " + e.getMessage());
                return null;
            }
            if (response.getStatus() == Status.WAIT) {
                sleep(RETRY_DELAY);
            }
        } while (response.getStatus() == Status.WAIT);
        return response;
    }

    public StatusResponse jobsBackup(IPAndPort ipAndPort, JobBackUpRequest request) {
        try {
            HttpRequest httpRequest = getRequestFactory().buildPostRequest(
                    new GenericUrl("http://" + ipAndPort.toString() + "/api/jobs/backup"),
                    new JsonHttpContent(JSON_FACTORY, request)
            );
            return httpRequest.execute().parseAs(StatusResponse.class);
        } catch (Exception e) {
            log.error("jobsBackup failed: " + e.getMessage());
            return null;
        }
    }

    private void sleep(long duration) {
        try {
            Thread.sleep(duration);
        } catch (InterruptedException e) {
            log.warn("HttpHelper sleep interrupted.");
        }
    }
}
