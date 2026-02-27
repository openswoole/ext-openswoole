<?php
/*
 +----------------------------------------------------------------------+
 | OpenSwoole                                                          |
 +----------------------------------------------------------------------+
 | Copyright (c) 2017-now OpenSwoole Group                            |
 | Copyright (c) 2012-2017 The Swoole Group                             |
 +----------------------------------------------------------------------+
 | This source file is subject to version 2.0 of the Apache license,    |
 | that is bundled with this package in the file LICENSE, and is        |
 | available through the world-wide-web at the following url:           |
 | http://www.apache.org/licenses/LICENSE-2.0.html                      |
 | If you did not receive a copy of the Apache2.0 license and are unable|
 | to obtain it through the world-wide-web, please send a note to       |
 | hello@swoole.co.uk so we can mail you a copy immediately.            |
 +----------------------------------------------------------------------+
 | Author: Tianfeng Han  <mikan.tenny@gmail.com>                        |
 +----------------------------------------------------------------------+
 */

namespace SwooleTest;

use OpenSwoole\Coroutine;

class WaitRef
{
    private $cid;

    public function __destruct()
    {
        Coroutine::resume($this->cid);
    }

    static function create()
    {
        return new static();
    }

    static function wait(WaitRef &$wr)
    {
        $wr->cid = Coroutine::getCid();
        $wr = null;
        Coroutine::yield();
    }
}