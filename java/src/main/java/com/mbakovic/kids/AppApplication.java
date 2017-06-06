package com.mbakovic.kids;

import com.mbakovic.kids.core.Node;
import com.mbakovic.kids.model.IPAndPort;
import com.mbakovic.kids.resources.BasicResource;
import com.mbakovic.kids.resources.BootstrapResource;
import com.mbakovic.kids.resources.NetworkResource;
import com.mbakovic.kids.resources.PingResource;
import io.dropwizard.Application;
import io.dropwizard.bundles.assets.ConfiguredAssetsBundle;
import io.dropwizard.setup.Bootstrap;
import io.dropwizard.setup.Environment;

public final class AppApplication extends Application<AppConfiguration> {

    public static void main(final String[] args) throws Exception {
        new AppApplication().run(args);
    }

    @Override
    public void initialize(final Bootstrap<AppConfiguration> bootstrap) {
        bootstrap.addBundle(new ConfiguredAssetsBundle("/assets", "/", "index.html"));
    }

    @Override
    public void run(final AppConfiguration configuration,
                    final Environment environment) throws ClassNotFoundException {

        // Add ping service
        final PingResource pingResource = new PingResource();
        environment.jersey().register(pingResource);

        if (configuration.getNode().isBootstrap()) {
            // Run as bootstrap

            // API
            final BootstrapResource bootstrapResource = new BootstrapResource();
            environment.jersey().register(bootstrapResource);
        } else {
            // Run as node

            // Set bootstrap
            Node.getInstance().setBootstrap(new IPAndPort(
                    configuration.getNode().getBootstrapIP(),
                    configuration.getNode().getBootstrapPort()
            ));
            // TODO
            // Initialize Node and join the network

            // API
            final BasicResource basicResource = new BasicResource();
            environment.jersey().register(basicResource);
            final NetworkResource networkResource = new NetworkResource();
            environment.jersey().register(networkResource);
        }
    }

}
