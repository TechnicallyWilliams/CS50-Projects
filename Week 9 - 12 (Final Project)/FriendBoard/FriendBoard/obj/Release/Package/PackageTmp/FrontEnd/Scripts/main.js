/* Copyright 2013 Chris Wilson

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

window.AudioContext = window.AudioContext || window.webkitAudioContext;

var audioContext = new AudioContext();
var audioInput = null,
    realAudioInput = null,
    inputPoint = null,
    audioRecorder = null;
var rafID = null;
var analyserContext = null;
var canvasWidth, canvasHeight;
var recIndex = 0;
var audioEvent = null;
var audioSource = null;
var audioBytes = null;

// TODO:

// offer mono option
// "Monitor input" switch


function saveAudio() {
    audioRecorder.exportWAV(doneEncoding);
    // could get mono instead by saying
    // audioRecorder.exportMonoWAV( doneEncoding );
}

function gotBuffers(buffers) {
    // the ONLY time gotBuffers is called is right after a new recording is completed - 
    // so here's where we should set up the download.
    audioRecorder.exportWAV(doneEncoding);
}

function doneEncoding(blob) {

    //stop listening and nullify audioRecorder
    audioInput.mediaStream.getAudioTracks()[0].stop();   //dot notation or intellisense doesn't always give away the internals of an object in javascript
    realAudioInput.mediaStream.getAudioTracks()[0].stop();
    audioRecorder.clear();
    audioRecorder = null;

    var audio; // = Recorder.setupDownload(blob, "myRecording" + ((recIndex < 10) ? "0" : "") + recIndex + ".wav");
    var reader = new window.FileReader();
    reader.readAsDataURL(blob);
    reader.onloadend = function () {
        var base64data = reader.result.split(',')[1];
        var padding = base64data.length % 4;
        for (var i = 1; i <= padding; i++) {
            base64data += '=';
        }

        savedWAVBlob = base64data;
        audioBytes = savedWAVBlob;
        //JSON.stringify({ 'Audio': null, 'Image': imgbytes, 'Name': null })
    }
    // {}/Blah x,  ={}/Blah x,  =JSON/blah x, JSON=/blah, JSON/blah x, , // all with id inside of body
    //contentType: 

    var saveAudio = function () {

        var audioID = audioSource.split(" ");
        if (audioID[0] > 0) {
            console.log(audioID[0]);    
            $.ajax({
                method: 'put',  //method: 'post',
                headers: { Authorization: 'Bearer ' + sessionStorage.getItem('token') },
                contentType: 'application/json; charset=utf-8', //dataType: 'text',
                url: '/api/Sogo/' + audioID[0], //url: '/api/Sogo',
                data: JSON.stringify({ 'Audio': audioBytes, 'Image': null, 'Name': null, 'Id': null }), //"=value=" // data: "=" + audioBytes,
                success: function () {
                   // document.getElementsByTagName('script')[2].src.replace('http://localhost:4324/FrontEnd/Scripts/main.js', 'http://localhost:4324/FrontEnd/Scripts/main.js?n=1');
                   // document.getElementsByTagName('script')[8].src.replace('http://localhost:4324/FrontEnd/Scripts/JS/App.js', 'http://localhost:4324/FrontEnd/Scripts/JS/App.js?n=1');
                   // document.getElementsByTagName('script')[13].src.replace('http://localhost:4324/FrontEnd/Scripts/Controllers/SogoController.js', 'http://localhost:4324/FrontEnd/Scripts/Controllers/SogoController.js?n=1'); //sogocontroller

                    var jsVersion = +document.getElementsByTagName('script')[2].src.slice(-1) + 1;
                    alert(jsVersion);
                    document.getElementsByTagName('script')[2].src = null;
                    document.getElementsByTagName('script')[8].src = null;
                    document.getElementsByTagName('script')[13].src = null;
                    document.getElementsByTagName('script')[2].src = 'http://localhost:4324/FrontEnd/Scripts/main.js?n=' + jsVersion;
                    document.getElementsByTagName('script')[8].src = 'http://localhost:4324/FrontEnd/Scripts/JS/App.js?n=' + jsVersion;
                    document.getElementsByTagName('script')[13].src = 'http://localhost:4324/FrontEnd/Scripts/Controllers/SogoController.js?n=' + jsVersion;

                    location.reload();
                },
                error: function () {
                    console.log("Failed!");
                }
            });
        } else {

            document.getElementById('newAudio').value = audioBytes;
            audioSource = null;

            if (document.getElementById('newImage').value && document.getElementById('newAudio').value && document.getElementById('newName').value) {
                document.getElementById('ok').style.backgroundColor = 'Green';
                document.getElementById('ok').style.opacity = 1;
            } else {
                document.getElementById('ok').style.backgroundColor = 'Yellow';
            }

            /* $.ajax({
                method: 'post',  //method: 'post',
                dataType: 'text', //dataType: 'text',
                url: '/api/Sogo', //url: '/api/Sogo',
                data: "=" + Audio, //"=value=" // data: "=" + audioBytes,
                success: function (msg) {
                    alert("wow!");
                },
                error: function () {
                    console.log("Failed!");
                }
            });*/
        }

    }

    setTimeout(saveAudio, 400); //50 miliseconds, 1000 is 1 second

    // contentType: 'application/x-www-form-urlencoded; charset=utf-8',


    recIndex++;
}

