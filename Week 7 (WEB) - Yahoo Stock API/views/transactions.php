<table class="table table-striped">

    <thead>
        <tr>
            <th>Symbol</th>
            <th>Price</th>
            <th>Transaction</th>
            <th>Shares</th>
        </tr>
    </thead>

    <tbody>
    <?php foreach ($transactions as $transaction): ?>
        <tr  style="text-align:left">
            <td><?= $transaction["symbol"] ?></td>
            <td>$<?= $transaction["shareprice"] ?></td>
            <td><?= $transaction["transaction"] ?></td>
            <td><?= $transaction["shares"] ?></td>
        </tr>
    <?php endforeach ?>
    </tbody>

</table>