app.factory('friendBoard', function ($http, $window, $q) {
    //The factory is for functions that need to be executed for more than one view (i.e. checkLogin)

    // Login Check
    var isLoggedin = false;
    var checkLogin = function () {
        if ($window.sessionStorage.getItem('token') !== null && $window.sessionStorage.getItem('token') !== "") {
            isLoggedin = true;
        } else {
            isLoggedin = false;
        }
        return isLoggedin;
    }

 

 

    /* var userProfile = {};
    var getProfile = function (profileID) {
        console.log("This is the Factory Function taking the profileID:" + profileID);
        var deferred = $q.defer(); //This is a function that delays execution. The delayed part has been abstracted away.
        $http({
            headers: { Authorization: 'Bearer ' + sessionStorage.getItem('token') },
            url: '/api/Reservation/' + profileID,
            method: 'GET'
        }).success(function (data) {
            console.log(data);
            userProfile = data;
            deferred.resolve(userProfile); //$q.defer().resolve(userProfile) // Notice there is no return
        }).error(function () {
            alert("getProfile function GET did not work");
            deferred.reject(); //$q.defer().resolve
        }); return deferred.promise;
    } */

    // RETURNS
    return {
        checkLogin: checkLogin
    }

});