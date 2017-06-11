package com.mbakovic.kids.response;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.google.api.client.util.Key;
import com.mbakovic.kids.model.Bool;
import com.mbakovic.kids.model.Edge;
import com.mbakovic.kids.model.Status;

import java.util.List;

@JsonInclude(JsonInclude.Include.NON_NULL)
public class AdoptResponse extends StatusResponse {

    @Key
    Bool redirect;

    @Key("create_level")
    @JsonProperty("create_level")
    Bool createLevel;

    @Key
    List<Edge> edges;

    @Key
    Edge next;

    public AdoptResponse() {
    }

    public AdoptResponse(Bool redirect, Bool createLevel, List<Edge> edges, Edge next) {
        super(Status.OK, null);
        this.redirect = redirect;
        this.createLevel = createLevel;
        this.edges = edges;
        this.next = next;
    }

    public static AdoptResponse redirect(Edge next) {
        return new AdoptResponse(Bool.TRUE, null, null, next);
    }

    public static AdoptResponse adopted() {
        return new AdoptResponse(Bool.FALSE, Bool.FALSE, null, null);
    }

    public static AdoptResponse adoptedWithCreateLevel(List<Edge> edges) {
        return new AdoptResponse(Bool.FALSE, Bool.TRUE, edges, null);
    }

    public Bool getRedirect() {
        return redirect;
    }

    public void setRedirect(Bool redirect) {
        this.redirect = redirect;
    }

    public Bool getCreateLevel() {
        return createLevel;
    }

    public void setCreateLevel(Bool createLevel) {
        this.createLevel = createLevel;
    }

    public List<Edge> getEdges() {
        return edges;
    }

    public void setEdges(List<Edge> edges) {
        this.edges = edges;
    }

    public Edge getNext() {
        return next;
    }

    public void setNext(Edge next) {
        this.next = next;
    }

    @Override
    public String toString() {
        return "AdoptResponse{" +
                "redirect=" + redirect +
                ", createLevel=" + createLevel +
                ", edges=" + edges +
                ", next=" + next +
                '}';
    }
}
