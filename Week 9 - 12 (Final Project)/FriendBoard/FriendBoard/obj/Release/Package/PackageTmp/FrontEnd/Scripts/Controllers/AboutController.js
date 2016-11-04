app.controller('AboutController', function ($scope, $http, $location, $route, $window, $routeParams, friendBoard) {

    $scope.pageClass = 'About';

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

    }



});