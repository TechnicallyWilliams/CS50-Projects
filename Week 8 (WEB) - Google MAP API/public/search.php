<?php

    require(__DIR__ . "/../includes/config.php");
    // TODO: search database for places matching $_GET["geo"], store in $places
    //search th database for places mathincg a geo http parameter
    // so that when you contact this file with a url like search.php?geo=something your code will return a json array of all the places
    //in your database table that match that input
    if ($_SERVER["REQUEST_METHOD"] == "GET")
    {
        // numerically indexed array of places
        $places = [];
        $array1 = [];
        $array2 = [];
    
        $_GET["geo"];
                            //Previous: SELECT postal_code, place_name, admin_name1 
        $array1 = CS50::query("SELECT * FROM places WHERE admin_name1 LIKE ?", "%" . $_GET["geo"] . "%"); 
        
        if (!$array1)
        {                   //Previous: SELECT postal_code, place_name, admin_name1 
            $places = CS50::query("SELECT * FROM places WHERE MATCH(postal_code, place_name) AGAINST (?)", $_GET["geo"]); 
        } else
        {                   //Previous: SELECT postal_code, place_name, admin_name1 
            $array2 = CS50::query("SELECT * FROM places WHERE MATCH(postal_code, place_name) AGAINST (?)", $_GET["geo"]);
            $places = array_merge($array1, $array2);
        }

    }

    // output places as JSON (pretty-printed for debugging convenience)
    header("Content-type: application/json");
    print(json_encode($places, JSON_PRETTY_PRINT));

?>