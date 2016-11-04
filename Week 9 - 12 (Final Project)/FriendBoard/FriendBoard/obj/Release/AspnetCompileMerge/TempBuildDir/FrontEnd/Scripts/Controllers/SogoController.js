//angular.module('friendboard.IndexController',[])
app.controller('SogoController', function ($scope, $http, $location, $route, $routeParams, friendBoard) {
    $scope.pageClass = 'Sogos';

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

    var jsVersion = +document.getElementsByTagName('script')[2].src.slice(-1) + 1;
    document.getElementsByTagName('script')[2].src = null;
    document.getElementsByTagName('script')[8].src = null;
    document.getElementsByTagName('script')[13].src = null;
    document.getElementsByTagName('script')[2].src = 'http://localhost:4324/FrontEnd/Scripts/main.js?n=' + jsVersion;
    document.getElementsByTagName('script')[8].src = 'http://localhost:4324/FrontEnd/Scripts/JS/App.js?n=' + jsVersion;
    document.getElementsByTagName('script')[13].src = 'http://localhost:4324/FrontEnd/Scripts/Controllers/SogoController.js?n=' + jsVersion;

    //var check = wishHeroes.checkLogin();
    // Admin Info = AJAX success

    $scope.files = {};
    $scope.sogos = {};
    $scope.sogosIndex = [];
    $scope.Name = '';
    document.getElementById('ok').style.backgroundColor = 'Red';
    var newName = document.getElementById('newName');
    var newImage = document.getElementById('newImage');


    // get a random Sogo
    $scope.getUserSogos = function () {
        $http({
            // GET sogo info
            headers: { Authorization: 'Bearer ' + sessionStorage.getItem('token') },
            url: '/api/Sogo/1',
            method: 'GET',
        }).success(function (data) {

            for (var sogo in data) { //G CODE
                $scope.sogosIndex.push(data[sogo]); //Due to this, I need to rerrange my html targets to be unique
                $scope.sogosIndex[sogo].Image = "Images/" + $scope.sogosIndex[sogo].Image;
                $scope.sogosIndex[sogo].Audio = "Audio/" + $scope.sogosIndex[sogo].Audio;
            }

        }).error(function () {
            // Error - send alert
            alert("Index GET did not work");
        });
    }

    $scope.getUserSogos();

    $scope.playAudio = function (value) {
        document.getElementById(value).play();
    }

    $scope.delete = function (value) {

        $http({
            // GET sogo info
            headers: { Authorization: 'Bearer ' + sessionStorage.getItem('token') },
            url: '/api/Sogo/' + value,
            method: 'DELETE',
        }).success(function (data) {
            $('#' + value).remove();
        }).error(function () {
            // Error - send alert
            alert("DELETE did not work");
        });

    }




    /*
    $(function () {
        $(":file").change(function () {
            if (this.files && this.files[0]) {
                var reader = new FileReader();
                reader.onload = imageIsLoaded;
                reader.readAsDataURL(this.files[0]);
                // console.log(reader.readAsArrayBuffer(this.files[0]));
                //console.log(reader);

                function getBase64Image(img) {
                    // console.log("This is it" + img.width);
                    console.log("This is width" + img.width);
                    var canvas = document.createElement("canvas");
                    canvas.width = img.width;
                    canvas.height = img.height;

                    var ctx = canvas.getContext("2d");
                    console.log("This is it" + ctx);
                    ctx.drawImage(img, 0, 0);

                    var dataURL = canvas.toDataURL("image/png"); //dataurl is like where an image is located on a canvas

                    return dataURL.replace(/^data:image\/(png|jpg);base64,/, "");
                }


                var timeitout = function () {
                    var image = new Image();
                    // image.src = $('#myImg').src;
                    bannerImage = $('#myImg')[0];
                    $scope.imageBytes = getBase64Image(bannerImage); //Take this to local server
                    //console.log($scope.imageWrapper.imgData);
                    //value = "hello";

                    //localStorage.setItem("imgData", imgData);
                    $http({
                        url: '/api/Sogo',
                        contentType: 'application/x-www-form-urlencoded; charset=utf-8',
                        method: 'POST',
                        data: JSON.stringify($scope.imageBytes)
                    }).success(function () {
                        // Navigator.getUserMedia();
                        // document.getElementById("kidlimit").innerHTML = "You have just added" + $scope.addForm.newKid.alias;
                        //$route.reload();
                    }).error(function () {
                        alert("problem with sending" + $scope.imgData + ".");
                    })
                }

                setTimeout(timeitout, 2000);

            }
        });
    });

    function imageIsLoaded(e) {
        $('#myImg').attr('src', e.target.result);
    }; */


    //webaudiodemos.appspot.com (how to record)
    /* 
    
    (load) recorderWorker.js ------> recorder.js when window loads   ------------------------------------------------> main.js
                                       - create and load Recorder() function with all types of functions                 - Initialize window.AudioContext and instantiate [ var audioContext = new AudioContext(); ]      
                                            - create instance called worker from prev. js                                - initAudio() ---> gotStream ----> create new instance of Recorder() ---> call a series of function
                                             - load Worker with all types of functions                                    that implement functions that come from audioContext like audioContext.createMediaStreamSource(stream);
                                       - Connect Recorder's node to the source                                           - 
                                       - Connect node to destination
                                       - Define download function
                                       - Set DOM's (window) window.recorder property equal to the Recorder() just created

    - id's to be concerned with: "controls" (CSS only), "record" (css and click for recording), "save" ( setupdownload via recorder.js and CSS );
                                        
         - Microphone turns on automatically               ToggleRecording( e )
         -create audioRecorder node from stream            - talks to audioRecorder, which is the DOM'S window recorder implemented in other javascript;
         -  all mono's can be ignored                     - it checks if already recording, if it is, it stops and saves bits into a buffer
                                                           - if not recording, starts recording
                                                           - These methods come from the previous page
    */

    //webaudio
    /*var errorCallback = function (e) {
        console.log("Reeejected!", e);
    };


    window.AudioContext = window.AudioContext || window.webkitAudioContext;

    var context = new AudioContext();

    navigator.getUserMedia({ audio: true }, function (stream) {
        var microphone = context.createMediaStreamSource(stream);
        var filter = context.createBiquadFilter();

        microphone.connect(filter);
        filter.connect(context.destination);
    }, errorCallback);*/


    //Taking Screenshots
    /*Screenshot Modals*/
    var modal = document.getElementById('myModal');
    var nameModal = document.getElementById('nameModal');
    var nameSpan = document.getElementById('nameClose');
    var nameId;
    //var btn = document.getElementById('myBtn');
    var span = document.getElementsByClassName('close')[0];
    //var video1 = "<video id='theVideo' height='200' width='200' class='circle-avatar' autoplay='' src=''></video>";
    var video = document.querySelector('video');
    var ctx;
    var localMediaStream = null;


    $scope.camera = function (audioId) {
        modal.style.display = "block";
        startVideo(audioId);
    }

    $scope.createName = function (element) {
        nameModal.style.display = "block";
        nameId = element.id;
    }

    $scope.sendName = function () {

        if (/[^A-Za-z\d]/.test($scope.Name)) {
            //document.nameForm.txt.focus();
            if (document.getElementById("charWarning")) {
                return document.getElementById("charWarning").style.color = "red";
            }

            var warning = document.createElement("p");
            warning.id = "charWarning";
            warning.style.backgroundColor = 'rgb(0,0,0,0.4)';
            warning.innerText = "Special characters are not allowed: !@#$....."
            return $("#specialChar").append(warning);
        }

        nameModal.style.display = "none";

        if (nameId) {
            $.ajax({
                method: 'put',
                contentType: 'application/json; charset=utf-8',
                url: '/api/Sogo/' + nameId,
                headers: { Authorization: 'Bearer ' + sessionStorage.getItem('token') },
                data: JSON.stringify({ 'Audio': null, 'Image': imgbytes, 'Name': $scope.Name, 'Id': nameId }),
                success: function (msg) {
                    $route.reload();
                },
                error: function () {
                    console.log("Failed!");
                }
            });

        } else {
            //Fill up the hidden DIV
            newName.value = $scope.Name;
            if (newName.value && document.getElementById('newAudio').value && newImage.value) {
                document.getElementById('ok').style.backgroundColor = 'Green';
                document.getElementById('ok').style.opacity = 1;
            } else {
                document.getElementById('ok').style.backgroundColor = 'Yellow';
            }
        }



    }



    nameSpan.onclick = function () {
        nameModal.style.display = "none";
        nameId = null;
    }

    /* btn.onclick = function () {
         modal.style.display = "block";
         startVideo();
     } */

    span.onclick = function () {
        modal.style.display = "none";
        localMediaStream.getVideoTracks()[0].stop();
        // localMediaStream = null;
        ctx.clearRect(0, 0, 200, 200);

        var video1 = document.createElement("video");
        video1.id = 'theVideo';
        video1.height = 200;
        video1.width = 200;
        video1.className = 'circle-avatar'; //removed video1.src
        video1.autoplay = true;
        $("#theVideo").remove();
        $("#videoParent").append(video1);
        video = video1;

    }

    window.onclick = function (event) {
        if (event.target == modal) {
            modal.style.display = "none";
            localMediaStream.getVideoTracks()[0].stop();
            // localMediaStream = null;
            ctx.clearRect(0, 0, 200, 200);

            var video1 = document.createElement("video");
            video1.id = 'theVideo';
            video1.height = 200;
            video1.width = 200;
            video1.className = 'circle-avatar';  //removed video1.src
            video1.autoplay = true;
            $("#theVideo").remove();
            $("#videoParent").append(video1);
            video = video1;

        }
    }

    var startVideo = function (audioId) {

        var errorCallback = function (e) {
            console.log("Reeejected!", e);
        };

        function snapshot() { //canvas by default is 300width x 100 height by default
            if (localMediaStream) { //250, 400, 0, 0, 250, 400
                console.log("THis is the stream" + localMediaStream);
                ctx.drawImage(video, 0, 0, 200, 200); //Need to write down all the pain of previewing an image: http://stackoverflow.com/questions/16499687/html5-canvas-drawimage-stretched#16499914
                //ctx.drawImage(video, 160, 30, 250, 350, 0, 0, 200, 200); //probably need length and width;
                // "image/webp" works in Chrome.
                // Other browsers will fall back to image/png.  
                var videoObject = [document.getElementById('myVideo')];
                videoObject.src = $scope.canvas.toDataURL('image/png'); //correct
                imgbytes = videoObject.src.replace(/^data:image\/(png|webp);base64,/, ""); ///replace then wrap

                //'username=' + $scope.username + '&password=' + $scope.password + '&grant_type=password'
                //'=imagebytes=' + imgbytes + '&audiobytes=null' + '&sogoname=null', //JSON.stringify removed
                //contentType is more appropriate than datatype for
                var saveImage = function () {
                    if (!audioId) {
                        newImage.value = imgbytes;
                        if (newName.value && document.getElementById('newAudio').value && newImage.value) {
                            document.getElementById('ok').style.backgroundColor = 'Green';
                            document.getElementById('ok').style.opacity = 1;
                        } else {
                            document.getElementById('ok').style.backgroundColor = 'Yellow';
                        }
                        /* $.ajax({
                            method: 'post',
                            contentType: 'application/json; charset=utf-8',
                            url: '/api/Sogo',
                            data: JSON.stringify({ 'Audio': null, 'Image': imgbytes, 'Name': null }), //JSON.stringify removed
                            success: function (msg) {
                                alert("wow!");
                            },
                            error: function () {
                                console.log("Failed!");
                            }
                        });*/
                    } else {
                        $.ajax({
                            method: 'put',
                            contentType: 'application/json; charset=utf-8',
                            url: '/api/Sogo/' + audioId,
                            headers: { Authorization: 'Bearer ' + sessionStorage.getItem('token') },
                            data: JSON.stringify({ 'Audio': null, 'Image': imgbytes, 'Name': null }), //JSON.stringify removed
                            success: function (msg) {
                                $route.reload();
                            },
                            error: function () {
                                console.log("Failed!");
                            }
                        });
                    }
                }

                setTimeout(saveImage, 700);
            }
        };


        video.addEventListener('click', snapshot, false);
        //removed mediaDevices.get

        var holdVideo = function () {
            navigator.getUserMedia({ video: true }, function (stream) {
                video.src = window.URL.createObjectURL(stream);
                localMediaStream = stream;

            }, errorCallback);

            var createCanvas = function () {
                document.querySelector('canvas').setAttribute("width", 200);
                document.querySelector('canvas').setAttribute("height", 200);
                $scope.canvas = document.querySelector('canvas');
                //$('#myCanvas'); Is a different object than a query selector
                ctx = $scope.canvas.getContext('2d');
                console.log(video);
                video.play();

            }

            setTimeout(createCanvas, 1000);
        }

        setTimeout(holdVideo, 2000);

    }
    //Source is the audio data generated (MP3 as a file or microphone)
    //5 stages to production
    //Destination is where you want to output the data in the end (recording)
    /* context = new AudioContext();
    context.currentTime;
    osc = context.createOscillator;
    osc.frequency.value = 400; //44 hz
    osc.connect(context.destination); //connects to speakers by default
    osc.start(0);


    context = new AudioContext();
    context.currentTime;
    osc = context.createOscillator;
    gain = context.createGain(); //node for volume
    osc.connect(gain);
    gain.gain.value = 0.6 //is 60%
    gain.connect(context.destination) */


    // context.destination //default is speakers for output //script processor node for recording is more complicated
    /* 
    Each step in the pipeline is a series of audio nodes

    1.) Ex. Source is Oscillator
    OscillatorNode - generates a tone    osc = context.createOscillator()  //keeps wave forms in a digital format

    2.) Buffer load (mp3)
    www.html5rocks.com/en/tutorials/webaudio/intro/js/buffer-loader.js

    3.) Recording (microphone)
       <input type="file" accept="audio/*;capture=microphone">
       Navigator.getUserMedia() to request access to the user's microphone

       4.) Snapshots
       <input type="file" accept="image/*;capture=camera">

    */

    /* jshint validthis:true 
    // var vm = this;
    $scope.onFileSelect = function () {
        console.log($scope.files);
        console.log(formData.file);
        console.log($scope.files.name);
        //$files: an array of files selected, each file has name, size, and type.
        //for (var i = 0; i < $files.length; i++) {
        //    var file = $files[i];
        $http({
            headers: { 'Content-Type': 'multipart/form-data' },
            url: '/api/Register',
            method: 'POST',
            transformRequest: $scope.files
            //file: file, // or list of files ($files) for html5 only
        }).success(function (data, status, headers, config) {
            alert('Uploaded successfully ' + files.name);
        }).error(function () {
            alert('Error occured during upload');
        });
    }

    var formData = new FormData();
    formData.append("file", $scope.files);*/
    var infoModal = document.getElementById('infoModal');
    var infoSpan = document.getElementById('infoClose');

    infoSpan.onclick = function () {
        infoModal.style.display = "none";
    }

    $scope.createSogo = function () {

        if (newName.value && document.getElementById('newAudio').value && newImage.value) {

            $("#sogoDisplay").remove();
            var innerDiv = document.createElement("div");
            innerDiv.className = 'text-center';
            innerDiv.height = 160;
            innerDiv.width = 160;
            innerDiv.innerHTML = "<p> SENDING.. </p>";
            $("#sogoContainer").append(innerDiv);

            var audioChar = document.getElementById('newAudio').value;

            $.ajax({
                method: 'post',
                contentType: 'application/json; charset=utf-8',
                url: '/api/Sogo/',
                headers: { Authorization: 'Bearer ' + sessionStorage.getItem('token') },
                data: JSON.stringify({ 'Audio': audioChar, 'Image': newImage.value, 'Name': newName.value, 'Id': 0 }),
                success: function () {
                    alert("hmm");
                    $route.reload();
                },
                error: function () {
                    console.log("Failed!");
                }
            });

        } else {
            infoModal.style.display = "block";
        }
    }



});