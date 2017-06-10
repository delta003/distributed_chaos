package com.mbakovic.kids.background;

import com.mbakovic.kids.core.Node;
import com.mbakovic.kids.helper.HttpHelper;
import com.mbakovic.kids.model.*;
import com.mbakovic.kids.request.AdoptRequest;
import com.mbakovic.kids.request.EdgeRequest;
import com.mbakovic.kids.request.EdgeTypeRequest;
import com.mbakovic.kids.request.IPAndPortRequest;
import com.mbakovic.kids.response.*;
import org.apache.log4j.Logger;

import java.util.List;
import java.util.stream.Collectors;

public class NetworkJoiner implements Runnable {
    private static Logger log = Logger.getLogger(NetworkJoiner.class);

    // This must finish and release Node lock
    @Override
    public void run() {
        // Join network
        join();

        // Fetch jobs
        fetchJobs();

        // Release lock
        Node.getInstance().release();
    }

    private void join() {
        // Contact bootstrap
        IPAndPortAndUUIDResponse bootstrapResponse = HttpHelper.getInstance().bootstrapHello(
                Node.getInstance().getBootstrap(), new IPAndPortRequest(
                        Node.getInstance().getMyself().getIp(), Node.getInstance().getMyself().getPort()));
        if (bootstrapResponse == null) {
            log.error("Bootstrap unreachable.");
            return;
        }
        if (bootstrapResponse.getStatus() == Status.ERROR) {
            log.error("Bootstrap responded with error: " + bootstrapResponse.getMessage());
            return;
        }
        // Setting UUID if needed
        log.info(String.format("Bootstrap gave me uuid %s", bootstrapResponse.getUuid()));
        if (Node.getInstance().getMyself().getUuid() == null) {
            log.info(String.format("Taking uuid %s", bootstrapResponse.getUuid()));
            Node.getInstance().getMyself().setUuid(bootstrapResponse.getUuid());
        }
        // If IP is null, I'm the first
        if (bootstrapResponse.getIp() == null) {
            log.info("I'm the first one.");
            Node.getInstance().replaceEdge(Edge.meAs(EdgeType.PREV));
            Node.getInstance().replaceEdge(Edge.meAs(EdgeType.NEXT));
            return;
        }
        // Getting parent of contact
        log.info(String.format("Bootstrap said to contact %s:%s",
                bootstrapResponse.getIp(), bootstrapResponse.getPort()));
        EdgesResponse contactEdges = HttpHelper.getInstance().networkEdgesWithRetry(
                new IPAndPort(bootstrapResponse.getIp(), bootstrapResponse.getPort()));
        if (contactEdges == null) {
            log.error("Contact unreachable.");
            return;
        }
        if (contactEdges.getStatus() == Status.ERROR) {
            log.error("Contact parent responded with error: " + contactEdges.getMessage());
            return;
        }
        // Parent is null, I'm maybe in top circle or in second one
        Edge parentEdge = null, prevEdge = null, nextEdge = null;
        for (Edge e : contactEdges.getEdges()) {
            if (e.getType() == EdgeType.PARENT) {
                parentEdge = e;
            } else if (e.getType() == EdgeType.PREV) {
                prevEdge = e;
            } else if (e.getType() == EdgeType.NEXT) {
                nextEdge = e;
            }
        }

        // Get uuid of main contact
        IPAndPortAndUUIDResponse mainContactInfo = HttpHelper.getInstance().basicInfo(
                new IPAndPort(bootstrapResponse.getIp(), bootstrapResponse.getPort()));
        if (mainContactInfo == null) {
            log.error("Main contact unreachable for info.");
            return;
        }
        if (mainContactInfo.getStatus() == Status.ERROR) {
            log.error("Error getting main contact info: " + mainContactInfo.getMessage());
            return;
        }
        IPAndPortAndUUID mainContact = new IPAndPortAndUUID(
                mainContactInfo.getIp(), mainContactInfo.getPort(), mainContactInfo.getUuid());

        if (parentEdge == null) {
            log.info("I'm in top circle.");
            joinTopCircle(mainContact, prevEdge, nextEdge);
        } else {
            log.info("Joining bigger network.");
            joinNetwork(mainContact, parentEdge);
        }
    }

