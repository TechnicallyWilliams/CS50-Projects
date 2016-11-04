app.controller('RegisterController', function ($scope, $http, $location, $route, $window, friendBoard) {
    $scope.pageClass = 'Register';
    $scope.formInfo = {};
    $scope.formInfo.Avatar = null;


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


    $scope.submit = function () {
        $scope.formInfo.AreaCode = Number($scope.formInfo.AreaCode);
        $scope.formInfo.Zip = Number($scope.formInfo.Zip);

        $http.post('/api/Account/Register', $scope.formInfo)
         .success(function () {
             $route.reload();
             //redirect to sogo as a logged in user soon:
             //$location.path('/login');
         })
         .error(function () {
             alert("problem with sending:   " + JSON.stringify($scope.formInfo));
         })
         /*$.ajax({
            method: 'post',
            contentType: 'application/json; charset=utf-8',
            url: '/api/Account/Register',
            data: JSON.stringify($scope.formInfo),
            success: function () {
                alert("hmm");
                $route.reload();
            },
            error: function () {
                alert("problem with sending:   " + JSON.stringify($scope.formInfo));
            }
        });*/
    }



   /* $scope.getAllSchools = function () {
        $http({
            // GET profile info
            headers: { Authorization: 'Bearer ' + sessionStorage.getItem('token') },
            url: '/api/Register',
            method: 'GET',
        }).success(function (data) {
            $scope.allSchools = data;
            console.log($scope.allSchools);
        }).error(function () {
            // Error - send alert
            alert("Register Schools GET did not work");
        });
    }

    $scope.getAllSchools();*/



});