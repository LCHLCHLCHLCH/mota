return {
    map = {
        {

		2,2,2,2,2,2,2,2,2,2,2,2,2},
        {2,57,1,4,1,1,1,1,1,1,1,9,2},
        {2,1,56,2,2,2,1,2,2,2,1,1,2},
        {2,3,2,2,1,3,1,3,1,2,2,2,2},
        {2,119,1,120,1,2,1,2,118,2,51,55,2},
        {2,1,51,1,120,2,1,2,1,3,1,51,2},
        {2,2,2,2,2,2,1,2,1,2,2,2,2},
        {2,51,51,51,51,2,125,2,124,2,51,52,2},
        {2,1,1,1,52,2,1,2,1,3,1,51,2},
        {2,2,8,2,2,2,1,2,2,2,2,2,2},
        {2,121,1,121,1,1,1,1,1,1,155,1,2},
        {2,1,1,1,1,10,1,2,118,1,1,1,2},
        {2,2,2,2,2,2,2,2,2,2,2,2,2}
    },
    events = {
        {
            trigger = "on_guard_kill",
            guards = {{x = 1, y = 10}, {x = 3, y = 10}},
            once = true,
            run = function()
                set_tile(2, 9, 1)
                msg("守卫门已打开")
            end
        },
        {
            trigger = "on_tile",
            x = 6, y = 7,
            once = true,
            run = function()
                say("骑士队长：你打败了前两个区域的头目，这表明你是个勇士。但现在游戏结束了，我将在这里亲手杀死你！")
            end
        },
        {
            trigger = "on_guard_kill",
            guards = {{x = 6, y = 7}},
            run = function()
                say("骑士队长：你以为你已非常强大了吗？只是我今天状态不佳而已。有本事到40楼与我再打一次。")
            end
        },
        {
                        trigger = "on_tile",
            tile = 155,
            run = function()
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
            end
        }
    }
}
