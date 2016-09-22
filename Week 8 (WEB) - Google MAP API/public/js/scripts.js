/* global google */
/* global _ */
/**
 * scripts.js
 *
 * Computer Science 50
 * Problem Set 8
 *
 * Global JavaScript.
 */

// Google Map
var map;

// markers for map
var markers = [];

// info window
var info = new google.maps.InfoWindow();


//Words (feel free to add more to this "highly advanced" structure ;) (could be allocated to a file)
var words = [ 
    {   CompanyOne: 'Facebook',
        CompanyTwo: 'Twitter',
        CompanyThree: 'Instagram',
        CompanyFour: 'Tindr',
        CompanyFive: 'Amazon',
        CompanySix: 'Zuckerberg'
    },
    {   CrimeOne: 'police',
        CrimeTwo: 'cops',
        CrimeThree: 'plot',
        CrimeFour: 'crime',
        CrimeFive: 'indecent',
        CrimeSix: 'high-speed',
        CrimeSeven: 'shooting',
        CrimeEight: 'Theft',
        CrimeNine: 'asian',
        CrimeTen: 'suspects',
        CrimeEleven: 'caught',
        CrimeTwelve: 'sought',
        CrimeThirteen: 'sentence',
        CrimeFourteen: 'guilty',
        CrimeFifteen: 'killed',
        Crimesixteen: 'rape'
    },  
    {   SchoolOne: 'campus',
        SchoolTwo: 'university',
        SchoolThree: 'classroom',
        SchoolFour: 'teacher',
        SchoolFive: 'academy',
        SchoolSix: 'graduation',
        SchoolSeve: 'college'
    },
    {   MilitaryOne: 'soldier',
        MilitaryTwo: 'military',
        MilitaryThree: 'veteran'
    },
    {   FoodOne: 'restaurant',
        FoodTwo: 'fast food',
        FoodThree: 'pizza',
        FoodFour: 'burger',
        FoodFive: 'noodles',
        FoodSix: 'eating',
        FoodSeven: 'drink',
        FoodEight: 'grocery'
    }];
    
//categories for the words array    
var icons = [
    {   type: 'company',
        link: 'http://seosmart.net/wp-content/uploads/2015/10/social.png'
    },
    {   type: 'crime',
        link: 'http://beachconnections.com/images/government/crime_map.png'
    }, 
    {   type: 'school',
        link: 'http://www.geekchamp.com/upload/SymbolIcons/LifestyleLeasure/1f393-Graduation%20cap.png'
    }, 
    {   type: 'military',
        link: 'http://www.ncsi.com/img/usarmy_logo.png'
    },
    {   type: 'food',
        link: 'http://www.justaddwater.com.ph/wp-content/themes/jawuikit/images/food.png'
    }];

// execute when the DOM is fully loaded
$(function() {

    // styles for map
    // https://developers.google.com/maps/documentation/javascript/styling
    var styles = [

        // hide Google's labels
        {
            featureType: "all",
            elementType: "labels",
            stylers: [
                {visibility: "off"}
            ]
        },

        // hide roads
        {
            featureType: "road",
            elementType: "geometry",
            stylers: [
                {visibility: "off"}
            ]
        }

    ];

    // options for map
    // https://developers.google.com/maps/documentation/javascript/reference#MapOptions
    var options = {
        center: {lat: 37.4236, lng: -122.1619}, // Stanford, California
        disableDefaultUI: true,
        mapTypeId: google.maps.MapTypeId.ROADMAP,
        maxZoom: 14,
        panControl: true,
        styles: styles,
        zoom: 13,
        zoomControl: true
    };

    // get DOM node in which map will be instantiated
    var canvas = $("#map-canvas").get(0);

    // instantiate map
    map = new google.maps.Map(canvas, options);

    // configure UI once Google Map is idle (i.e., loaded)
    google.maps.event.addListenerOnce(map, "idle", configure);

});

/**
 * Adds marker for place to map.
 */
function addMarker(place, map)
{
    //image for the marker
    var image = "https://maps.google.com/mapfiles/kml/pal2/icon31.png";
    
    //latitude and longitude for the marker
    var myLatlng = new google.maps.LatLng(place.latitude, place.longitude);
    
    var marker = new google.maps.Marker({
        position: myLatlng,
        title: place.place_name,
        zipCode: place.postal_code,
        icon: image
    });
    
    // place marker in the array of markers to prevent overwriting previous ones
    markers.push(marker);
    var length = markers.length;
    markers[length - 1].setMap(map); //adds the marker to the map, call setMap();
    
    markers[length - 1].addListener('click', function() {
        
        hideInfo(); //Close info box even if it's not open 

        var parameters = {
            geo: place.postal_code
        };
    
        $.getJSON("articles.php", parameters)
        .done(function(data, textStatus, jqXHR) {
    
            //Setup the html to be stuffed inside the info box
            var contentString = '<div id="' + place.postal_code + '"> <ul>';
            for (var i = 0; i < data.length; i++)
            {
                 contentString +=  '<li>' + '<a href="' + data[i].link + '">' + data[i].title + '</a>' +  '</li>';
            }
            contentString += '</ul> </div>';
            
            //Display infowindow
            showInfo(markers[length - 1], contentString );
            
            //Grab div just created in order to place icons within it
            var div = document.getElementById(place.postal_code);
            var newText = '<ul style="margin: 0; padding: 0; list-style-type: none;" >';
            
            for ( i = 0; i < data.length; i++ ) //Loop for every article found
            {
                var tag = data[i].title.split(" ");
                
                for ( var j = 0; j < words.length; j++ )  //isolate each index holding objects in the words array
                {
                    var objectCount = 0;
                    
                    for ( var word in words[j] ) //loop for each object in the array within the index of a array
                    { 
                        objectCount++;
                        
                        for ( var k = 0; k < tag.length; k++ ) //loop for every word in the title of an article and see it has keywords
                        {  
                            if ( words[j][word].toUpperCase() == tag[k].toUpperCase() )
                            {
                                var elementExists = document.getElementById(icons[j]['type']); //do not create more than one icon per category aka type
                                if (elementExists)
                                    continue;
                                    
                                var newDiv = document.createElement('div');
                                newDiv.id = icons[j]['type'];
                                newDiv.style.float = 'left';
                                div.appendChild(newDiv);
                                newText += '<li style="display: inline-block;" > <a href="' + data[i].link + '" target="_blank"> <img src="' + icons[j]['link'] + '" alt="' + tag[k] + '" /> </a> </li> </ul>';
                                document.getElementById(newDiv.id).innerHTML = newText;
                                newText = " "; //empty out to prevent duplicates
                            }
                        }
                    }
                }
            }
            
        })
        .fail(function(jqXHR, textStatus, errorThrown) {
    
            // log error to browser's console
            console.log(errorThrown.toString());
        });
    
    });
    
}

