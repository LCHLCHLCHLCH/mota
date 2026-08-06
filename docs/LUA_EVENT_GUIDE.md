# Lua 事件编写指南

## 文件结构

每个楼层对应一个 Lua 文件：`map/floor_N.lua`（N 为楼层号，范围 0~50）。

文件返回一个 table，包含 `map` 和 `events` 两个字段：

```lua
return {
    map = {
        -- 13×13 地图数据
    },
    events = {
        -- 事件列表
    }
}
```

---

## 地图格式

`map` 是 13 行 × 13 列的二维数组（Lua 中为 table 的 table）。第 1 行对应游戏中的第 0 行（y=0），第 1 列对应第 0 列（x=0）。

数组中的每个数字为**地块 ID**，详细见文末 [地块 ID 速查表](#地块-id-速查表)。

```lua
map = {
    { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 },
    { 2, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2 },
    { 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2 },
    -- ... 共 13 行
    { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 },
}
```

每行必须恰好 13 个元素。

---

## 事件结构

每个事件是一个 table，通用字段如下：

| 字段 | 类型 | 必需 | 说明 |
|---|---|---|---|
| `trigger` | string | 是 | 触发类型：`"on_tile"`、`"on_guard_kill"`、`"on_clear"` |
| `once` | bool | 否 | **推荐**。设为 `true` 即自动一次性触发，无需手动管理 flag 编号 |
| `condition_flag` | int | 否 | 前置 flag ID（0~15）。仅当本层该 flag 未设置时触发。与 `once` 互斥 |
| `set_flag` | int | 否 | 事件执行后设置的本层 flag ID（0~15）。与 `once` 互斥 |
| `run` | function | 是 | 事件触发后执行的 Lua 函数 |

> **推荐**：直接使用 `once = true` 实现一次性触发，让系统自动管理 flag。
> 详见 [`docs/FLAGS.md`](FLAGS.md)。

事件的所有逻辑都写在 `run` 函数里，可直接调用全部注册的 Lua API（见下文 [Lua API](#lua-api)）：

```lua
{
    trigger = "on_tile",
    x = 5, y = 3,
    once = true,
    run = function()
        say("买把黄钥匙吧，10金币。")
    end
}
```

不同 trigger 类型有额外字段，见下。

---

## 触发器类型

### 1. `on_tile` — 踩踏地块触发

当玩家踩到指定位置时触发。支持两种匹配方式：

- **按坐标匹配**（推荐）：使用 `x` 和 `y` 字段，精确匹配某个位置
- **按 tile ID 匹配**（兼容旧版）：使用 `tile` 字段，匹配某种地块类型

两种方式**互斥**：如果同时指定了 `x`/`y` 和 `tile`，优先按坐标匹配。

额外字段：

| 字段 | 类型 | 说明 |
|---|---|---|
| `x` | int | 触发位置的列号（0~12）。与 `y` 配合使用 |
| `y` | int | 触发位置的行号（0~12）。与 `x` 配合使用 |
| `tile` | int | （如不使用 x/y）触发地块的 ID |

**按坐标匹配**（推荐）——同一楼层多个相同 NPC 可提供不同服务：

```lua
-- 坐标 (5,3) 的商人卖黄钥匙
{
    trigger = "on_tile",
    x = 5, y = 3,
    run = function()
        say("买把黄钥匙吧，10金币。")
    end
}

-- 坐标 (9,7) 的商人卖蓝钥匙
{
    trigger = "on_tile",
    x = 9, y = 7,
    run = function()
        say("蓝钥匙20金币一把。")
    end
}
```

**按 tile ID 匹配**（兼容旧版）：

```lua
{
    trigger = "on_tile",
    tile = 151,
    once = true,
    run = function()
        say("我可以给你一本怪物手册，你可以用它预测该楼层各怪物对你造成的伤害。")
        set_tile(11, 4, 1)
    end
}
```

---

### 2. `on_guard_kill` — 击败全部守卫后触发

当玩家击败指定的**全部**守卫后触发。游戏会检查每个守卫位置是否已变为空地（ID=1），全部清空时才执行函数。

额外字段：

| 字段 | 类型 | 说明 |
|---|---|---|
| `guards` | table | 守卫坐标列表，每个坐标 `{x = 列, y = 行}` |

示例——击败两个中级卫兵（121）后开门：

```lua
{
    trigger = "on_guard_kill",
    guards = {{x = 6, y = 2}, {x = 8, y = 2}},
    once = true,
    run = function()
        replace_all(player_floor(), 8, 1)   -- 守卫门 → 空地
    end
}
```

守卫门初始 ID 为 8，全部守卫被击败后 `replace_all` 将门变为空地（1）。

**注意**：
- 守卫必须是怪物（ID 101~150），玩家需通过战斗或炸药消灭它们
- 使用炸药消灭守卫也能正确触发此事件
- 每个事件只有一个守卫组；若楼层有多组守卫门，需创建多个事件各配不同的 `condition_flag` 或 `once`

---

### 3. `on_clear` — 楼层怪物清空后触发

当楼层中所有怪物被消灭后自动触发。没有额外字段（无 `tile`、`guards`）。

```lua
{
    trigger = "on_clear",
    once = true,
    run = function()
        say("你已消灭了本层所有怪物！")
        replace_all(player_floor(), 8, 1)
    end
}
```

**注意**：`on_clear` 在每次击杀怪物后都会检查。如果一个楼层有多个 `on_clear` 事件，只会执行第一个未标记 flag 的事件（执行后 `break`）。

---

## Flag 机制

Flag 是按楼层隔离的布尔数组（每层 16 个，ID 0~15）。用于：

- **防止事件重复触发**：使用 `once = true` 或 `condition_flag` / `set_flag`
- **跨事件状态**：同一楼层内多个事件共享 flag，可用于"先触发 A 后 B 才生效"这类顺序谜题

```lua
-- 此事件仅在 flag 3 未设置时触发，触发后设置 flag 3
{
    trigger = "on_tile",
    x = 3, y = 7,
    condition_flag = 3,   -- 仅当本层 flag 3 == 0 时触发
    set_flag = 3,         -- 触发后将本层 flag 3 设为 1
    run = function()
        say("第一次来到这里吧？")
    end
}
```

在 `run` 函数中也可直接操作 flag：

```lua
run = function()
    if has_flag(5) then
        say("flag 5 已设置")
    else
        set_flag(5)
    end
end
```

**注意**：`has_flag` / `set_flag` 操作的始终是**当前玩家所在楼层**的 flag 空间。`once` 与 `condition_flag` 同时存在时，`once` 优先。

---

## Lua API

`run` 函数内可直接调用以下注册的 Lua 函数：

### 对话与消息

| 函数 | 说明 |
|---|---|
| `say(text)` | 弹出模态对话框，按任意键关闭 |
| `msg(text)` | 在窗口底部消息栏显示一行文字 |
| `choose_menu(a, b, c, ...)` | 显示选择菜单，最多 8 项。返回 0~7 或 255（取消） |
| `drain()` | 清空输入缓冲区（弹出菜单后调用，避免残留按键） |

### 属性操作

| 函数 | 说明 |
|---|---|
| `add_health(n)` | 生命 +n |
| `add_attack(n)` | 攻击 +n |
| `add_defence(n)` | 防御 +n |
| `add_money(n)` | 金币 +n |
| `take_money(n)` | 扣除 n 金币。成功返回 `true`，不足返回 `false` |
| `set(attr, value)` | 直接设置属性，`attr` 为 `health`/`attack`/`defence`/`money`/`yellow`/`blue`/`red`/`floor`/`x`/`y` |

### 地图操作

| 函数 | 说明 |
|---|---|
| `set_tile(x, y, value)` | 设置当前楼层单格地块 |
| `replace_all(floor, from, to)` | 指定楼层全图替换（`from=8, to=1` 时自动提示"守卫门已打开"） |
| `get_tile(x, y)` | 读取当前楼层指定坐标的地块 ID |
| `count_monsters(floor)` | 返回指定楼层怪物数量 |
| `freeze_lava()` | 冰冻四周岩浆（V 键功能） |
| `detonate()` | 引爆四周炸药，返回击杀的怪物数 |

### 标记操作

| 函数 | 说明 |
|---|---|
| `has_flag(id)` | 查询当前楼层 flag 是否已设置，返回 boolean |
| `set_flag(id)` | 设置当前楼层 flag 为 1 |

### 道具

| 函数 | 说明 |
|---|---|
| `give(id)` | 给予指定道具（放入背包） |
| `add_yellow_key(n)` / `add_blue_key(n)` / `add_red_key(n)` | 增加钥匙 |
| `set_teleporter(v)` | 设置是否拥有楼层传送器 |
| `backpack_add(id)` | 向背包添加道具 |
| `backpack_has(id)` | 背包中是否持有某道具，返回 boolean |

### 演出与调试

| 函数 | 说明 |
|---|---|
| `player_floor()` | 返回玩家当前楼层号 |
| `player_x()` / `player_y()` / `player_dir()` | 玩家坐标与朝向 |
| `altar_times()` | 返回祭坛已使用次数 |
| `altar_tick()` | 祭坛使用次数 +1 |
| `sleep_ms(n)` | 暂停 n 毫秒（先刷新画面再等待） |
| `darken_map()` | 游戏地图变为黑屏 |
| `lighten_map()` | 取消黑屏，恢复地图显示 |
| `debug_on()` | 开启 debug 模式（无视碰撞/事件/怪物，底部显示坐标） |
| `debug_off()` | 关闭 debug 模式 |

---

## 完整示例

### 示例 1：NPC 对话（一次性，按坐标）

```lua
events = {
    {
        trigger = "on_tile",
        x = 3, y = 5,
        once = true,
        run = function()
            say("少年，前方的道路充满危险。")
            add_health(200)
            msg("获得老人的祝福，生命+200")
        end
    }
}
```

### 示例 2：守卫门

```lua
events = {
    {
        trigger = "on_guard_kill",
        guards = {{x = 1, y = 5}, {x = 3, y = 5}},
        once = true,
        run = function()
            replace_all(player_floor(), 8, 1)
        end
    }
}
```

### 示例 3：祭坛（选择分支）

```lua
events = {
    {
        trigger = "on_tile",
        tile = 155,
        run = function()
            local t = altar_times()
            local r = (player_floor() - 1) // 10 + 1
            local cost = 20 + 10 * (t + 1) * t
            local hp  = 100 * (t + 1)
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
```

### 示例 4：商人（坐标匹配 + 选择分支）

```lua
events = {
    {
        trigger = "on_tile",
        x = 5, y = 3,   -- 精确匹配 (5,3) 的商人
        run = function()
            local c = choose_menu("黄钥匙(10金)", "蓝钥匙(20金)", "不买")
            if c == 0 and take_money(10) then
                give(51)
            elseif c == 1 and take_money(20) then
                give(52)
            elseif c < 2 then
                say("金币不足！")
            end
            drain()
        end
    }
}
```

---

## 地块 ID 速查表

### 地形（1~11）

| ID | 符号 | 名称 | 说明 |
|---|---|---|---|
| 1 | ` ` | 空地 | 可通行 |
| 2 | ` ` | 墙壁 | 不可通行 |
| 3 | 〓 | 黄门 | 消耗黄钥匙打开 |
| 4 | 〓 | 蓝门 | 消耗蓝钥匙打开 |
| 5 | 〓 | 红门 | 消耗红钥匙打开 |
| 6 | ` ` | 岩浆 | 不可通行（可用冰霜魔法冻结） |
| 7 | ★ | 星星 | 装饰性地块 |
| 8 | 〓 | 守卫门 | 击败守卫后自动打开 |
| 9 | △ | 上行楼梯 | 通往上一层 |
| 10 | ▽ | 下行楼梯 | 通往下一层 |
| 11 | ` ` | 可破墙 | 踩踏后消失 |

### 道具（51~70）

| ID | 符号 | 名称 |
|---|---|---|
| 51 | 钥 | 黄钥匙 |
| 52 | 钥 | 蓝钥匙 |
| 53 | 钥 | 红钥匙 |
| 54 | ★ | 红血瓶 |
| 55 | ★ | 蓝血瓶 |
| 56 | ◆ | 红宝石 |
| 57 | ◆ | 蓝宝石 |
| 58 | 剑 | 铁剑 |
| 59 | 盾 | 铁盾 |
| 60 | 剑 | 银剑 |
| 61 | 盾 | 银盾 |
| 62 | 剑 | 骑士剑 |
| 63 | 盾 | 骑士盾 |
| 64 | 剑 | 圣剑 |
| 65 | 盾 | 圣盾 |
| 66 | 剑 | 神圣剑 |
| 67 | 盾 | 神圣盾 |
| 68 | 杖 | 楼层传送器 |
| 69 | 冰 | 冰霜魔法 |
| 70 | 炸 | 炸药 |

### 怪物（101~135）

| ID | 符号 | 名称 |
|---|---|---|
| 101 | ⊙ | 绿史莱姆 |
| 102 | ⊙ | 红史莱姆 |
| 103 | 蝠 | 小蝙蝠 |
| 104 | 法 | 初级法师 |
| 105 | 骷 | 骷髅 |
| 106 | 骷 | 骷髅士兵 |
| 107 | 卫 | 初级卫兵 |
| 108 | 骷 | 骷髅队长 |
| 109 | ⊙ | 大史莱姆 |
| 110 | 蝠 | 大蝙蝠 |
| 111 | 法 | 高级法师 |
| 112 | 兽 | 兽人 |
| 113 | 兽 | 兽人武士 |
| 114 | 石 | 石头人 |
| 115 | 章 | 巨型章鱼 |
| 116 | 血 | 吸血鬼 |
| 117 | 师 | 大法师 |
| 118 | 鬼 | 鬼战士 |
| 119 | 战 | 战士 |
| 120 | 幽 | 幽灵 |
| 121 | 卫 | 中级卫兵 |
| 122 | 武 | 双手剑士 |
| 123 | 龙 | 魔龙 |
| 124 | 骑 | 骑士 |
| 125 | 骑 | 骑士队长 |
| 126 | 巫 | 初级巫师 |
| 127 | 巫 | 高级巫师 |
| 128 | Θ | 史莱姆王 |
| 129 | 蝠 | 吸血蝙蝠 |
| 130 | 骑 | 黑暗骑士 |
| 131 | 警 | 魔法警卫 |
| 132 | 卫 | 高级卫兵 |
| 133 | 王 | 假魔王 |
| 134 | 王 | 假魔王(封) |
| 135 | 王 | 真魔王 |

### NPC（151~155）

| ID | 符号 | 名称 |
|---|---|---|
| 151 | 老 | 老人 |
| 152 | 商 | 商人 |
| 153 | 公 | 公主 |
| 154 | 偷 | 小偷 |
| 155 | 祭 | 祭坛 |

### 其他

| ID | 说明 |
|---|---|
| 255 | 勇者（玩家）——不要在地图数据中使用 |

---

## 注意事项

1. **执行优先级**：玩家移动时，引擎**先查 Lua 事件**，若该坐标有匹配的 `on_tile` 事件则执行其 `run` 函数（跳过 C++ 默认行为）；若没有则走 C++ 内置逻辑（开门/捡道具/战斗/移动）。因此 Lua 事件可以完全覆盖任何地块类型的默认行为。同一 trigger 类型的多个事件，只会执行**第一个**满足条件的（执行后 `break`）。

2. **Flag 范围**：ID 范围 0~15（按楼层隔离）。不要超过 15。`once = true` 的自动 flag 由事件索引决定，每层最多 16 个事件。

3. **坐标系统**：x 是列号（0~12），y 是行号（0~12）。地图数组中第 1 行是 y=0，第 1 列是 x=0。使用坐标匹配时，不同位置的相同 NPC 可以拥有完全不同的事件逻辑，解决了"一楼层多商人"的问题。

4. **守卫门初始值**：守卫门必须在地图中写为 ID=8，否则 `replace_all` 的 `from=8` 匹配不到。

5. **run 中的错误**：`run` 函数内的 Lua 错误会输出到 stderr，游戏不会崩溃。编写复杂逻辑时建议先在控制台 REPL 中测试。

6. **文件编码**：地图文件使用 UTF-8 编码保存。
