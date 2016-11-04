app.controller('ProfileController', function ($scope, $http, $location, $route, $window, luncHola, friendBoard) {
    //var check = wishHeroes.checkLogin();
    $scope.pageClass = 'Profile';

    var loggedIn = friendBoard.checkLogin();

    if (loggedIn) {
        $('#profileTab').removeClass('hidden');
        $('#sogoTab').removeClass('hidden');
        $('#signOutTab').removeClass('hidden');
        $('#loginTab').addClass('hidden');
        $('#loginFrame').addClass('hidden');
        $('#registerTab').addClass('hidden');

    } else {
        $location.url('/index');
        $('#profileTab').addClass('hidden');
        $('#sogoTab').addClass('hidden');
        $('#signOutTab').addClass('hidden');
        $('#loginTab').removeClass('hidden');
        $('#registerTab').removeClass('hidden');
    }





    // Profile Info = AJAX success
    $scope.profile = {};
    $scope.addForm = {};
    $scope.newArray = [];
    $scope.newLunches = [];
    $scope.data = {};
    //Edit Modes
    $scope.contactEditMode = false;
    $scope.lunchEditMode = false;
    $scope.schoolEditMode = false;
    $scope.kidEditMode = false;
    $scope.profileTypeMode = false;

    //AddModes
    $scope.kidAddMode = false;
    $scope.lunchAddMode = false;

    //Factory Method Data Holder for schools divided by type




    $scope.allSchools = {};

    luncHola.getAllSchools().then(function (data) {
        $scope.allSchools = data;
    });

    //Factory Method Data Holder for all schools undivided
    $scope.everySchool = {};

    luncHola.getSchools().then(function (data) {
        $scope.everySchool = data;
        console.log("This is the everySchool Object:" + "" + $scope.everySchool);
    });

    //ADD KIDTYPE AS A DROPDOWN LIST OR RADIO BUTTONS [PENDING]
    //$scope.specialValue = {
    //    "id": "12345",
    //    "value": "green"
    //};
    $scope.checkKid = function () {
        //Finally this makes sense. Your problem was that your array was empty so it had nothing to loop through but would still attempt to
        if ($scope.profile.kids.length > 0) {
            for (var i in $scope.profile.kids) {
                if ($scope.profile.kids[i].kidCount === 5) {
                    console.log("Inside the kid count for kid equalling 5");
                    return $scope.kidAddMode = false;
                }
            };
            if ($scope.kidAddMode === true) {
                console.log("turn kid mode off");
                $scope.kidAddMode = false;
            } else {
                console.log("turn kid mode on");
                $scope.kidAddMode = true;
            }
        } else if ($scope.kidAddMode === true) {
            console.log("Don't show KidAddBox");
            $scope.kidAddMode = false;
        }
        else {
            console.log("Show kidaddbox");
            $scope.kidAddMode = true;
        }
    }


    $scope.addKid = function (addForm) {
        //need a separate button to change profile type
        console.log($scope.addForm);
        if ($scope.profile.kids.length > 0) {
            for (var i in $scope.profile.kids) {
                if ($scope.profile.kids[i] === 5) {
                    return document.getElementById("kidlimit").innerHTML = "You cannot add more than 5 children!";
                }
            };
            $http({
                headers: { Authorization: 'Bearer ' + sessionStorage.getItem('token') },
                url: '/api/Profile',
                method: 'POST',
                data: $scope.addForm
            }).success(function () {
                document.getElementById("kidlimit").innerHTML = "You have just added" + $scope.addForm.newKid.alias;
                $route.reload();
            }).error(function () {
                alert("problem with sending" + $scope.addForm + ".");
            })
        } else {
            $http({
                headers: { Authorization: 'Bearer ' + sessionStorage.getItem('token') },
                url: '/api/Profile',
                method: 'POST',
                data: $scope.addForm
            }).success(function () {
                $route.reload();
            }).error(function () {
                alert("problem with sending" + $scope.addForm + ".");
            })
        }
    }

    $scope.addLunch = function (addForm) {
        // $scope.profile.lunches[0].propname.value
        document.getElementById("lunchmessage").innerHTML = "Tasty!";
        console.log($scope.addForm.newLunch);
        $http({
            headers: { Authorization: 'Bearer ' + sessionStorage.getItem('token') },
            url: '/api/Profile',
            method: 'POST',
            data: $scope.addForm
        }).success(function () {
            $route.reload();
        }).error(function () {
            alert("problem with sending" + $scope.addForm + ".");
        });
    }


    // getProfile - makes an AJAX call to the api
    $scope.getProfile = function () {
        $http({
            // GET profile info
            headers: { Authorization: 'Bearer ' + sessionStorage.getItem('token') },
            url: '/api/Profile',
            method: 'GET'
        }).success(function (data) {
            console.log(data);
            if (data.userType == "Both") {
                $scope.profile = data;
                console.log($scope.profile);
            }
            else if (data.userType == "Cook") {
                $scope.profile = data;
            }
            else {
                $scope.profile = data;
                $scope.profile.userType == 'User';
            };
        }).error(function () {
            // Error - send alert
            alert("Profile GET did not work");
        });
    }

    $scope.getProfile();
    //};
    //if (data.lunches) {
    //};
    //    $('#adminTab').removeClass('hidden');
    //$scope.profile = data;
    //<div id="lunch">
    //<select id="highSchool" ng-model="formInfo.hsID" style="color:black; font-weight:bold">
    //<option ng-repeat="s in allSchools.highSchoolName">
    //$('#logoutTab').removeClass('hidden');


    // submit - submits an edited version of the profile to the api
    $scope.submit = function (profile) {
        console.log($scope.profile);

        for (var prop in $scope.profile) {
            console.log(prop);
        }
        //if ($scope.profile.userType === 'Cook' || $scope.profile.userType === 'Both' && ($scope.profile.highSchoolName.highSchoolID == isNaN && $scope.profile.middleSchoolName.middleSchoolID == isNaN && $scope.profile.elementarySchoolName.elementarySchoolID == isNaN))
        //{
        //    return console.log("Theres a problem")
        //} 

        //var ksId = document.getElementById("kidSchool").value;
        //var hsId = document.getElementById("highSchool").value;
        //var msId = document.getElementById("middleSchool").value;
        //var esId = document.getElementById("elementarySchool").value;
        //if (hsId != null) {
        //    profile.highSchoolName.highSchoolID = hsId;
        //    console.log(msId);
        //};
        ////if (msId !== null) {
        ////    console.log(profile.middleSchoolName.middleSchoolID);
        ////    profile.middleSchoolName.middleSchoolID = msId;
        ////};
        //if (esId != null) {
        //    profile.elementarySchoolName.elementarySchoolID = esId;
        //};
        $http({
            headers: { Authorization: 'Bearer ' + sessionStorage.getItem('token') },
            url: '/api/Profile/',
            method: 'PUT',
            data: profile,
        }).success(function () {
            alert('Your profile has been successfully updated.')
            $route.reload();
            //$scope.editMode = false;
        }).error(function () {
            alert("Edit did not work");
        });
    }

    //Soft deletes the profile
    $scope.removeLunch = function (lunch) {
        console.log(lunch.lunchID); 
            $http({
                headers: { Authorization: 'Bearer ' + sessionStorage.getItem('token') },
                url: '/api/Profile/' + lunch.lunchID,
                method: 'DELETE',
            }).success(function () {
                console.log("Removed Lunch Successfully");
                $route.reload();
            }).error(function () {
                alert("Delete lunch did not work");
            }); 
    }


    $scope.removeKid = function (kid) {
        console.log(kid.kidID);
        $http({
            headers: { Authorization: 'Bearer ' + sessionStorage.getItem('token') },
            url: '/api/Profile/' + kid.kidID,
            method: 'PATCH',
        }).success(function () {
            console.log("Removed Kid Successfully");
            $route.reload();
        }).error(function () {
            alert("Delete kid did not work");
        });
    }










    /* if (check == false) {
        $location.path('/');
    } else {
        check = wishHeroes.checkLogin();
        $('#profileTab').removeClass('hidden');
        $('#loginTab').addClass('hidden');
        $('#logoutTab').removeClass('hidden');
        if ($window.sessionStorage.getItem('role') == 'Admin') {
            $('#adminTab').removeClass('hidden');
        }
    } */
});