/**
 * Configures application.
 */
function configure()
{
    // update UI after map has been dragged
    google.maps.event.addListener(map, "dragend", function() {
        update();
    });

    // update UI after zoom level changes
    google.maps.event.addListener(map, "zoom_changed", function() {
        update();
    });

    // remove markers whilst dragging
    google.maps.event.addListener(map, "dragstart", function() {
        removeMarkers();
    });

    // configure typeahead
    // https://github.com/twitter/typeahead.js/blob/master/doc/jquery_typeahead.md
    $("#q").typeahead({
        autoselect: true,
        highlight: true,
        minLength: 1
    },
    {
        source: search,
        templates: {
            empty: "no places found yet",
            suggestion: _.template("<p> <%- place_name %>, <%- admin_name1 %>, <%- postal_code %> </p>") //It looks like what's returned from functions in an object is available for that objects scope 
        }
    });

    // re-center map after place is selected from drop-down
    $("#q").on("typeahead:selected", function(eventObject, suggestion, name) {

        // ensure coordinates are numbers
        var latitude = (_.isNumber(suggestion.latitude)) ? suggestion.latitude : parseFloat(suggestion.latitude);
        var longitude = (_.isNumber(suggestion.longitude)) ? suggestion.longitude : parseFloat(suggestion.longitude);

        // set map's center
        map.setCenter({lat: latitude, lng: longitude});

        // update UI
        update();
    });

    // hide info window when text box has focus
    $("#q").focus(function(eventData) {
        hideInfo();
    });

    // re-enable ctrl- and right-clicking (and thus Inspect Element) on Google Map
    // https://chrome.google.com/webstore/detail/allow-right-click/hompjdfbfmmmgflfjdlnkohcplmboaeo?hl=en
    document.addEventListener("contextmenu", function(event) {
        event.returnValue = true; 
        event.stopPropagation && event.stopPropagation(); 
        event.cancelBubble && event.cancelBubble();
    }, true);

    // update UI
    update();

    // give focus to text box
    $("#q").focus();
}

/**
 * Hides info window.
 */
function hideInfo()
{
    info.close();
}

/**
 * Removes markers from map.
 */
function removeMarkers()
{
    //
    markers.length = 0;
}

/**
 * Searches database for typeahead's suggestions.
 */
function search(query, cb)
{
    // get places matching query (asynchronously)
    var parameters = {
        geo: query
    };
    $.getJSON("search.php", parameters)
    .done(function(data, textStatus, jqXHR) {

        // call typeahead's callback with search results (i.e., places)
        cb(data);
    })
    .fail(function(jqXHR, textStatus, errorThrown) {

        // log error to browser's console
        console.log(errorThrown.toString());
    });
}

/**
 * Shows info window at marker with content.
 */
function showInfo(marker, content)
{
    // start div
    var div = "<div id='info'>";
    if (typeof(content) === "undefined")
    {
        // http://www.ajaxload.info/
        div += "<img alt='loading' src='img/ajax-loader.gif'/>";
    }
    else
    {
        div += content;
    }

    // end div
    div += "</div>";

    // set info window's content
    info.setContent(div);

    // open info window (if not already open)
    info.open(map, marker);
}

/**
 * Updates UI's markers.
 */
function update() 
{
    // get map's bounds
    var bounds = map.getBounds();
    var ne = bounds.getNorthEast();
    var sw = bounds.getSouthWest();

    // get places within bounds (asynchronously)
    var parameters = {
        ne: ne.lat() + "," + ne.lng(),
        q: $("#q").val(),
        sw: sw.lat() + "," + sw.lng()
    };
    $.getJSON("update.php", parameters)
    .done(function(data, textStatus, jqXHR) {

        // remove old markers from map
        removeMarkers();
   
        //Construct a new map
        var canvas = $("#map-canvas").get(0);
        var myLatlng = new google.maps.LatLng(data[0].latitude, data[0].longitude);
        var mapOptions = {
            zoom: 11,
            center: myLatlng
        };
        var map = new google.maps.Map(canvas, mapOptions); //remove mapOptions

        // add new markers to map
        for (var i = 0; i < data.length; i++)
        {
            addMarker(data[i], map);
        }
     })
     .fail(function(jqXHR, textStatus, errorThrown) {

         // log error to browser's console
         console.log(errorThrown.toString());
     });
}