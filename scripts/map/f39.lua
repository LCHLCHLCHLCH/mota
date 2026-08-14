return {
    map = {
        {

		2,2,2,2,2,2,2,2,2,2,2,2,2},
        {2,1,1,1,1,1,1,1,2,1,1,10,2},
        {2,1,3,1,3,1,3,1,2,152,1,1,2},
        {2,1,1,1,1,1,1,1,2,1,1,51,2},
        {2,1,3,1,3,1,3,1,2,2,3,2,2},
        {2,1,1,1,1,1,1,1,2,1,1,119,2},
        {2,1,3,1,3,1,3,1,2,120,2,56,2},
        {2,1,1,1,1,1,1,1,2,1,1,118,2},
        {2,2,4,2,2,2,2,2,2,2,3,2,2},
        {2,1,1,118,2,122,57,124,2,1,124,1,2},
        {2,2,118,1,3,1,2,1,3,1,1,1,2},
        {2,151,1,51,2,1,120,1,2,54,1,9,2},
        {2,2,2,2,2,2,2,2,2,2,2,2,2}
    },
    on_step = function()
        -- 黄门谜题：当且仅当 (4,2) 与 (6,4) 两扇门被开启、且其余 7 扇门都未被开启时，
        -- 中心对称飞行器出现在 (4,4)。任何一扇门开错都会导致飞行器丢失。
        if get_tile(4, 2) == 1 and get_tile(6, 4) == 1 then
            if get_tile(2, 2) == 3 and get_tile(6, 2) == 3 and
               get_tile(2, 4) == 3 and get_tile(4, 4) == 3 and
               get_tile(2, 6) == 3 and get_tile(4, 6) == 3 and
               get_tile(6, 6) == 3 then
                set_tile(4, 4, 74)
                msg("黄门中出现了中心对称飞行器！")
            end
        end
    end,
    events = {
        {
            trigger = "on_tile",
            tile = 151,
            once = true,
            run = function()
                say("老人：在三点，拥有传送功能的秘宝就会出现")
                note("老人", "在三点，拥有传送功能的秘宝就会出现")
                set_tile(1, 11, 1)
            end
        },
        {
            trigger = "on_tile",
            tile = 152,
            run = function()
                if has_flag(2) then return end
                if has_flag(1) then
                    say("商人：塔内有个幸运金币。拥有它在打败敌人后能获得2倍的金钱。")
                    note("商人", "塔内有个幸运金币。拥有它在打败敌人后能获得2倍的金钱。")
                    set_tile(9, 2, 1)
                    set_flag(2)
                    return
                end
                say("商人：我有三把蓝钥匙，你出2000个金币就都给你。")
                local c = choose_menu("我太需要了", "下次再说")
                if c == 0 then
                    if take_money(2000) then
                        for i = 1, 3 do give(52) end
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
