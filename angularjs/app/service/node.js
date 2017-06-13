(function() {
  'use strict';

  angular.module('app').factory('NodeService', NodeService);

  NodeService.$inject = ['$http'];
  function NodeService($http) {
    var service = {};

    service.NetworkVisualize = NetworkVisualize;
    service.JobsIds = JobsIds;
    service.JobsKill = JobsKill;
    service.JobsVisualize = JobsVisualize;
    service.JobsNew = JobsNew;

    return service;

    function NetworkVisualize() { return $http.get('/api/network/visualize').then(handleSuccess, handleError('Error making request')); }

    function JobsIds() { return $http.get('/api/jobs/ids').then(handleSuccess, handleError('Error making request')); }

    function JobsKill(jobid) { return $http.get('/api/jobs/kill/' + jobid).then(handleSuccess, handleError('Error making request')); }

    function JobsVisualize(jobid) { return $http.get('/api/jobs/visualize/' + jobid).then(handleSuccess, handleError('Error making request')); }
    function JobsNew(job) { return $http.post('/api/jobs/new', job).then(handleSuccess, handleError('Error starting job!')); }

    // private functions

    function handleSuccess(res) { return res.data; }

    function handleError(error) {
      return function() { return {status: 'error', message: error}; };
    }
  }

})();
