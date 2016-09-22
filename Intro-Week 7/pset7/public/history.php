<?php

    require("../includes/config.php"); //checks session id
    
    $sessionID = $_SESSION["id"]; 
   
    
    //Pull entire list of transactions with this specific user
    $rows = CS50::query("SELECT * FROM history WHERE user_id = ?", $sessionID); 
       
    if ($rows !== false)
    {
    
        $history = []; //generally best not to alter functions' return values (like $rows from query)
        foreach ($rows as $masterKey) //$masterKey is a masterkey you make up for the time being, just like when passing arguments to render
        {
            $masterKey["shareprice"] = number_format( $masterKey["shareprice"], 2, '.', ',');
        
            //This passes previous values to a new associative array and it's new keys with them. 
            $history[] = [
                "symbol" => $masterKey["symbol"],
                "shareprice" => $masterKey["shareprice"],
                "transaction" => $masterKey["transaction"], 
                "shares" => $masterKey["shares"]
            ];
        }
        
    } else
    {
            apologize("There was a problem with loading your history!");
    }
        
    //assigning a new masterKey called "transactions" to the associative array for passing to a new scope
    render("transactions.php", ["title" => "transactions", "transactions" => $history]);
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
//no memory

#portfolios hasss
#id ; userid ; symbol ; shares //userid is the foreign key
//always updated //sessionid from registration is used to create a new row;//usersid is generated auto when you register


#users has
#id ; username; hash; cash //fk is userid

//needs to be updated in the buy and sell controllers but displayed via the history controller
#history
#id; user_id ; symbol ; bought/sold; number of shares bought/sold; share price at time; date and time


?>