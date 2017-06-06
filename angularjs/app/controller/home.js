(function () {
    'use strict';

    angular
        .module('app')
        .controller('HomeController', HomeController);

    HomeController.$inject = ['NodeService', '$rootScope' , 'FlashService'];
    function HomeController(NodeService, $rootScope, FlashService) {
        var vm = this;

        initController();

        function initController() {
            
        }

        function loadData(request) {
            NodeService.GetData(request)
                .then(function (response) {
                    // TODO
                });
        }
    }

})();
