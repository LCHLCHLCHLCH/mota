return {
    map = {
        {

		2,2,2,2,2,2,2,2,2,2,2,2,2},
        {2,10,1,4,1,1,1,1,1,1,1,1,2},
        {2,1,1,2,2,1,121,1,121,1,2,2,2},
        {2,1,2,2,2,2,2,1,2,2,2,2,2},
        {2,1,2,51,51,2,1,1,1,2,1,151,2},
        {2,1,2,51,1,8,1,1,1,8,1,1,2},
        {2,1,2,2,2,2,1,1,1,2,2,2,2},
        {2,1,2,154,1,2,1,1,1,2,1,152,2},
        {2,1,2,1,1,8,1,1,1,8,1,1,2},
        {2,1,2,2,2,2,1,1,1,2,2,2,2},
        {2,1,2,55,55,2,1,1,1,2,1,1,2},
        {2,9,2,55,1,8,1,1,1,8,1,1,2},
        {2,2,2,2,2,2,2,2,2,2,2,2,2}
    },
        events = {
        {
            trigger = "on_tile",
            x = 3, y = 7,
            once = true,
            run = function()
                say("小偷：你清醒了吗？你到监狱时还处在昏迷中，魔法警卫把你扔到我这个房间。但你很幸运，我刚完成逃跑的暗道你就醒了，我们一起越狱吧。")
                say("小偷：你的剑和盾被警卫拿走了，你必须先找到武器。我知道铁剑在5楼，铁盾在9楼，你最好先取到它们。我现在有事要做没法帮你，再见。")
                set_tile(3, 7, 1)
                set_tile(2, 7, 1)
            end
        },
        {
            trigger = "on_guard_kill",
            guards = {{x = 6, y = 2}, {x = 8, y = 2}},
            once = true,
            run = function()
                replace_all(player_floor(), 8, 1)
            end
        },
        {
            trigger = "on_tile",
            tile = 154,
            run = function()
                if has_flag(2) then return end
                say("小偷：谢谢你救了我，我现在就帮你打通第35层暗道。")
                set_tile(11, 11, 1)
                set_flag(2)
                set_tile_floor(35, 5, 10, 154)
            end
        },
        {
            trigger = "on_tile",
            tile = 151,
            once = true,
            run = function()
                say("老人：谢谢你救了我，为感谢你的帮助，请收下这些礼物（1000金币）。")
                add_money(1000)
                set_tile(11, 4, 1)
            end
        },
        {
            trigger = "on_tile",
            tile = 152,
            run = function()
                if has_flag(4) then return end
                say("商人：谢谢你救了我，我可以用祝福魔法提升你3%的攻击力和防御力，现在就提升吗？")
                local c = choose_menu("好的", "下次再说")
                if c == 0 then
                    add_attack(math.floor(player_attack() * 0.03))
                    add_defence(math.floor(player_defence() * 0.03))
                    say("商人：祝福完成，你的力量增强了！")
                    set_tile(11, 7, 1)
                    set_flag(4)
                end
                drain()
            end
        }
    }
}
