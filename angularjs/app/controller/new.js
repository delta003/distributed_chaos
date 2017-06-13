(function() {
  'use strict';

  angular.module('app').controller('NewJobController', NewJobController);

  NewJobController.$inject = ['NodeService', '$location', '$rootScope', 'FlashService'];
  function NewJobController(NodeService, $location, $rootScope, FlashService) {
    var vm = this;

    // data
    vm.job = {};
    vm.job.points = [];

    // functions
    vm.draw = draw;
    vm.remove = remove;
    vm.resize = resize;
    vm.start = startJob;

    var canvas = document.getElementById("newjobcanvas");
    var context = canvas.getContext("2d");

    function redraw() {
      context.clearRect(0, 0, canvas.width, canvas.height);
      var pxsize = 10;
      context.fillStyle = "#00C853";
      for (var i = 0; i < vm.job.points.length; i++) {
        context.fillRect(vm.job.points[i].x - pxsize / 2.0, vm.job.points[i].y - pxsize / 2.0, pxsize, pxsize);
      }
      context.fill();
    }

    function draw(e) {
      var pos = getMousePos(canvas, e);
      vm.job.points.push(pos);
      redraw();
    }

    function remove(p) {
      var index = vm.job.points.indexOf(p);
      if (index > -1) {
        vm.job.points.splice(index, 1);
      }
      redraw();
    }
    function resize() {
      if (vm.job.width && vm.job.height) {
        var width = parseInt(vm.job.width);
        var height = parseInt(vm.job.height);
        if (isNaN(width) || isNaN(height)) {
          return;
        }
        canvas.height = height;
        canvas.width = width;
        redraw();
      }
    }

    function startJob() {
      if (vm.job.width && vm.job.height && vm.job.p) {
        vm.job.width = parseInt(vm.job.width);
        vm.job.height = parseInt(vm.job.height);
        vm.job.p = parseFloat(vm.job.p);
        if (isNaN(vm.job.width) || isNaN(vm.job.height) || isNaN(vm.job.p)) {
          alert("Input error!");
          return;
        }
        NodeService.JobsNew(vm.job).then(function(response) {
          if (response.status == 'ok') {
            $location.path('/');
          } else {
            console.log(response);
          }
        });
      } else {
        alert("Input error!");
      }
    }

    function getMousePos(canvas, evt) {
      var rect = canvas.getBoundingClientRect();
      return {x: evt.clientX - rect.left, y: evt.clientY - rect.top};
    }
  }
})();
