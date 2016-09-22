<?php

    // configuration
    require("../includes/config.php");
    
    if ($_SERVER["REQUEST_METHOD"] == "GET")
    {
        $sessionID = $_SESSION["id"];
       
        $userRows = CS50::query("SELECT cash FROM users WHERE id = ?", $sessionID); 
        $userRows[0]["cash"] = number_format( $userRows[0]["cash"], 2, '.', ',');
        
        $rows = CS50::query("SELECT symbol, shares FROM portfolios WHERE user_id = ?", $sessionID); 
           
        $positions = []; //generally best not to alter functions' return values (like $rows from query)
        foreach ($rows as $masterKey)
        {
            $stock = lookup($masterKey["symbol"]);
        
            if ($stock !== false)
            {
                $stock["price"] = number_format( $stock["price"], 2, '.', ',');
            
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
           
        //need to pass ebtire portfolio and cash on hand to view
        render("sell_form.php", ["amount" => $userRows, "title" => "Sell", "portfolio" => $positions]);
 
    } else if ($_SERVER["REQUEST_METHOD"] == "POST")
    {
        $stock = lookup($_POST["symbol"]);
        
        if (!$stock)
        {
            apologize("That symbol does not return stock information! ---> $stock");
        }
        
        //go to cash and increase it by current price * total number of shares owned
        $UserID = $_SESSION["id"];
        $revenue = $stock["price"] * $_POST["shares"];
        $userRows = CS50::query("SELECT cash FROM users WHERE id = ?", $UserID);
        $newTotal = $userRows[0]["cash"] + $revenue; 
        CS50::query("UPDATE users SET cash = $newTotal WHERE id = $UserID");
        
        //remove company from portfolio (user is forced to sell all shares for simplicity)
        $symbol = $_POST["symbol"];
        CS50::query("DELETE FROM portfolios WHERE user_id = ? AND symbol = ?", $UserID, $symbol);
        
        //record transaction into database history table
        $transaction = "sold";
        $price = $stock["price"];
        $shares = $_POST["shares"];
        CS50::query("INSERT INTO history (user_id, symbol, transaction, shares, shareprice) VALUES(?, ?, ?, ?, ?)", $UserID, $symbol, $transaction, $shares, $price);
        
        redirect("index.php");
        
      
    } else 
    {
        apologize("THere something wrong with your oder!");
    }
     
    
?>
