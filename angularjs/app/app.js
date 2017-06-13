(function() {
  'use strict';

  angular.module('app', ['ngRoute', 'ngCookies']).config(config).run(run);

  config.$inject = ['$routeProvider', '$locationProvider'];
  function config($routeProvider, $locationProvider) {
    $routeProvider.when('/', {controller: 'HomeController', templateUrl: 'view/home.html', controllerAs: 'vm'})
        .when('/job/:jobid', {controller: 'JobController', templateUrl: 'view/job.html', controllerAs: 'vm'})
        .when('/new/', {controller: 'NewJobController', templateUrl: '/view/new.html', controllerAs: 'vm'})
        .otherwise({redirectTo: '/'});

    // use the HTML5 History API
    //$locationProvider.html5Mode(true);
  }

  run.$inject = ['$rootScope', '$location', '$cookieStore', '$http'];
  function run($rootScope, $location, $cookieStore, $http) {}
})();
