(function () {
    'use strict';

    angular
        .module('app')
        .controller('HomeController', HomeController);

    HomeController.$inject = ['NodeService', '$rootScope' , 'FlashService'];
    function HomeController(NodeService, $rootScope, FlashService) {
        var vm = this;

        vm.jobids = [];
        initController();

        function initController() {
            loadJobIds();
            loadNetwork();
        }

        function loadJobIds() {
            // NodeService.JobsIds()
            //     .then(function (response)) {
            //         vm.jobids = JSON.parse(response.jobids);
            //     });
            vm.jobids = [1, 2, 3, 4];
        }

        function loadNetwork(request) {
            // NodeService.NetworkVisualize()
            //     .then(function (response) {
            //         nodes = JSON.parse(response.nodes);
            //         edges = JSON.parse(response.edges);
            //         showNetwork(nodes, edges);
            //     });
            var nodes = [
                { uuid: "1", ip: "0.0.0.1", port: "8001" },
                { uuid: "2", ip: "0.0.0.2", port: "8002" },
                { uuid: "3", ip: "0.0.0.3", port: "8003" }
            ];
            var edges = [
                { start_uuid: "1", end_uuid: "2", type: "next" },
                { start_uuid: "1", end_uuid: "3", type: "prev" },
                { start_uuid: "2", end_uuid: "3", type: "next" },
                { start_uuid: "2", end_uuid: "1", type: "prev" },
                { start_uuid: "3", end_uuid: "1", type: "next" },
                { start_uuid: "3", end_uuid: "2", type: "prev" }
            ];
            showNetwork(nodes, edges);
        }

        function getColor(type) {
            switch (type) {
                case "parent":
                    return {color:'red'};
                case "prev":
                    return {color:'blue'};
                case "next":
                    return {color:'green'};
                case "child":
                    return {color:'yellow'};
                default:
                    return {color:'gray'};
            }
        }

        function showNetwork(nodes, edges) {
            var dataNodes = [];
            for (var i = 0; i < nodes.length; i++) {
                dataNodes.push({
                    id: parseInt(nodes[i].uuid),
                    label: "" + nodes[i].uuid + " (" + nodes[i].ip + ":" + nodes[i].port + ")"
                });
            }
            var nodes = new vis.DataSet(dataNodes);

            var dataEdges = [];
            for (var i = 0; i < edges.length; i++) {
                dataEdges.push({
                    from: parseInt(edges[i].start_uuid),
                    to: parseInt(edges[i].end_uuid),
                    arrows:'to',
                    color: getColor(edges[i].type)
                });
            }
            var edges = new vis.DataSet(dataEdges);

            var container = document.getElementById('mynetwork');
            var data = {
                nodes: nodes,
                edges: edges
            };
            var options = {};
            var network = new vis.Network(container, data, options);
        }
    }

})();
