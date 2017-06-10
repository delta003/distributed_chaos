package com.mbakovic.kids;

import com.mbakovic.kids.background.JobExecutor;
import com.mbakovic.kids.background.NodeHealthcheck;
import com.mbakovic.kids.core.Node;
import com.mbakovic.kids.model.IPAndPort;
import com.mbakovic.kids.model.IPAndPortAndUUID;
import com.mbakovic.kids.resources.BasicResource;
import com.mbakovic.kids.resources.BootstrapResource;
import com.mbakovic.kids.resources.NetworkResource;
import com.mbakovic.kids.resources.PingResource;
import io.dropwizard.Application;
import io.dropwizard.bundles.assets.ConfiguredAssetsBundle;
import io.dropwizard.setup.Bootstrap;
import io.dropwizard.setup.Environment;
import org.apache.log4j.Logger;

import java.net.InetAddress;

public final class AppApplication extends Application<AppConfiguration> {
    private static Logger log = Logger.getLogger(AppApplication.class);

    public static void main(final String[] args) throws Exception {
        new AppApplication().run(args);
    }

    @Override
    public void initialize(final Bootstrap<AppConfiguration> bootstrap) {
        bootstrap.addBundle(new ConfiguredAssetsBundle("/assets", "/", "index.html"));
    }

    @Override
    public void run(final AppConfiguration configuration,
                    final Environment environment) throws Exception {
        // Add ping service
        final PingResource pingResource = new PingResource();
        environment.jersey().register(pingResource);

        if (configuration.getNode().isBootstrap()) {
            // Run as bootstrap

            // Set API
            final BootstrapResource bootstrapResource = new BootstrapResource();
            environment.jersey().register(bootstrapResource);
        } else {
            // Run as node

            // Set myself
            String myIP = InetAddress.getLocalHost().getHostAddress();
            String myPort = configuration.getNode().getPort();
            log.info(String.format("Node %s:%s", myIP, myPort));
            Node.getInstance().setMyself(new IPAndPortAndUUID(myIP, myPort, null));

            // Set bootstrap
            Node.getInstance().setBootstrap(new IPAndPort(
                    configuration.getNode().getBootstrapIP(),
                    configuration.getNode().getBootstrapPort()
            ));

            // Join network
            if (!Node.getInstance().lock()) {
                throw new Exception("Unable to take lock when joining.");
            }
            Node.getInstance().joinNetwork();

            // Start background tasks
            new Thread(new JobExecutor()).start();
            new Thread(new NodeHealthcheck()).start();

            // Set API
            final BasicResource basicResource = new BasicResource();
            environment.jersey().register(basicResource);
            final NetworkResource networkResource = new NetworkResource();
            environment.jersey().register(networkResource);
        }
    }

}