    private void joinTopCircle(IPAndPortAndUUID mainContact,
                               Edge mainContactPrevEdge, Edge mainContactNextEdge) {
        if (mainContactPrevEdge == null || mainContactNextEdge == null) {
            log.error(String.format("Something is wrong with %s:%s, prev or next edge is null",
                    mainContact.getIp(), mainContact.getPort()));
            return;
        }
        log.info("Main contact prev uuid: " + mainContactPrevEdge.getUuid());
        log.info("Main contact next uuid: " + mainContactNextEdge.getUuid());
        if (mainContactPrevEdge.getUuid().equals(mainContactNextEdge.getUuid())) {
            // Node joins top level circle
            log.info("Joining top circle...");
            joinCircle(mainContact);
        } else {
            // Start second circle
            log.info("Starting second circle...");
            AdoptResponse firstAdopt = HttpHelper.getInstance().networkAdoptWithRetry(
                    new IPAndPort(mainContact.getIp(), mainContact.getPort()),
                    AdoptRequest.adoptMeWithoutRedirect());
            if (firstAdopt == null) {
                log.error("Main contact unreachable for adopt.");
                return;
            }
            if (firstAdopt.getStatus() == Status.ERROR) {
                log.error("Main contact failed to adopt for: " + firstAdopt.getMessage());
                return;
            }

            // Add my edges
            Node.getInstance().replaceEdge(new Edge(
                    mainContact.getIp(), mainContact.getPort(), mainContact.getUuid(), EdgeType.PARENT
            ));
            Node.getInstance().replaceEdge(Edge.meAs(EdgeType.PREV));
            Node.getInstance().replaceEdge(Edge.meAs(EdgeType.NEXT));

            log.info("Second circle stared.");
        }
    }

    private void joinNetwork(IPAndPortAndUUID mainContact, Edge parentEdge) {
        Edge myParent = null;
        AdoptResponse firstAdopt = HttpHelper.getInstance().networkAdoptWithRetry(
                new IPAndPort(parentEdge.getIp(), parentEdge.getPort()),
                AdoptRequest.adoptMeWithRedirect());
        if (firstAdopt == null) {
            log.error("Parent contact unreachable.");
            return;
        }
        if (firstAdopt.getRedirect() == null) {
            log.error("Parent contact responded without redirect.");
            return;
        }

        if (firstAdopt.getRedirect().toBoolean()) {
            // Redirected first time
            log.info("I'm redirected");
            Edge next = firstAdopt.getNext();
            if (next == null) {
                log.error("Contact responded without next but redirected.");
                return;
            }
            AdoptResponse secondAdopt = HttpHelper.getInstance().networkAdoptWithRetry(
                    new IPAndPort(next.getIp(), next.getPort()),
                    AdoptRequest.adoptMeWithoutRedirect());
            if (secondAdopt == null) {
                log.error("Second contact unreachable.");
                return;
            }
            if (secondAdopt.getRedirect() == null) {
                log.error("Second contact responded without redirect.");
                return;
            }
            if (secondAdopt.getRedirect().toBoolean()) {
                log.error("Second contact redirected although asked not to.");
                return;
            }
            myParent = next;
            firstAdopt = secondAdopt;
        } else {
            myParent = parentEdge;
        }

        // Add parent
        Node.getInstance().replaceEdge(new Edge(
                myParent.getIp(), myParent.getPort(), myParent.getUuid(), EdgeType.PARENT
        ));

        // Check not create level
        if (!firstAdopt.getCreateLevel().toBoolean()) {
            joinCircle(new IPAndPortAndUUID(mainContact.getIp(), mainContact.getPort(), mainContact.getUuid()));
            return;
        }

        // New level should be created
        createNewLevel(myParent, firstAdopt, mainContact);

        // Log something happy :)
        log.info("Complex join completed :)");
    }

