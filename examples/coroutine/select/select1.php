<?php declare(strict_types = 1);

// Example of co::select

Co\run(function() {
    $c1 = new chan();
    $num = 10;
    go(function () use ($c1, $num) {
        $pull_list = [$c1];
        $push_list = [];
        echo "select yield\n";
        $result = co::select($pull_list, $push_list, 2);
        echo "select resume res: ".var_export($result, true)."\n";
        if ($pull_list)
        {
            foreach($pull_list as $ch)
            {
                for ($i=0;$i<$num;$i++)
                {
                    $ret = $ch->pop();
                    echo "pop [#$i] ret:".var_export($ret, true)."\n";
                }
            }
        }
    });

    go(function () use ($c1,$num) {
        echo "push start\n";
        for ($i=0;$i<$num;$i++)
        {
            $ret = $c1->push("data-$i");
            echo "push [#$i] ret:". var_export($ret, true)."\n";
        }

    });
    echo "main end\n";
});
