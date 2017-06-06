(function () {
    'use strict';

    angular
        .module('app')
        .controller('JobController', JobController);

    JobController.$inject = ['NodeService', '$routeParams', '$rootScope' , 'FlashService'];
    function JobController(NodeService, $routeParams, $rootScope, FlashService) {
        var vm = this;

        initController();

        function initController() {
            vm.jobid = $routeParams.jobid;
            // TODO
        }
    }

})();
