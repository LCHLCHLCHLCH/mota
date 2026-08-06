return {
    map = {
        {

		2,2,2,2,2,2,2,2,2,2,2,2,2},
        {2,57,113,1,3,1,9,1,2,154,1,1,2},
        {2,112,1,1,2,1,1,1,2,1,1,109,2},
        {2,1,1,109,2,2,8,2,2,2,2,3,2},
        {2,3,2,2,2,1,1,1,2,110,1,1,2},
        {2,1,2,51,2,156,156,156,2,1,110,1,2},
        {2,1,2,52,2,156,156,156,2,3,2,111,2},
        {2,109,2,51,2,156,156,156,2,1,2,1,2},
        {2,1,2,1,2,1,1,1,2,1,2,55,2},
        {2,1,3,1,2,2,1,2,2,3,2,2,2},
        {2,110,2,110,2,1,1,1,2,1,103,1,2},
        {2,1,111,1,2,1,10,1,3,103,1,152,2},
        {2,2,2,2,2,2,2,2,2,2,2,2,2}
    },
    events = {
        {
            trigger = "on_tile",
            tile = 156,
            run = function(x, y)
                if not battle_monster(115) then
                    msg("你还不能击败它！")
                    return
                end
                -- 章鱼身体消散
                for yy = 5, 7 do
                    for xx = 5, 7 do
                        set_tile(xx, yy, 1)
                    end
                end
                set("x", x)
                set("y", y)
                set_tile(6, 3, 1)
                set_tile(6, 5, 71)
                msg("章鱼倒下，身体消散，一把镐子出现在原地！")
            end
        },
        {
            trigger = "on_tile",
            tile = 152,
            run = function()
                if has_flag(2) then return end
                if has_flag(1) then
                    say("商人：如果你持有十字架，面对兽人和吸血鬼时你的攻击力加倍。在没有十字架的情况下，你不可能打败吸血鬼。十字架被藏在高于15楼的墙内。")
                    set_tile(11, 11, 1)
                    set_flag(2)
                    return
                end
                say("商人：我有一把蓝钥匙，你出200个金币就卖给你。")
                local c = choose_menu("我太需要了", "下次再说")
                if c == 0 then
                    if take_money(200) then
                        give(52)
                        set_flag(1)
                    else
                        say("商人：你的金币不够！")
                    end
                end
                drain()
            end
        },
        {
            trigger = "on_tile",
            tile = 154,
            condition_flag = 3,
            set_flag = 3,
            run = function()
                say("啊哈！你还好吗？这大章鱼挡住了我前进的道路，现在暗道终于完工了，你现在最好也躲开它。我要走了，再见。")
                set_tile(8, 1, 1)
                set_tile(9, 1, 1)
            end
        }
    }
}
