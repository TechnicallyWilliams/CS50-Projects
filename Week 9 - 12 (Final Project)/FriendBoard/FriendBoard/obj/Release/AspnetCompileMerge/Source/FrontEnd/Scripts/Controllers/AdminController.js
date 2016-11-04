app.controller('AdminController', function ($scope, $http, $location, $route, $window, friendBoard) {
    //var check = wishHeroes.checkLogin();
    $scope.pageClass = 'Admin';
    // Admin Info = AJAX success
    $scope.profile = {};
    $scope.newArray = [];
    $scope.editMode = false;

    // getProfile - makes an AJAX call to the api
    $scope.getAllProfiles = function () {
        console.log("The admin get has been reached");
        $http({
            // GET profile info
            headers: { Authorization: 'Bearer ' + sessionStorage.getItem('token') },
            url: '/api/Admin',
            method: 'GET',
        }).success(function (data) {
            console.log(data);
            $scope.profile = data;
            for (var i in data.wishRelationships) { //G CODE

                $scope.newArray.push(data.wishRelationships[i])

            }
        }).error(function () {
            // Error - send alert
            alert("Profile GET did not work");
        });
    }

    $scope.getAllProfiles();



});