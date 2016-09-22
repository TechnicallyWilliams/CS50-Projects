<?php

    // configuration
    require("../includes/config.php"); //checks session id 
    
    
    if ($_SERVER["REQUEST_METHOD"] == "GET")
    {
        $sessionID = $_SESSION["id"];
       
        $userRows = CS50::query("SELECT cash FROM users WHERE id = ?", $sessionID); 
        $userRows[0]["cash"] = number_format( $userRows[0]["cash"], 2, '.', ',');
        
        //Pull all current shares held by current user from portfolios table
        $rows = CS50::query("SELECT symbol, shares FROM portfolios WHERE user_id = ?", $sessionID); 
           
        $positions = []; //generally best not to alter functions' return values (like $rows from query)
        foreach ($rows as $masterKey)
        {
            $stock = lookup($masterKey["symbol"]); //lookup talks to third-party. Possibly Yahoo API
        
            if ($stock !== false)
            {
                //Modifies number to 2 decimal places for viewing
                $stock["price"] = number_format( $stock["price"], 2, '.', ',');
            
                //This initializes a the new array's keys with the old array's values
                $positions[] = [
                    "name" => $stock["name"], 
                    "price" => $stock["price"],
                    "shares" => $masterKey["shares"],
                    "symbol" => $masterKey["symbol"]
                ];
            
            } else
            {
                apologize("There was a problem with loading your portfolio!");
            }
        }
           
        //assigning a new masterKey called "portfolio" to the associative array $positions for passing to the view
        render("portfolio.php", ["amount" => $userRows, "title" => "Portfolio", "portfolio" => $positions]);
        
    } else if ($_SERVER["REQUEST_METHOD"] == "POST")
    {
        //Check if an amount was entered in the form to credit their account with
        if ($_POST["credit"])
        {
            $sessionID = $_SESSION["id"];
            $credit = ltrim($_POST["credit"], '$'); //Removes the $ if there is one
            
            $userRows = CS50::query("SELECT cash FROM users WHERE id = ?", $sessionID);
            $newAmount = $credit + $userRows[0]["cash"];
            CS50::query("UPDATE users SET cash = $newAmount WHERE id = $sessionID");
            
            //return to portfolio page
            redirect("index.php");
        }
        
        //return to portfolio page faster
        redirect("index.php");
        
    } else
    {
        apologize("There was a problem with loading this page!");
    }
    

?>
