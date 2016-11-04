var app = angular.module('friendboard', ['ngRoute']);
console.log("I'm inside app.js aka the router");

app.config(["$routeProvider", function ($routeProvider) {

    $routeProvider.when('/', {
        templateUrl: '/FrontEnd/Index.html',
        controller: 'IndexController'
    }) 
    .when('/login', {
        templateUrl: '/FrontEnd/Index.html',
        controller: 'IndexController'
    })
    .when('/register', {
        templateUrl: '/FrontEnd/Templates/Register.html',
        controller: 'RegisterController'
    })
    .when('/profile', {
        templateUrl: '/FrontEnd/Templates/Profile.html',
        controller: 'ProfileController'
    })
    .when('/admin', {
        templateUrl: '/FrontEnd/Templates/Admin.html',
        controller: 'AdminController'
    })
    .when('/about', {
        templateUrl: '/FrontEnd/Templates/About.html',
        controller: 'AboutController'
    })
    .when('/reviews', {
        templateUrl: '/FrontEnd/Templates/Reviews.html',
        controller: 'ReviewsController'
    })
    .when('/sogos', {
        templateUrl: '/FrontEnd/Templates/Sogos.html',
        controller: 'SogoController'
    })
    .otherwise({
        redirectTo: '/'
    });

}]);