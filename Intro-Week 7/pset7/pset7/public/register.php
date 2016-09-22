<?php

    // configuration
    require("../includes/config.php");

    // if user reached page via GET (as by clicking a link or via redirect)
    if ($_SERVER["REQUEST_METHOD"] == "GET")
    {
        // render form while staying on register.php
        render("register_form.php", ["title" => "Register"]); 
    }

    // else if user reached page via POST (as by submitting a form via POST) (body of http request, not url or header)
    else if ($_SERVER["REQUEST_METHOD"] == "POST")
    {
        // TODO: Check if either username or password is empty
        if ( (empty($_POST["username"])) || (empty($_POST["password"])) )
        {
            apologize("Please enter both username and password!");
        }
        
        // TODO: Check if user entered the correct password twice
        if ( $_POST["password"] != $_POST["confirmation"] )
        {
            apologize("Please enter matching passwords!"); 
        }
        
        //add user to database for future logins
        $rows = CS50::query("INSERT IGNORE INTO users (username, hash, cash) VALUES(?, ?, 10000.0000)", $_POST["username"], password_hash($_POST["password"], PASSWORD_DEFAULT));
        if ( $rows != 0 )
        {
            //log them in now!
            $rows = CS50::query("SELECT LAST_INSERT_ID() AS id");
            $id = $rows[0]["id"]; 
            $_SESSION["id"] = $id; //Session id stores id in a cookie file, then upon redirecting to another page,
                                //that page should via server instruction should tell browsers to check for cookie again before displaying the page
        
            //redirect to portfolio view inside index.php
            redirect("/index.php"); 
        }

        apologize("Username probably already exists!");
    }

?>
