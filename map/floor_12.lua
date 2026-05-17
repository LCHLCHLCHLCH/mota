return {
    map = {
        {

		2,2,2,2,2,2,2,2,2,2,2,2,2},
        {2,152,11,1,2,51,56,51,2,1,11,11,2},
        {2,2,2,110,2,1,2,1,2,1,2,2,2},
        {2,1,1,1,2,111,1,111,2,1,112,1,2},
        {2,3,2,2,2,2,3,2,2,2,2,1,2},
        {2,1,111,1,3,1,112,1,2,54,1,110,2},
        {2,2,2,2,2,1,1,111,3,1,57,1,2},
        {2,51,51,1,2,1,55,1,2,51,1,109,2},
        {2,51,52,1,2,2,2,2,2,4,2,3,2},
        {2,1,1,112,2,1,155,1,2,109,1,112,2},
        {2,2,2,3,2,54,1,54,2,2,1,2,2},
        {2,9,1,1,103,1,1,1,103,1,1,10,2},
        {2,2,2,2,2,2,2,2,2,2,2,2,2}
    },
    events = {
        {
            trigger = "on_tile",
            x = 1, y = 1,
            actions = {
                { type = "call", func = function()
                    if has_flag(3) then
                        set("x", 1)
                        set("y", 1)
                        return
                    end
                    if has_flag(2) then
                        say("商人：你是否注意到5、9、14、16、18楼的墙与众不同？")
                        set_tile(1, 1, 1)
                        set_flag(3)
                        return
                    end
                    say("商人：我有一把红钥匙，你出800个金币我就卖给你")
                    local c = choose_menu("我太需要了", "下次再说")
                    if c == 0 then
                        if take_money(800) then
                            give(53)
                            set_flag(2)
                        else
                            say("商人：你的金币不够！")
                        end
                    end
                    drain()
                end },
            }
        },
        {
            trigger = "on_tile",
            x = 11, y = 1,
            actions = {
                { type = "call", func = function()
                    if not has_flag(1) then
                        set_tile(11, 1, 152)
                        set_flag(1)
                        msg("墙壁裂开，出现了一个商人！")
                        return
                    end
                    say("商人：我有很多黄钥匙，1000金币一把你要吗？")
                    local c = choose_menu("我太需要了", "下次再说")
                    if c == 0 then
                        if take_money(1000) then
                            give(51)
                        else
                            say("商人：你的金币不够！")
                        end
                    end
                    drain()
                end },
            }
        },
        {
                        trigger = "on_tile",
            tile = 155,
            actions = {
                { type = "call", func = function()
                    local t = altar_times()
                    local r = (player_floor() - 1) // 10 + 1
                    local cost = 20 + 10 * (t + 1) * t
                    local hp = 100 * (t + 1)
                    local atk = 2 * r
                    local def = 4 * r
                    say("供奉"..cost.."金币，便可以增加你的力量，你想要什么呢……")
                    local c = choose_menu("生命+"..hp, "攻击+"..atk, "防御+"..def, "离开")
                    if c < 3 and take_money(cost) then
                        if c == 0 then add_health(hp)
                        elseif c == 1 then add_attack(atk)
                        elseif c == 2 then add_defence(def) end
                        altar_tick()
                        drain()
                    elseif c < 3 then
                        say("你的金币不足，无法供奉！")
                    end
                end }
            }
        }
    }
}
