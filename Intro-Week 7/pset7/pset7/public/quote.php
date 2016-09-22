<?php

    // configuration
    require("../includes/config.php");

    // if user reached page via GET (as by clicking a link or via redirect)
    if ($_SERVER["REQUEST_METHOD"] == "GET")
    {
        // render form
        render("stock_form.php", ["title" => "Symbol"]); //this argument of title is the tab because html special char pulls from whatever you called the key
                                                        //the key could be called anything, it's jut a variable to hold values
    }

    // else if user reached page via POST (as by submitting a form via POST) (body of http request, not url or header)
    else if ($_SERVER["REQUEST_METHOD"] == "POST")
    {
        
        $stock = lookup($_POST["symbol"]);
        
        if (!$stock)
        {
            apologize("That symbol does not return stock information!");
        }
        
        $stock["price"] = number_format( $stock["price"], 2, '.', ',');
        
        //assign a masterkey called "stockData" to hold and pass the associatve array $stock
        render("price.php", ["title" => "Price", "stockData" => $stock]); 
        
    }

?>
