(function() {
  'use strict';

  angular.module('app').controller('JobController', JobController);

  JobController.$inject = ['NodeService', '$location', '$routeParams', '$rootScope', 'FlashService'];
  function JobController(NodeService, $location, $routeParams, $rootScope, FlashService) {
    var vm = this;

    vm.killJob = killJob;

    vm.jobid = '-';
    vm.colors = [];
    vm.killresponse = '';
    initController();

    function initController() {
      vm.jobid = $routeParams.jobid;
      NodeService.JobsVisualize(vm.jobid).then(function(response) {
        console.log("/api/jobs/visualize/" + vm.jobid);
        console.log(response);
        if (response.status == "ok") {
          var startingPoints = response.startingpoints;
          var points = response.points;
          var w = parseInt(response.width);
          var h = parseInt(response.height);
          showJob(startingPoints, points, w, h);
        } else if (response.status == "wait") {
          alert("Cvor zauzet!");
        }
      });
      // var startingPoints = [{x: "10.1", y: "10.1"}, {x: "20.1", y: "200.1"}, {x: "260.2", y: "18.2"}];
      // var points = [
      //   {uuid: "0", points: [{x: "100.5", y: "100.5"}, {x: "130.0", y: "130.0"}]},
      //   {uuid: "1", points: [{x: "110.32", y: "110.5"}, {x: "86.0", y: "89.5"}]}
      // ];
      // showJob(startingPoints, points, 300, 300);
    }

    function killJob() {
      console.log('Killing job ' + vm.jobid);
      NodeService.JobsKill(vm.jobid).then(function(response) {
        if (response.status == 'ok') {
          $location.path('/');
        } else {
          vm.killresponse = response;
        }
      });
    }

    function getRandomColor() {
      var letters = '0123456789ABCDEF';
      var color = '#';
      for (var i = 0; i < 6; i++) {
        color += letters[Math.floor(Math.random() * 16)];
      }
      return color;
    }

    function showJob(startingPoints, points, w, h) {
      var canvas = document.getElementById('canvas');
      var ctx = canvas.getContext("2d");
      canvas.width = w;
      canvas.height = h;
      var pxsize = 10;
      ctx.fillStyle = "#00C853";
      for (var i = 0; i < startingPoints.length; i++) {
        var x = parseFloat(startingPoints[i].x);
        var y = parseFloat(startingPoints[i].y);
        ctx.fillRect(x - pxsize / 2.0, y - pxsize / 2.0, pxsize, pxsize);
      }
      pxsize = 5;
      for (var i = 0; i < points.length; i++) {
        var node = points[i].uuid;
        var color = getRandomColor();
        var list = points[i].points;
        ctx.fillStyle = color;
        for (var j = 0; j < list.length; j++) {
          var x = parseFloat(list[j].x);
          var y = parseFloat(list[j].y);
          ctx.fillRect(x - pxsize / 2.0, y - pxsize / 2.0, pxsize, pxsize);
        }
        vm.colors.push({node: node, color: color});
      }
    }
  }

})();
