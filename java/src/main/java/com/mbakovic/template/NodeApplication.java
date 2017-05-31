package com.mbakovic.template;

import com.mbakovic.template.resources.BasicResource;
import io.dropwizard.Application;
import io.dropwizard.bundles.assets.ConfiguredAssetsBundle;
import io.dropwizard.setup.Bootstrap;
import io.dropwizard.setup.Environment;

public final class NodeApplication extends Application<NodeConfiguration> {

    public static void main(final String[] args) throws Exception {
        new NodeApplication().run(args);
    }

    @Override
    public String getName() {
        return "node";
    }

    @Override
    public void initialize(final Bootstrap<NodeConfiguration> bootstrap) {
        bootstrap.addBundle(new ConfiguredAssetsBundle("/assets", "/", "index.html"));
    }

    @Override
    public void run(final NodeConfiguration configuration,
                    final Environment environment) throws ClassNotFoundException {

        // API
        final BasicResource basicResource = new BasicResource();
        environment.jersey().register(basicResource);
    }

}
