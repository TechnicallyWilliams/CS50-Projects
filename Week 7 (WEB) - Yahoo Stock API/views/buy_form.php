<?php 
    
    if ($ask == false) {
     ?>
     <h3>Enter a stock symbol and purchase shares:</h3>
     
        <form action='buy.php' method='post'>
            <div class='form-group'>
                <input autocomplete='off' autofocus class='form-control' name='symbol' placeholder='Symbol' type='text'/>
          </div>
            <div class='form-group'>
               <input autocomplete='off' name='shares' placeholder='Number of Shares' type='text'/>
            </div>
            <div class='form-group' style="display: none;">
             <input autocomplete='off' name='confirm' value="no" type='text'/>
            </div>
            <div class='form-group'>
                <button class='btn btn-default' type='submit'>
            <span aria-hidden='true' class='glyphicon glyphicon-log-in'></span>
                Buy
            </button>
            </div>
        </form>
    <?php
     } else 
     { ?>
            <h4>Stock Symbol: <?php print($stockSymbol); ?> </h4>
            <h4>Number of Shares: <?php print($stockShares); ?> </h4>
             
         <div>
            <form action="buy.php" method='post'>
            <div class='form-group' style="display: none;">
         <input autocomplete='off' name='symbol' type='text' value=<?php print($stockSymbol); ?> />
            </div>
            <div class='form-group' style="display: none;">
        <input autocomplete='off' name='shares' type='text' value=<?php print($stockShares); ?>  />
            </div>
            <div class='form-group' style="display: none;">
                <input autocomplete='off' name='confirm' type='text' value='yes'/>
            </div>
            
             <div class='form-group'>
                <button class='btn btn-default' type='submit'>
                    <span aria-hidden='true' class='glyphicon glyphicon-log-in'></span>
                    Confirm
                </button>
            </div>
            </form>
        </div>
        
        <div>
            <form action="buy.php" method='get'>
            <div class='form-group' style="display: none;">
                <input autocomplete='off' autofocus class='form-control' name='confirm' type='text' value='no'/>
            </div>
            
             <div class='form-group'>
                <button class='btn btn-default' type='submit'>
                    <span aria-hidden='true' class='glyphicon glyphicon-log-in'></span>
                    Cancel
                </button>
            </div>
            </form>
        </div>
            
     <?php }
     ?>