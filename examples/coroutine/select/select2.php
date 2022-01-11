<?php declare(strict_types = 1);

Co\run(function() {
    $c1 = new chan(1);
    $num = 10;
    go(function () use ($c1, $num) {
        echo "push start\n";
        for ($i=0;$i<$num;$i++)
        {
            $ret = $c1->push("data-$i");
            echo "push [#$i] ret:".var_export($ret, true)."\n";
        }
    });

    go(function () use ($c1, $num) {
        $read_list = [$c1];
        $write_list = [];
        echo "select yield\n";
        $result = co::select($read_list, $write_list, 2);
        echo "select resume res: ".var_export($result, true)."\n";
        if ($read_list)
        {
            foreach($read_list as $ch)
            {
                for ($i=0;$i<$num;$i++)
                {
                    $ret = $ch->pop();
                    echo "pop [#$i] ret:".var_export($ret, true)."\n";
                }
            }
        }
    });
    echo "main end\n";
});