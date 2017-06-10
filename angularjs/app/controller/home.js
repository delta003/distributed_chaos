(function() {
  'use strict';

  angular.module('app').controller('HomeController', HomeController);

  HomeController.$inject = ['NodeService', '$rootScope', 'FlashService'];
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
      //         vm.jobids = response.jobids;
      //     });
      vm.jobids = [1, 2, 3, 4];
    }

    function loadNetwork(request) {
      NodeService.NetworkVisualize().then(function(response) {
        var nodes = response.nodes;
        var edges = response.edges;
        showNetwork(nodes, edges);
      });
    }

    function getColor(type) {
      switch (type) {
        case "parent":
          return {color: 'red'};
        case "prev":
          return {color: 'blue'};
        case "next":
          return {color: 'green'};
        case "child":
          return {color: '#CCCC00'};
        default:
          return {color: 'gray'};
      }
    }

    function showNetwork(nodes, edges) {
      var dataNodes = [];
      for (var i = 0; i < nodes.length; i++) {
        dataNodes.push({id: parseInt(nodes[i].uuid), label: "" + nodes[i].uuid + " (" + nodes[i].ip + ":" + nodes[i].port + ")"});
      }
      var nodes = new vis.DataSet(dataNodes);

      var dataEdges = [];
      for (var i = 0; i < edges.length; i++) {
        dataEdges.push({from: parseInt(edges[i].start_uuid), to: parseInt(edges[i].end_uuid), arrows: 'to', color: getColor(edges[i].type)});
      }
      var edges = new vis.DataSet(dataEdges);

      var container = document.getElementById('mynetwork');
      var data = {nodes: nodes, edges: edges};
      var options = {};
      var network = new vis.Network(container, data, options);
    }
  }

})();
