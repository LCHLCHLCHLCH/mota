return {
    map = {
        {

		2,2,2,2,2,2,2,2,2,2,2,2,2},
        {2,2,2,2,1,1,1,1,1,2,2,2,2},
        {2,2,2,1,1,131,131,131,1,1,2,2,2},
        {2,2,1,1,1,131,133,131,1,1,1,2,2},
        {2,1,1,1,1,131,131,131,1,1,1,1,2},
        {2,1,2,1,1,1,1,1,1,1,2,1,2},
        {2,1,2,1,1,1,1,1,1,1,2,1,2},
        {2,1,2,1,2,2,8,2,2,1,2,1,2},
        {2,1,2,1,2,130,1,130,2,1,2,1,2},
        {2,1,1,1,2,2,8,2,2,1,1,1,2},
        {2,2,2,2,2,127,1,127,2,2,2,2,2},
        {2,10,1,5,1,1,1,1,1,1,1,1,2},
        {2,2,2,2,2,2,2,2,2,2,2,2,2}
    },
    events = {
        {
            trigger = "on_guard_kill",
            guards = {
                {x = 6, y = 2}, {x = 5, y = 3}, {x = 7, y = 3}, {x = 6, y = 4},
            },
            once = true,
            run = function()
                -- 当且仅当正方向四守卫(2,4,6,8)全死、且对角四守卫(1,3,7,9)全存活时，假魔王被封印
                if get_tile(5, 2) ~= 1 and get_tile(7, 2) ~= 1 and
                   get_tile(5, 4) ~= 1 and get_tile(7, 4) ~= 1 then
                    set_tile(6, 3, 134)
                    say("魔王：啊！我怎么被封印了，我只剩下一成的功力了！！！")
                end
            end
        },
        {
            trigger = "on_guard_kill",
            guards = {{x = 5, y = 8}, {x = 7, y = 8}},
            run = function()
                set_tile(6, 7, 1)
                msg("守卫门已打开")
            end
        },
        {
            trigger = "on_guard_kill",
            guards = {{x = 5, y = 10}, {x = 7, y = 10}},
            run = function()
                set_tile(6, 9, 1)
                msg("守卫门已打开")
            end
        },
        {
            trigger = "on_tile",
            x = 6, y = 6,
            once = true,
            run = function()
                set("x", 6)
                set("y", 6)
                say("魔王：你终于来了，我很想与你立刻决斗，但我的部下不同意。")
            end
        },
        {
            trigger = "on_guard_kill",
            guards = {{x = 6, y = 3}},
            run = function()
                say("魔王：哈哈，很好，你是个合格的战士。")
                -- 清除所有魔法警卫
                for y = 0, 12 do
                    for x = 0, 12 do
                        if get_tile(x, y) == 131 then
                            set_tile(x, y, 1)
                        end
                    end
                end
                -- 奖励物品：红钥匙 (5,2)，屠龙匕首 (7,2)
                set_tile(5, 2, 53)
                set_tile(7, 2, 81)
                -- 红宝石 (2,4)(3,4)(4,4)
                set_tile(2, 4, 56)
                set_tile(3, 4, 56)
                set_tile(4, 4, 56)
                -- 蓝宝石 (8,4)(9,4)(10,4)
                set_tile(8, 4, 57)
                set_tile(9, 4, 57)
                set_tile(10, 4, 57)
                -- 蓝血瓶 (5,5)(6,5)(7,5)
                set_tile(5, 5, 55)
                set_tile(6, 5, 55)
                set_tile(7, 5, 55)
            end
        }
    }
}
