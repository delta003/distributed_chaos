(function () {
    'use strict';

    angular
        .module('app')
        .factory('NodeService', NodeService);

    NodeService.$inject = ['$http'];
    function NodeService($http) {
        var service = {};

        service.GetData = GetData;

        return service;

        function GetData(request) {
            return $http.post('/api/get-data', request).then(handleSuccess, handleError('Error getting data'));
        }

        // private functions

        function handleSuccess(res) {
            return res.data;
        }

        function handleError(error) {
            return function () {
                return { status: 'error', message: error };
            };
        }
    }

})();
