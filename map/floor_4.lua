return {
    map = {
        {

        2,2,2,2,2,2,2,2,2,2,2,2,2},
        {2,1,52,1,2,1,155,1,2,1,151,1,2},
        {2,54,1,51,2,1,1,1,2,51,1,55,2},
        {2,1,104,1,2,1,1,1,2,1,106,1,2},
        {2,2,3,2,2,2,4,2,2,2,3,2,2},
        {2,1,1,1,3,1,102,1,1,105,1,1,2},
        {2,1,1,1,2,2,2,2,2,2,2,2,2},
        {2,102,1,101,1,1,1,1,1,1,1,1,2},
        {2,3,2,2,3,2,2,2,3,2,2,3,2},
        {2,1,2,1,103,1,2,1,104,1,2,1,2},
        {2,1,2,101,1,51,2,56,1,54,2,1,2},
        {2,9,2,51,101,51,2,1,101,1,2,10,2},
        {2,2,2,2,2,2,2,2,2,2,2,2,2}
    },
    events = {
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
        },
        {
            trigger = "on_tile",
            tile = 151,
            once = true,
            run = function()
                say("有些门不能用钥匙打开，只有当你击败它的守卫后才会自动打开。")
                set_tile(10, 1, 1)
            end
        }
    }
}
