<table class="table table-striped">

    <thead>
        <tr>
            <th>Symbol</th>
            <th>Name</th>
            <th>Shares</th>
            <th>Price</th>
            <th>TOTAL</th>
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
        </tr>
    <?php endforeach ?>
    <tr>
        <td style="text-align:left">
            <form action='index.php' method='post'>
                <div class='form-group'>
            <input autocomplete='off' name='credit' type='text' placeholder="$1.00" />
                    <button class='btn btn-default' type='submit'>
            <span aria-hidden='true' class='glyphicon glyphicon-log-in'></span>
                ADD CASH
                    </button>
                </div>
            </form>
        </td>
    <td style="text-align:right" colspan="3"><strong>CASH</strong></td>
    <td>$<?= $amount[0]["cash"] ?></td>
    </tr>
    </tbody>

</table>


