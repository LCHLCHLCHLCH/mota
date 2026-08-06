return {
    map = {
        {

		2,2,2,2,2,2,2,2,2,2,2,2,2},
        {2,9,1,3,113,1,120,3,1,1,1,10,2},
        {2,1,1,2,1,54,1,2,3,2,2,2,2},
        {2,4,2,2,151,1,51,2,1,1,1,55,2},
        {2,1,54,2,2,3,2,2,2,2,1,2,2},
        {2,120,1,2,1,1,113,1,2,118,1,118,2},
        {2,1,1,2,122,2,2,3,2,1,1,1,2},
        {2,1,120,3,1,1,118,1,2,119,1,119,2},
        {2,3,2,2,2,2,2,2,2,2,1,2,2},
        {2,1,1,113,2,1,119,1,2,1,1,1,2},
        {2,122,2,1,2,51,2,120,1,1,62,1,2},
        {2,51,119,1,4,1,3,1,2,1,1,1,2},
        {2,2,2,2,2,2,2,2,2,2,2,2,2}
    },
    events = {
        {
            trigger = "on_tile",
            x = 10, y = 5,
            once = true,
            run = function()
                set("x", 10)
                set("y", 5)
                set_flag(2)
                -- 若四名守卫已被提前击杀，则不困住玩家，避免软锁
                if get_tile(9, 5) == 1 and get_tile(11, 5) == 1 and
                   get_tile(9, 7) == 1 and get_tile(11, 7) == 1 then
                    return
                end
                set_tile(10, 4, 8)
                set_tile(10, 8, 8)
                msg("你被困住了！")
            end
        },
        {
            trigger = "on_guard_kill",
            guards = {{x = 9, y = 5}, {x = 11, y = 5}, {x = 9, y = 7}, {x = 11, y = 7}},
            run = function()
                if not has_flag(2) then return end
                if has_flag(3) then return end
                set_tile(10, 4, 1)
                set_tile(10, 8, 1)
                set_flag(3)
                msg("守卫门已打开")
            end
        },
        {
            trigger = "on_tile",
            tile = 151,
            condition_flag = 4,
            set_flag = 4,
            run = function()
                say("老人：别匆忙，降低速度！")
                set_tile(4, 3, 1)
            end
        }
    }
}
