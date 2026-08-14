return {
    map = {
        {

		2,2,2,2,2,2,2,2,2,2,2,2,2},
        {2,10,1,5,120,1,1,1,120,3,1,9,2},
        {2,1,1,2,1,152,51,1,1,2,1,1,2},
        {2,118,2,2,2,2,2,3,2,2,2,3,2},
        {2,1,1,119,119,1,2,1,2,51,2,1,2},
        {2,2,1,2,2,4,2,1,2,51,2,1,2},
        {2,1,1,1,4,4,2,118,2,51,2,118,2},
        {2,1,63,1,2,2,2,122,2,1,124,1,2},
        {2,1,1,1,2,57,54,1,2,2,2,3,2},
        {2,2,8,2,2,2,2,2,2,51,1,118,2},
        {2,121,1,121,2,1,1,1,2,1,124,1,2},
        {2,1,1,1,3,122,1,112,3,119,1,55,2},
        {2,2,2,2,2,2,2,2,2,2,2,2,2}
    },
    events = {
        {
            trigger = "on_tile",
            x = 2, y = 6,
            once = true,
            run = function()
                set("x", 2)
                set("y", 6)
                -- 设置隐形墙：看起来像空地，踩上会变为墙
                set_tile(2, 5, 140)
            end
        },
        {
            trigger = "on_tile",
            tile = 140,
            run = function()
                set_tile(2, 5, 2)
            end
        },
        {
            trigger = "on_guard_kill",
            guards = {{x = 1, y = 10}, {x = 3, y = 10}},
            run = function()
                set_tile(2, 9, 1)
                msg("守卫门已打开")
            end
        },
        {
            trigger = "on_tile",
            tile = 152,
            run = function()
                if has_flag(2) then return end
                if has_flag(1) then
                    say("商人：存放圣剑的房间的门坏了，你必须用镐破墙而入。")
                    note("商人", "存放圣剑的房间的门坏了，你必须用镐破墙而入。")
                    set_tile(5, 2, 1)
                    set_flag(2)
                    return
                end
                say("商人：我有3把黄钥匙，你出200个金币就都给你。")
                local c = choose_menu("我太需要了", "下次再说")
                if c == 0 then
                    if take_money(200) then
                        for i = 1, 3 do give(51) end
                        set_flag(1)
                    else
                        say("商人：你的金币不够！")
                    end
                end
                drain()
            end
        }
    }
}
