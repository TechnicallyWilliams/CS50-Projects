<?php

        // configuration
    require("../includes/config.php");

        // if user reached page via GET (as by clicking a link or via redirect)
    if ($_SERVER["REQUEST_METHOD"] == "GET")
    {
        // else render form
        $check = false;
        render("buy_form.php", ["title" => "BUY", "ask" => $check]);
    }
        // else if user reached page via POST (as by submitting a form via POST) (body of http request, not url or header)
    else if ($_SERVER["REQUEST_METHOD"] == "POST" && $_POST["confirm"] == "no")
    {
        
        $symbol = $_POST["symbol"];
        $shares = $_POST["shares"];     
        
        //Send this variable to influence HTML DOM (Help user confirm their choice)
        $check = true; 
        
        //Send this variable to be returned by HTML DOM (reflect users confirmation)
        $confirm = false;
    
        render("buy_form.php", ["title" => "BUY", "ask" => $check, "confirming" => $confirm, "stockSymbol" => $symbol, "stockShares" => $shares]);
    }
    else if ($_SERVER["REQUEST_METHOD"] == "POST" && $_POST["confirm"] == "yes")
    {
            
        $confirm = $_POST["confirm"];
        $symbol = $_POST["symbol"];
        
        $shares = $_POST["shares"];
        
        $stock = lookup($symbol);
        if (!$stock)
        {
            apologize("That symbol does not return stock information!");
        }
        $cost = $stock["price"] * $shares;

        
        //returns true if and only if the post contains a non-negative integer
        if ( !preg_match("/^\d+$/", $shares) )
        {
           apologize("$shares were entered incorrectly!");
        }

        $sessionID = $_SESSION["id"]; 
        $userRows = CS50::query("SELECT cash FROM users WHERE id = ?", $sessionID); 
        
        $account = $userRows[0]["cash"];
        $leftOver = $account - $cost;
        $leftOver = number_format( $leftOver, 2, '.', ','); //Wait until end of calculation to modify the number
        
        if ($leftOver < 0)
        {
            apologize("You can't afford that... \n Cash Account: $account \n Cost: $cost \n leftOver: $leftOver");
        }
        
        //update database
        //store stock symbols in database as uppercase (don't force users to input symbols as uppercase)
        //buying shares with joint key language
        $newRows = CS50::query("INSERT INTO portfolios (user_id, symbol, shares) VALUES(?, ?, ?) ON DUPLICATE KEY UPDATE shares = shares + VALUES(shares)", $sessionID, strtoupper($symbol), $shares);
        
        //update users cash account
        $leftOver = $account - $cost;
        if ($newRows)
        {
            CS50::query("UPDATE users SET cash = $leftOver WHERE id = $sessionID"); 
        } else
        {
            apologize("THere something wrong with updating cash");
        }
        
        //update history table with transation
        $transaction = "bought";
        CS50::query("INSERT INTO history (user_id, symbol, transaction, shares, shareprice) VALUES(?, ?, ?, ?, ?)", $sessionID, strtoupper($symbol), $transaction, $shares, $stock["price"]);
        
        //Return to original view
        $check = false;
        render("buy_form.php", ["title" => "BUY", "ask" => $check]);
        
    } 
    else
    {
         apologize("THere something wrong with your oder!");
    }


?>