    private void createNewLevel(Edge myParent, AdoptResponse adoptResponse, IPAndPortAndUUID mainContact) {
        log.info("Creating new level...");
        List<Edge> childEdges = adoptResponse.getEdges().stream()
                .filter(x -> x.getType() == EdgeType.CHILD)
                .collect(Collectors.toList());

        if (childEdges.size() != 5) {
            log.error(String.format("Responded with create level but without 5 children - %s.%s",
                    myParent.getIp(), myParent.getPort()));
            return;
        }

        log.info("ChildEdges: " + childEdges.toString());
        Edge thisPrev0 = childEdges.get(0);  // This one gets two new children
        Edge thisPrev1 = childEdges.get(1);  // This one gets one child
        Edge thisPrev2 = childEdges.get(2);  // First node in new level
        Edge thisPrev3 = childEdges.get(3);  // This is mainContact

        // Exclude thisPrev2 and thisPrev3 from layer
        EdgeResponse newNextResponse = HttpHelper.getInstance().networkGetEdgeWithRetry(
                new IPAndPort(mainContact.getIp(), mainContact.getPort()),
                new EdgeTypeRequest(EdgeType.NEXT));
        if (newNextResponse == null) {
            log.error("Main contact unreachable for get next.");
            return;
        }
        if (newNextResponse.getStatus() == Status.ERROR) {
            log.error("Main contact get next failed for: " + newNextResponse.getMessage());
            return;
        }
        if (newNextResponse.getEdge() == null) {
            log.error(String.format("Main contact responded on get next without next - %s:%s",
                    mainContact.getIp(), mainContact.getPort()));
            return;
        }
        Edge newNext = newNextResponse.getEdge();

        EdgeResponse newPrevResponse = HttpHelper.getInstance().networkGetEdgeWithRetry(
                new IPAndPort(thisPrev2.getIp(), thisPrev2.getPort()),
                new EdgeTypeRequest(EdgeType.PREV));
        if (newPrevResponse == null) {
            log.error("thisPrev2 unreachable for get prev.");
            return;
        }
        if (newPrevResponse.getStatus() == Status.ERROR) {
            log.error("thisPrev2 get prev failed for: " + newPrevResponse.getMessage());
            return;
        }
        if (newPrevResponse.getEdge() == null) {
            log.error(String.format("thisPrev2 responded on get next without next - %s:%s",
                    thisPrev2.getIp(), thisPrev2.getPort()));
            return;
        }
        Edge newPrev = newPrevResponse.getEdge();

        OldEdgeResponse newNextPrevResponse = HttpHelper.getInstance().networkSetEdgeWithRetry(
                new IPAndPort(newNext.getIp(), newNext.getPort()), new EdgeRequest(new Edge(
                        newPrev.getIp(), newPrev.getPort(), newPrev.getUuid(), EdgeType.PREV)));
        if (newNextPrevResponse == null) {
            log.error("newNextPrevResponse unreachable for set prev.");
            return;
        }
        if (newNextPrevResponse.getStatus() == Status.ERROR) {
            log.error("newNextPrevResponse failed to set prev for: " + newNextPrevResponse.getMessage());
            return;
        }

        OldEdgeResponse newPrevNextResponse = HttpHelper.getInstance().networkSetEdgeWithRetry(
                new IPAndPort(newPrev.getIp(), newPrev.getPort()), new EdgeRequest(new Edge(
                        newNext.getIp(), newNext.getPort(), newNext.getUuid(), EdgeType.NEXT)));
        if (newPrevNextResponse == null) {
            log.error("newPrevNextResponse unreachable for set next.");
            return;
        }
        if (newPrevNextResponse.getStatus() == Status.ERROR) {
            log.error("newPrevNextResponse failed to set next for: " + newPrevNextResponse.getMessage());
            return;
        }

        // Change parents
        changeParentsForNewLayer(thisPrev0, thisPrev1, thisPrev2, thisPrev3);

        // Make circle in new layer
        OldEdgeResponse oldPrev2NextResponse = HttpHelper.getInstance().networkSetEdgeWithRetry(
                new IPAndPort(thisPrev2.getIp(),thisPrev2.getPort()), new EdgeRequest(Edge.meAs(EdgeType.PREV)));
        if (oldPrev2NextResponse == null) {
            log.error("thisPrev2 unreachable for set prev.");
            return;
        }
        if (oldPrev2NextResponse.getStatus() == Status.ERROR) {
            log.error("thisPrev2 failed to set prev for: " + oldPrev2NextResponse.getMessage());
            return;
        }

        OldEdgeResponse oldPrev3NextResponse = HttpHelper.getInstance().networkSetEdgeWithRetry(
                new IPAndPort(thisPrev3.getIp(),thisPrev3.getPort()), new EdgeRequest(Edge.meAs(EdgeType.NEXT)));
        if (oldPrev3NextResponse == null) {
            log.error("thisPrev3 unreachable for set prev.");
            return;
        }
        if (oldPrev3NextResponse.getStatus() == Status.ERROR) {
            log.error("thisPrev3 failed to set prev for: " + oldPrev3NextResponse.getMessage());
            return;
        }

        // Set my edges
        Node.getInstance().replaceEdge(new Edge(
                thisPrev3.getIp(), thisPrev3.getPort(), thisPrev3.getUuid(), EdgeType.PREV
        ));
        Node.getInstance().replaceEdge(new Edge(
                thisPrev2.getIp(), thisPrev2.getPort(), thisPrev2.getUuid(), EdgeType.NEXT
        ));

        log.info("New level created.");
    }

