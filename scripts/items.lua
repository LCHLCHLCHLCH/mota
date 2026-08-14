-- ============================================================
-- 道具定义表 — 所有道具效果统一在此定义
-- ============================================================
-- 分离原则：
--   地图瓦片 (g_tile_defs)   → 只管"地上长什么样"（symbol, color）
--   道具定义 (items)         → 只管"这是什么、能干什么"
--   拾取映射 (pickup_map)     → tile_id → item_id 的桥梁
--
-- 添加新道具：在此文件加一个 register() 即可。
-- 若该道具会出现在地图上，再额外在 tile_data.cpp 的 g_tile_defs
-- 中加一行外观定义，并在 pickup_map 里建立 tile→item 映射。
-- 纯任务/对话道具只需 register()，不需要 tile 条目。
-- ============================================================

local M = {}

M.items = {}
M.pickup_map = {}

-- 注册一个道具
function M.register(id, def)
    M.items[id] = def
end

-- 建立 tile_id → item_id 拾取映射
function M.map_pickup(tile_id, item_id)
    M.pickup_map[tile_id] = item_id
end

-- ============================================================
-- 钥匙类（立即消耗，不入背包）
-- ============================================================
M.register(51, {
    name = "黄钥匙",
    desc = "打开黄色门的钥匙。",
    on_acquire = function()
        add_yellow_key(1)
        msg("获得黄钥匙")
    end,
})

M.register(52, {
    name = "蓝钥匙",
    desc = "打开蓝色门的钥匙。",
    on_acquire = function()
        add_blue_key(1)
        msg("获得蓝钥匙")
    end,
})

M.register(53, {
    name = "红钥匙",
    desc = "打开红色门的钥匙。",
    on_acquire = function()
        add_red_key(1)
        msg("获得红钥匙")
    end,
})

