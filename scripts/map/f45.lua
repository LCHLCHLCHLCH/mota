return {
    map = {
        {

		2,2,2,2,2,2,2,2,2,2,2,2,2},
        {2,10,1,1,1,1,1,1,1,1,1,9,2},
        {2,2,2,2,2,2,3,2,2,2,2,2,2},
        {2,56,56,1,2,126,1,127,2,152,1,1,2},
        {2,4,2,130,2,1,1,1,2,1,1,128,2},
        {2,57,57,1,2,127,1,126,2,2,2,3,2},
        {2,4,2,130,2,2,3,2,2,151,2,1,2},
        {2,1,1,1,3,1,1,129,1,1,1,51,2},
        {2,2,5,2,2,2,2,2,2,2,127,1,2},
        {2,1,1,1,2,130,1,2,131,2,2,3,2},
        {2,1,76,1,8,1,1,8,1,3,1,1,2},
        {2,1,1,1,2,130,1,2,131,2,1,55,2},
        {2,2,2,2,2,2,2,2,2,2,2,2,2}
    },
    events = {
        {
            trigger = "on_tile",
            tile = 151,
            once = true,
            run = function()
                say("老人：44楼被藏在异空间，你只能用密宝才能到达。")
                note("老人", "44楼被藏在异空间，你只能用密宝才能到达。")
                set_tile(9, 6, 1)
            end
        },
        {
            trigger = "on_tile",
            tile = 152,
            run = function()
                if has_flag(2) then return end
                if has_flag(1) then
                    say("商人：神圣盾能防御魔法攻击，但它被藏在异空间的楼层内。")
                    note("商人", "神圣盾能防御魔法攻击，但它被藏在异空间的楼层内。")
                    set_tile(9, 3, 1)
                    set_flag(2)
                    return
                end
                say("给我1000个金币，我就提升你生命值2000点。")
                local c = choose_menu("我太需要了", "下次再说")
                if c == 0 then
                    if take_money(1000) then
                        add_health(2000)
                        set_flag(1)
                    else
                        say("商人：你的金币不够！")
                    end
                end
                drain()
            end
        },
        {
            trigger = "on_guard_kill",
            guards = {{x = 5, y = 9}, {x = 5, y = 11}},
            run = function()
                set_tile(4, 10, 1)
                msg("守卫门已打开")
            end
        },
        {
            trigger = "on_guard_kill",
            guards = {{x = 8, y = 9}, {x = 8, y = 11}},
            run = function()
                set_tile(7, 10, 1)
                msg("守卫门已打开")
            end
        }
    }
}