    private void changeParentsForNewLayer(Edge parent1, Edge parent2, Edge child1, Edge child2) {
        parent1.setType(EdgeType.PARENT);
        parent2.setType(EdgeType.PARENT);
        child1.setType(EdgeType.CHILD);
        child2.setType(EdgeType.CHILD);

        AdoptResponse parent1AdoptChild1Response = HttpHelper.getInstance().networkAdoptWithRetry(
                new IPAndPort(parent1.getIp(), parent1.getPort()),
                new AdoptRequest(child1, Bool.FALSE));
        if (parent1AdoptChild1Response == null) {
            log.error("Parent1 not responding for child1 adoption.");
            return;
        }
        if (parent1AdoptChild1Response.getStatus() == Status.ERROR) {
            log.error("Parent1 failed to adopt child1 because: " + parent1AdoptChild1Response.getMessage());
            return;
        }

        OldEdgeResponse child1SetParentResponse = HttpHelper.getInstance().networkSetEdgeWithRetry(
                new IPAndPort(child1.getIp(), child1.getPort()),
                new EdgeRequest(parent1));
        if (child1SetParentResponse == null) {
            log.error("Child1 not responding for set parent.");
            return;
        }
        if (child1SetParentResponse.getStatus() == Status.ERROR) {
            log.error("Child1 failed to set parent because: " + child1SetParentResponse.getMessage());
            return;
        }

        AdoptResponse parent1AdoptChild2Response = HttpHelper.getInstance().networkAdoptWithRetry(
                new IPAndPort(parent1.getIp(), parent1.getPort()),
                new AdoptRequest(child2, Bool.FALSE));
        if (parent1AdoptChild2Response == null) {
            log.error("Parent1 not responding for child2 adoption.");
            return;
        }
        if (parent1AdoptChild2Response.getStatus() == Status.ERROR) {
            log.error("Parent1 failed to adopt child2 because: " + parent1AdoptChild2Response.getMessage());
            return;
        }

        OldEdgeResponse child2SetParentResponse = HttpHelper.getInstance().networkSetEdgeWithRetry(
                new IPAndPort(child2.getIp(), child2.getPort()),
                new EdgeRequest(parent1));
        if (child2SetParentResponse == null) {
            log.error("Child2 not responding for set parent.");
            return;
        }
        if (child1SetParentResponse.getStatus() == Status.ERROR) {
            log.error("Child2 failed to set parent because: " + child2SetParentResponse.getMessage());
            return;
        }

        AdoptResponse parent2AdoptChildResponse = HttpHelper.getInstance().networkAdoptWithRetry(
                new IPAndPort(parent2.getIp(), parent2.getPort()),
                AdoptRequest.adoptMeWithoutRedirect());
        if (parent2AdoptChildResponse == null) {
            log.error("Parent2 not responding for my adoption.");
            return;
        }
        if (parent2AdoptChildResponse.getStatus() == Status.ERROR) {
            log.error("Parent2 failed to adopt me because: " + parent2AdoptChildResponse.getMessage());
            return;
        }

        // Set my parent
        Node.getInstance().replaceEdge(parent2);
    }

    private void joinCircle(IPAndPortAndUUID myPrev) {
        log.info(String.format("Joining circle with prev %s:%s",
                myPrev.getIp(), myPrev.getPort()));

        // Set myself as next
        OldEdgeResponse oldEdgeResponse = HttpHelper.getInstance().networkSetEdgeWithRetry(
                myPrev, new EdgeRequest(Edge.meAs(EdgeType.NEXT)));
        if (oldEdgeResponse == null) {
            log.error("My prev unreachable for set next.");
            return;
        }
        if (oldEdgeResponse.getStatus() == Status.ERROR) {
            log.error("My prev failed to set next: " + oldEdgeResponse.getMessage());
            return;
        }
        Edge myNext = oldEdgeResponse.getOldEdge();
        if (myNext == null) {
            log.error("My prev set next returned null.");
            return;
        }
        log.info(String.format("My next is %s:%s", myNext.getIp(), myNext.getPort()));

        // Set myself as prev
        OldEdgeResponse oldEdgeResponse1 = HttpHelper.getInstance().networkSetEdgeWithRetry(
                new IPAndPort(myNext.getIp(), myNext.getPort()),
                new EdgeRequest(Edge.meAs(EdgeType.PREV)));
        if (oldEdgeResponse1 == null) {
            log.error("My next unreachable for set prev.");
            return;
        }
        if (oldEdgeResponse.getStatus() == Status.ERROR) {
            log.error("My next failed to set prev: " + oldEdgeResponse.getMessage());
            return;
        }

        // Set my edges
        Node.getInstance().replaceEdge(new Edge(
                myPrev.getIp(), myPrev.getPort(), myPrev.getUuid(), EdgeType.PREV
        ));
        Node.getInstance().replaceEdge(new Edge(
                myNext.getIp(), myNext.getPort(), myNext.getUuid(), EdgeType.NEXT
        ));

        log.info("Joining circle complete.");
    }

    private void fetchJobs() {
        // TODO
    }
}