-- ============================================================
-- 血瓶 / 宝石（随楼层成长）
-- ============================================================
M.register(54, {
    name = "红血瓶",
    desc = "恢复生命值，楼层越高效果越强。",
    on_acquire = function()
        local bonus = ((player_floor() - 1) // 10 + 1) * 50
        add_health(bonus)
        msg("获得红血瓶，生命增加" .. bonus)
    end,
})

M.register(55, {
    name = "蓝血瓶",
    desc = "大量恢复生命值，楼层越高效果越强。",
    on_acquire = function()
        local bonus = ((player_floor() - 1) // 10 + 1) * 200
        add_health(bonus)
        msg("获得蓝血瓶，生命增加" .. bonus)
    end,
})

M.register(56, {
    name = "红宝石",
    desc = "永久提升攻击力。",
    on_acquire = function()
        local bonus = ((player_floor() - 1) // 10 + 1)
        add_attack(bonus)
        msg("获得红宝石，攻击增加" .. bonus)
    end,
})

M.register(57, {
    name = "蓝宝石",
    desc = "永久提升防御力。",
    on_acquire = function()
        local bonus = ((player_floor() - 1) // 10 + 1)
        add_defence(bonus)
        msg("获得蓝宝石，防御增加" .. bonus)
    end,
})

-- ============================================================
-- 装备（固定数值）
-- ============================================================
M.register(58, {
    name = "铁剑",
    desc = "攻击力 +10。",
    on_acquire = function() add_attack(10);  msg("获得铁剑，攻击增加10") end,
})
M.register(59, {
    name = "铁盾",
    desc = "防御力 +10。",
    on_acquire = function() add_defence(10); msg("获得铁盾，防御增加10") end,
})
M.register(60, {
    name = "银剑",
    desc = "攻击力 +20。",
    on_acquire = function() add_attack(20);  msg("获得银剑，攻击增加20") end,
})
M.register(61, {
    name = "银盾",
    desc = "防御力 +20。",
    on_acquire = function() add_defence(20); msg("获得银盾，防御增加20") end,
})
M.register(62, {
    name = "骑士剑",
    desc = "攻击力 +40。",
    on_acquire = function() add_attack(40);  msg("获得骑士剑，攻击增加40") end,
})
M.register(63, {
    name = "骑士盾",
    desc = "防御力 +40。",
    on_acquire = function() add_defence(40); msg("获得骑士盾，防御增加40") end,
})
M.register(64, {
    name = "圣剑",
    desc = "攻击力 +50。",
    on_acquire = function() add_attack(50);  msg("获得圣剑，攻击增加50") end,
})
M.register(65, {
    name = "圣盾",
    desc = "防御力 +50。",
    on_acquire = function() add_defence(50); msg("获得圣盾，防御增加50") end,
})
M.register(66, {
    name = "神圣剑",
    desc = "攻击力 +100。",
    on_acquire = function() add_attack(100); msg("获得神圣剑，攻击增加100") end,
})
M.register(67, {
    name = "神圣盾",
    desc = "防御力 +100，免疫魔法伤害。",
    on_acquire = function()
        set_holy_shield(true)
        add_defence(100)
        msg("获得神圣盾，防御增加100，免疫魔法伤害")
    end,
})

-- ============================================================
-- 特殊道具
-- ============================================================
M.register(68, {
    name = "楼层传送器",
    desc = "在已到达的楼层之间上下传送。",
    on_acquire = function()
        set_teleporter(true)
        msg("获得楼层传送器")
    end,
})

-- ============================================================
-- 背包道具（可携带、可主动使用）
-- ============================================================
M.register(69, {
    name = "冰霜魔法",
    desc = "冰冻岩浆。",
    on_acquire = function()
        if not backpack_has(69) then
            backpack_add(69)
        end
        msg("获得冰霜魔法")
    end,
    on_use = function()
        freeze_lava()
    end,
})

M.register(70, {
    name = "炸药",
    desc = "炸毁相邻格子的怪物，对首领无效。",
    on_acquire = function()
        backpack_add(70)
        msg("获得炸药")
    end,
    on_use = function()
        local killed = detonate()
        if killed > 0 then
            msg("炸药炸死了" .. killed .. "个怪物")
        else
            msg("炸药没有效果")
        end
    end,
})

-- ============================================================
-- 镐子
-- ============================================================
M.register(71, {
    name = "镐子",
    desc = "破坏一堵墙。",
    on_acquire = function()
        backpack_add(71)
        msg("获得镐子")
    end,
    on_use = function()
        local dir = player_dir()
        local dx, dy = 0, 0
        if dir == 0 then dy = -1
        elseif dir == 1 then dy = 1
        elseif dir == 2 then dx = -1
        elseif dir == 3 then dx = 1
        end
        local tx = player_x() + dx
        local ty = player_y() + dy
        -- 边界检查：最外圈不可破坏
        if tx > 0 and tx < 12 and ty > 0 and ty < 12 and get_tile(tx, ty) == 2 then
            set_tile(tx, ty, 1)
            msg("镐子破坏了墙壁！")
            return true
        else
            msg("没有可破坏的墙")
            return false
        end
    end,
})

-- ============================================================
-- 怪物手册（获取后开放地图怪物红黄绿威胁指示灯）
-- ============================================================
M.register(72, {
    name = "怪物手册",
    desc = "查看本层怪物属性，开启威胁指示灯。",
    on_acquire = function()
        set_monster_book(true)
        backpack_add(72)
        msg("获得怪物手册")
    end,
    on_use = function()
        show_monster_book()
        return false
    end,
})

-- ============================================================
-- 十字架（被动道具：入背包，使用无效果且不消耗）
-- 持有十字架时，对兽人/兽人武士/吸血鬼的攻击力加倍
-- ============================================================
M.register(73, {
    name = "十字架(被动)",
    desc = "对兽人、兽人武士、吸血鬼的攻击翻倍。",
    on_acquire = function()
        set_cross(true)
        backpack_add(73)
        msg("获得十字架")
    end,
    on_use = function()
        return false
    end,
})

-- ============================================================
-- 中心对称飞行器（可使用 3 次）：传送到当前位置的对称位置
-- 对称位置需为空地(1)；传送不触发领域/夹击伤害
-- ============================================================
M.register(74, {
    name = "中心对称飞行器",
    desc = "传送到地图中心对称位置（可使用 3 次）。",
    on_acquire = function()
        backpack_add(74, 3)
        msg("获得中心对称飞行器（可传送3次）")
    end,
    on_use = function()
        local tx = 12 - player_x()
        local ty = 12 - player_y()
        if get_tile(tx, ty) == 1 then
            set("x", tx)
            set("y", ty)
            msg("传送到了对称位置")
            return true
        else
            msg("对称位置不是空地，无法传送")
            return false
        end
    end,
})

-- ============================================================
-- 下楼飞行器（单次）：传送到下一层楼相同位置
-- 对应位置需为空地(1)，否则无法使用且不消耗
-- ============================================================
M.register(75, {
    name = "下楼飞行器",
    desc = "传送到下一层相同位置。",
    on_acquire = function()
        backpack_add(75)
        msg("获得下楼飞行器")
    end,
    on_use = function()
        local f = player_floor() - 1
        if f < 0 then
            msg("你已在最底层，无法使用下楼飞行器")
            return false
        end
        local tx = player_x()
        local ty = player_y()
        if get_tile_floor(f, tx, ty) == 1 then
            set("floor", f)
            msg("下楼飞行器带你到了下一层")
            return true
        else
            msg("下一层对应位置不是空地，无法使用")
            return false
        end
    end,
})

-- ============================================================
-- 上楼飞行器（单次）：传送到上一层楼相同位置
-- 对应位置需为空地(1)，否则无法使用且不消耗
-- ============================================================
M.register(76, {
    name = "上楼飞行器",
    desc = "传送到上一层相同位置。",
    on_acquire = function()
        backpack_add(76)
        msg("获得上楼飞行器")
    end,
    on_use = function()
        local f = player_floor() + 1
        if f > 50 then
            msg("你已在最高层，无法使用上楼飞行器")
            return false
        end
        if f == 50 then
            msg("无法通过上楼飞行器到达50层")
            return false
        end
        local tx = player_x()
        local ty = player_y()
        if get_tile_floor(f, tx, ty) == 1 then
            set("floor", f)
            msg("上楼飞行器带你到了上一层")
            return true
        else
            msg("上一层对应位置不是空地，无法使用")
            return false
        end
    end,
})

-- ============================================================
-- 魔法钥匙（一次性消耗）：打开当前楼层所有黄门
-- ============================================================
M.register(77, {
    name = "魔法钥匙",
    desc = "打开本层所有黄门。",
    on_acquire = function()
        backpack_add(77)
        msg("获得魔法钥匙")
    end,
    on_use = function()
        local opened = 0
        for y = 0, 12 do
            for x = 0, 12 do
                if get_tile(x, y) == 3 then
                    set_tile(x, y, 1)
                    opened = opened + 1
                end
            end
        end
        if opened > 0 then
            msg("魔法钥匙打开了" .. opened .. "扇黄门")
        else
            msg("本层没有黄门")
        end
        return true
    end,
})

-- ============================================================
-- 地震卷轴（一次性）：摧毁当前楼层所有墙（最外圈除外）
-- ============================================================
M.register(78, {
    name = "地震卷轴",
    desc = "摧毁本层所有墙。",
    on_acquire = function()
        backpack_add(78)
        msg("获得地震卷轴")
    end,
    on_use = function()
        local replaced = 0
        for y = 0, 12 do
            for x = 0, 12 do
                if x > 0 and x < 12 and y > 0 and y < 12 then
                    if get_tile(x, y) == 2 then
                        set_tile(x, y, 1)
                        replaced = replaced + 1
                    end
                end
            end
        end
        if replaced > 0 then
            msg("地震卷轴摧毁了" .. replaced .. "面墙")
        else
            msg("本层没有可摧毁的墙")
        end
        return true
    end,
})

-- ============================================================
-- 幸运金币（被动道具：入背包，使用无效果且不消耗）
-- 持有幸运金币时，击败敌人获得 2 倍金币
-- ============================================================
M.register(79, {
    name = "幸运金币(被动)",
    desc = "击败敌人获得的金币翻倍。",
    on_acquire = function()
        set_lucky_coin(true)
        backpack_add(79)
        msg("获得幸运金币，打怪金币翻倍")
    end,
    on_use = function()
        return false
    end,
})

-- ============================================================
-- 圣水（一次性）：按（攻击+防御）增加生命
-- ============================================================
M.register(80, {
    name = "圣水",
    desc = "按攻击力与防御力之和恢复生命。",
    on_acquire = function()
        backpack_add(80)
        msg("获得圣水")
    end,
    on_use = function()
        local gain = player_attack() + player_defence()
        add_health(gain)
        msg("圣水使生命增加了" .. gain)
        return true
    end,
})

-- ============================================================
-- 屠龙匕首（被动道具：入背包，使用无效果且不消耗）
-- 持有屠龙匕首时，攻击魔龙(123) 攻击力翻倍
-- ============================================================
M.register(81, {
    name = "屠龙匕首(被动)",
    desc = "攻击魔龙时伤害翻倍。",
    on_acquire = function()
        set_dragon_slayer(true)
        backpack_add(81)
        msg("获得屠龙匕首，攻击魔龙伤害翻倍")
    end,
    on_use = function()
        return false
    end,
})

-- ============================================================
-- 记事本：查看记录过的所有有用对话（老人/商人）
-- ============================================================
M.register(82, {
    name = "记事本",
    desc = "记录与老人、商人的重要对话。",
    on_acquire = function()
        backpack_add(82)
        msg("获得记事本")
    end,
    on_use = function()
        show_notebook()
        return false
    end,
})

-- ============================================================
-- 拾取映射：地面 tile_id → 道具 id
-- 所有 51-71 的地面道具都映射到同号道具
-- ============================================================
for i = 51, 70 do
    M.map_pickup(i, i)
end
M.map_pickup(71, 71)
M.map_pickup(73, 73)
M.map_pickup(74, 74)
M.map_pickup(75, 75)
M.map_pickup(76, 76)
M.map_pickup(77, 77)
M.map_pickup(78, 78)
M.map_pickup(79, 79)
M.map_pickup(80, 80)
M.map_pickup(81, 81)
M.map_pickup(82, 82)

return M
