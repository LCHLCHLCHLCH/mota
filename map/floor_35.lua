-- 隐性事件：玩家到达35层后决定小偷的命运
-- 若2层两个中级守卫已被击败，小偷未被抓，直接出现在35层；否则被抓回2层
local function decide_thief_fate()
    if has_flag(4) then return end
    set_flag(4)
    if get_tile_floor(2, 6, 2) == 1 and get_tile_floor(2, 8, 2) == 1 then
        set_tile_floor(35, 5, 10, 154)
    else
        set_tile_floor(2, 11, 11, 154)
    end
end

return {
    map = {
        {

		2,2,2,2,2,2,2,2,2,2,2,2,2},
        {2,1,1,1,1,1,1,1,1,1,1,9,2},
        {2,1,2,2,2,2,8,2,2,2,2,2,2},
        {2,1,2,1,2,55,55,55,2,2,2,2,2},
        {2,1,2,1,2,1,69,1,2,2,2,2,2},
        {2,1,2,1,2,156,156,156,2,2,2,2,2},
        {2,1,2,1,2,156,156,156,2,2,2,2,2},
        {2,1,2,1,2,156,156,156,2,2,2,2,2},
        {2,1,2,1,2,1,1,1,2,2,2,2,2},
        {2,1,2,1,2,1,1,1,2,2,2,2,2},
        {2,1,2,1,2,1,1,1,2,2,2,2,2},
        {2,1,1,1,2,1,10,1,2,2,2,2,2},
        {2,2,2,2,2,2,2,2,2,2,2,2,2}
    },
    events = {
        {
            trigger = "on_tile",
            tile = 156,
            run = function(x, y)
                if not battle_monster(123) then
                    msg("你还不能击败它！")
                    return
                end
                -- 魔龙身体消散
                for yy = 5, 7 do
                    for xx = 5, 7 do
                        set_tile(xx, yy, 1)
                    end
                end
                set("x", x)
                set("y", y)
                set_tile(6, 2, 1)
                msg("魔龙倒下，身体消散，守卫门打开了！")
            end
        },
        {
            trigger = "on_tile",
            x = 6, y = 10,
            once = true,
            run = function()
                decide_thief_fate()
            end
        },
        {
            trigger = "on_tile",
            x = 5, y = 11,
            once = true,
            run = function()
                decide_thief_fate()
            end
        },
        {
            trigger = "on_tile",
            tile = 154,
            once = true,
            run = function()
                say("小偷：暗道挖好了。这座魔塔太危险了，我现在要离开了。后会有期。")
                set_tile(5, 10, 1)
                set_tile(4, 10, 1)
            end
        }
    }
}
