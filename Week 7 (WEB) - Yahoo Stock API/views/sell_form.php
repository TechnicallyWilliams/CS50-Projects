<table class="table table-striped">

    <thead>
        <tr>
            <th>Symbol</th>
            <th>Name</th>
            <th>Shares</th>
            <th>Price</th>
            <th>TOTAL</th>
            <th>Option</th>
        </tr>
    </thead>

    <tbody>
    <?php foreach ($portfolio as $position): ?>
        <tr style="text-align:left">
            
            <td><?= $position["symbol"] ?></td>
            <td><?= $position["name"] ?></td>
            <td><?= $position["shares"] ?></td>
            <td>$<?= $position["price"] ?></td>
            <td>$<?= $position["shares"] * $position["price"] ?></td>
            <td>
                <form action='sell.php' method='post'>
                    <div class='form-group'>
                        <button class='btn btn-default' type='submit'>
                <span aria-hidden='true' class='glyphicon glyphicon-log-in'></span>
                    sell
                        </button>
                    </div>
                    <div class='form-group' style="display: none;">
                        <input autocomplete='off' name='shares' type='text' value=<?php print($position["shares"]); ?> />
                        <input autocomplete='off' name='symbol' type='text' value=<?php print($position["symbol"]); ?> />
                    </div>
                </form>
            </td>
        </tr>
    <?php endforeach ?>
    
    <tr style="text-align:right">
        <td colspan="4"><strong>CASH</strong></td>
        <td>$<?= $amount[0]["cash"] ?></td>
    </tr>
    </tbody>

</table>