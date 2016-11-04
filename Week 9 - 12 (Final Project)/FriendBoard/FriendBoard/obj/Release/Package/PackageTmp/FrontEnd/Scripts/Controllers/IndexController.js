//angular.module('friendboard.IndexController',[])
app.controller('IndexController', function ($scope, $http, $location, $route, $window, $routeParams, friendBoard) {
    $scope.pageClass = 'Index';

    var loggedIn = friendBoard.checkLogin();

    if (loggedIn) {
        $('#profileTab').removeClass('hidden');
        $('#sogoTab').removeClass('hidden');
        $('#signOutTab').removeClass('hidden');
        $('#loginTab').addClass('hidden');
        $('#loginFrame').addClass('hidden');
        $('#registerTab').addClass('hidden');

    } else {

        $('#profileTab').addClass('hidden');
        $('#sogoTab').addClass('hidden');
        $('#signOutTab').addClass('hidden');
        $('#loginTab').removeClass('hidden');
        $('#registerTab').removeClass('hidden');

        if (document.getElementById("loginFrame").style.hasOwnProperty("hidden"))
            $('#loginFrame').removeClass('hidden');
    }



    // Admin Info = AJAX success
    $scope.files = {};

    //false
    $scope.editMode = false;

    // get a random Sogo
    $scope.getSogo = function () {
        console.log("The Index get has been reached");
        $http({
            // GET sogo info
            // headers: { Authorization: 'Bearer ' + sessionStorage.getItem('token') },
            url: '/api/Index',
            method: 'GET'
        }).success(function (data) {
            console.log(data);
            $scope.name = data[0];
            $scope.image = 'Images/' + data[1];
            $scope.audio = 'Audio/' + data[2];

        }).error(function () {
            // Error - send alert
            console.log("Index GET did not work");
        });
    }

    $scope.getSogo();

    $scope.playAudio = function (value) {
        console.log(document.getElementById(value));
        document.getElementById(value).play();
    }

    $scope.username = '';
    $scope.password = '';

    $scope.login = function () {
        $http({
            url: '/Token', //This is an internal URL for the backend API //  OWIN middleware based /token endpoint in ASP.Net Web API 
            method: 'POST',
            contentType: 'application/x-www-form-urlencoded',
            data: 'username=' + $scope.username + '&password=' + $scope.password + '&grant_type=password'
        }).success(function (data) {
            console.log(data);
            $window.sessionStorage.setItem('token', data.access_token);
            $('#profileTab').removeClass('hidden');
            $('#sogoTab').removeClass('hidden');
            $('#signOutTab').removeClass('hidden');
            $('#loginTab').addClass('hidden');
            $('#loginFrame').addClass('hidden');
            $('#registerTab').addClass('hidden');
            $location.url('/sogos');

        }).error(function () {
            console.log("Log-in failed. Please become a better developer.");
        });
    };




});