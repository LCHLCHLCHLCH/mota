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
    on_acquire = function()
        add_yellow_key(1)
        msg("获得黄钥匙")
    end,
})

M.register(52, {
    name = "蓝钥匙",
    on_acquire = function()
        add_blue_key(1)
        msg("获得蓝钥匙")
    end,
})

M.register(53, {
    name = "红钥匙",
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
    on_acquire = function()
        local bonus = ((player_floor() - 1) // 10 + 1) * 50
        add_health(bonus)
        msg("获得红血瓶，生命增加" .. bonus)
    end,
})

M.register(55, {
    name = "蓝血瓶",
    on_acquire = function()
        local bonus = ((player_floor() - 1) // 10 + 1) * 200
        add_health(bonus)
        msg("获得蓝血瓶，生命增加" .. bonus)
    end,
})

M.register(56, {
    name = "红宝石",
    on_acquire = function()
        local bonus = ((player_floor() - 1) // 10 + 1)
        add_attack(bonus)
        msg("获得红宝石，攻击增加" .. bonus)
    end,
})

M.register(57, {
    name = "蓝宝石",
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
    on_acquire = function() add_attack(10);  msg("获得铁剑，攻击增加10") end,
})
M.register(59, {
    name = "铁盾",
    on_acquire = function() add_defence(10); msg("获得铁盾，防御增加10") end,
})
M.register(60, {
    name = "银剑",
    on_acquire = function() add_attack(20);  msg("获得银剑，攻击增加20") end,
})
M.register(61, {
    name = "银盾",
    on_acquire = function() add_defence(20); msg("获得银盾，防御增加20") end,
})
M.register(62, {
    name = "骑士剑",
    on_acquire = function() add_attack(40);  msg("获得骑士剑，攻击增加40") end,
})
M.register(63, {
    name = "骑士盾",
    on_acquire = function() add_defence(40); msg("获得骑士盾，防御增加40") end,
})
M.register(64, {
    name = "圣剑",
    on_acquire = function() add_attack(50);  msg("获得圣剑，攻击增加50") end,
})
M.register(65, {
    name = "圣盾",
    on_acquire = function() add_defence(50); msg("获得圣盾，防御增加50") end,
})
M.register(66, {
    name = "神圣剑",
    on_acquire = function() add_attack(100); msg("获得神圣剑，攻击增加100") end,
})
M.register(67, {
    name = "神圣盾",
    on_acquire = function() add_defence(100); msg("获得神圣盾，防御增加100") end,
})

-- ============================================================
-- 特殊道具
-- ============================================================
M.register(68, {
    name = "楼层传送器",
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
-- 镐子（效果函数预留）
-- ============================================================
M.register(71, {
    name = "镐子",
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
-- 拾取映射：地面 tile_id → 道具 id
-- 所有 51-71 的地面道具都映射到同号道具
-- ============================================================
for i = 51, 70 do
    M.map_pickup(i, i)
end
M.map_pickup(71, 71)

return M
