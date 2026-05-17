return {
    map = {
        {

		2,2,2,2,2,2,2,2,2,2,2,2,2},
        {2,1,1,1,1,1,1,1,1,1,1,1,2},
        {2,2,2,2,2,1,1,1,2,2,2,2,2},
        {2,105,105,105,2,2,1,2,2,105,105,105,2},
        {2,1,106,1,8,1,108,1,8,1,106,1,2},
        {2,2,2,2,2,1,1,1,2,2,2,2,2},
        {2,105,57,105,2,2,1,2,2,105,56,105,2},
        {2,1,106,1,2,2,1,2,2,1,106,1,2},
        {2,1,1,1,2,2,1,2,2,1,1,1,2},
        {2,3,2,3,2,2,5,2,2,3,2,3,2},
        {2,1,2,1,2,1,1,1,2,1,2,1,2},
        {2,10,2,1,104,1,1,1,104,1,2,55,2},
        {2,2,2,2,2,2,2,2,2,2,2,2,2}
    },
    events = {
        {
            trigger = "on_tile",
            x = 6, y = 5,
            once = true,
            actions = {
                { type = "call", func = function()
                    set("x", 6)
                    set("y", 5)
                    say("哈哈哈，能安全到达这里是你的运气，但现在好运已经离你而去，你中埋伏了！兄弟们给我上！")
                    -- 墙 → 空地
                    set_tile(5, 6, 1)
                    set_tile(7, 6, 1)

                    -- 移除已有的骷髅人
                    set_tile(1, 3, 1)
                    set_tile(2, 3, 1)
                    set_tile(3, 3, 1)
                    set_tile(2, 4, 1)

                    set_tile(9, 3, 1)
                    set_tile(10, 3, 1)
                    set_tile(11, 3, 1)
                    set_tile(10, 4, 1)

                    -- 骷髅人 105
                    set_tile(5, 4, 105)
                    set_tile(5, 5, 105)
                    set_tile(5, 6, 105)
                    set_tile(7, 4, 105)
                    set_tile(7, 5, 105)
                    set_tile(7, 6, 105)
                    -- 骷髅士兵 106
                    set_tile(6, 4, 106)
                    set_tile(6, 6, 106)
                    -- 骷髅队长 108
                    set_tile(6, 1, 108)
                    -- 守卫门 8
                    set_tile(6, 3, 8)
                    set_tile(6, 7, 8)
                end },
            }
        },
        {
            trigger = "on_guard_kill",
            guards = {
                {x = 5, y = 4}, {x = 5, y = 5}, {x = 5, y = 6},
                {x = 7, y = 4}, {x = 7, y = 5}, {x = 7, y = 6},
                {x = 6, y = 4}, {x = 6, y = 6},
            },
            once = true,
            actions = {
                { type = "call", func = function()
                    set_tile(6, 3, 1)
                    say("你怎么能杀出重围？我是不会让你通过的，来吧，我要与你决斗！")
                end},
            }
        },
        {
            trigger = "on_guard_kill",
            guards = {{x = 6, y = 1}},
            once = true,
            actions = {
                {type = "call", func = function()
                    set_tile(6, 7, 1)
                    say("不，这是不可能的，你怎会打败我！你别太得意，后面还有许多强大的对手存在，你稍有疏忽就必死无疑。")

                    -- 宝石
                    set_tile(1, 3, 56)
                    set_tile(2, 3, 56)
                    set_tile(3, 3, 56)

                    set_tile(9, 3, 57)
                    set_tile(10, 3, 57)
                    set_tile(11, 3, 57)

                    --蓝血瓶
                    set_tile(1, 4, 55)
                    set_tile(2, 4, 55)
                    set_tile(3, 4, 55)

                    -- 黄钥匙
                    set_tile(9, 4, 51)
                    set_tile(10, 4, 51)
                    set_tile(11, 4, 51)

                    -- 怪物门
                    set_tile(4, 4, 1)
                    set_tile(8, 4, 1)

                    -- 上行楼梯
                    set_tile(6, 11, 9)
                    -- 小偷
                    set_tile(6, 10, 154)
                    set_flag(1)
                end},
            }
        },
        {
            trigger = "on_tile",
            tile = 154,
            once = true,
            actions = {
                { type = "call", func = function()
                    say("嘿，我们又见面了！非常感谢你打败了此区域的头目。我正苦恼于怎么到达更高的楼层，现在终于可以上去了。我听说银盾在11楼，银剑在17楼，希望这消息对你有用。")
                    set_tile(6, 10, 1)
                end },
            }
        }
    }
}