function toggleRecording(e) {
    // start recording
    if (!audioRecorder)
        return; //This should start listening and recording again
    e.classList.add("recording");
    audioRecorder.clear();
    audioRecorder.record();

    if (audioSource.split(" ")[0] == "recordNew") {
        console.log($("img.recordNew").parent('button')[0]);
        $("img.recordNew").parent('button')[0].style.backgroundColor = 'DarkRed';
        $("img.recordNew").parent('button')[0].style.opacity = 0.8;
    } else {
        //do something based on class with number
        console.log(audioSource.split(" "));
        $('.' + audioSource).parent('button')[0].style.backgroundColor = 'DarkRed';
        $('.' + audioSource).parent('button')[0].style.opacity = 0.8;
    }

}

function convertToMono(input) {
    var splitter = audioContext.createChannelSplitter(2);
    var merger = audioContext.createChannelMerger(2);

    input.connect(splitter);
    splitter.connect(merger, 0, 0);
    splitter.connect(merger, 0, 1);
    return merger;
}


function toggleMono() {
    if (audioInput != realAudioInput) {
        audioInput.disconnect();
        realAudioInput.disconnect();
        audioInput = realAudioInput;
    } else {
        realAudioInput.disconnect();
        audioInput = convertToMono(realAudioInput);
    }

    audioInput.connect(inputPoint);
}

function gotStream(stream) {
    inputPoint = audioContext.createGain();

    // Create an AudioNode from the stream. (start listening)
    realAudioInput = audioContext.createMediaStreamSource(stream);
    audioInput = realAudioInput;
    audioInput.connect(inputPoint);

    //    audioInput = convertToMono( input );
    audioRecorder = new Recorder(inputPoint);

    zeroGain = audioContext.createGain();
    zeroGain.gain.value = 0.0;
    inputPoint.connect(zeroGain);
    zeroGain.connect(audioContext.destination);
    toggleRecording(audioEvent);
    //updateAnalysers();
}

function startListening(e) {

    console.log(e.className);
    audioSource = e.className;

    if (e.classList.contains("recording")) {
        // stop recording
        audioRecorder.stop();
        e.classList.remove("recording"); //I think getBuffer has an override, one with arguments and one without
        audioEvent = null;

        if (audioSource == "recordNew recording") {
            console.log($("img.recordNew").parent('button')[0]);
            $("img.recordNew").parent('button')[0].style.backgroundColor = '';
            $("img.recordNew").parent('button')[0].style.opacity = 0.4
        } else {
            //do something based on class with number
            $('.' + e.className).parent('button')[0].style.backgroundColor = '';
            $('.' + e.className).parent('button')[0].style.opacity = 0.4;
        }
        return audioRecorder.getBuffers(gotBuffers); //getBuffer is part of the default or what was implemented in prior scripts
    }

    audioEvent = e;
    if (!navigator.getUserMedia)
        navigator.getUserMedia = navigator.webkitGetUserMedia || navigator.mozGetUserMedia;

    navigator.getUserMedia(
        {
            "audio": {
                "mandatory": {
                    "googEchoCancellation": "false",
                    "googAutoGainControl": "false",
                    "googNoiseSuppression": "false",
                    "googHighpassFilter": "false"
                },
                "optional": []
            },
        }, gotStream, function (e) {
            alert('Error getting audio');
            console.log(e);
        });
}


