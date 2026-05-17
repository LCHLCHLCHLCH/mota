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

## 事件类型

### 事件结构

每个事件是一个 table，通用字段如下：

| 字段 | 类型 | 必需 | 说明 |
|---|---|---|---|
| `trigger` | string | 是 | 触发类型：`"on_tile"`、`"on_guard_kill"`、`"on_clear"` |
| `once` | bool | 否 | **推荐**。设为 `true` 即自动一次性触发，无需手动管理 flag 编号 |
| `condition_flag` | int | 否 | 前置 flag ID（0~15）。仅当本层该 flag 未设置时触发。与 `once` 互斥 |
| `set_flag` | int | 否 | 事件执行后设置的本层 flag ID（0~15）。与 `once` 互斥 |
| `actions` | table | 是 | 动作列表，按顺序执行 |

> **推荐**：直接使用 `once = true` 实现一次性触发，让系统自动管理 flag。
> 详见 [`docs/FLAGS.md`](FLAGS.md)。

不同 trigger 类型有额外字段，见下。

---

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
    actions = {
        { type = "say", text = "买把黄钥匙吧，10金币。" },
    }
}

-- 坐标 (9,7) 的商人卖蓝钥匙
{
    trigger = "on_tile",
    x = 9, y = 7,
    actions = {
        { type = "say", text = "蓝钥匙20金币一把。" },
    }
}
```

**按 tile ID 匹配**（兼容旧版）：

```lua
{
    trigger = "on_tile",
    tile = 151,
    condition_flag = 1,
    set_flag = 1,
    actions = {
        { type = "say", text = "我可以给你一本怪物手册，你可以用它预测该楼层各怪物对你造成的伤害。" },
    }
}
```

---

### 2. `on_guard_kill` — 击败全部守卫后触发

当玩家击败指定的**全部**守卫后触发。游戏会检查每个守卫位置是否已变为空地（ID=1），全部清空时才执行动作。

额外字段：

| 字段 | 类型 | 说明 |
|---|---|---|
| `guards` | table | 守卫坐标列表，每个坐标 `{x = 列, y = 行}` |

示例——击败两个中级卫兵（121）后开门：

```lua
{
    trigger = "on_guard_kill",
    guards = {{x = 6, y = 2}, {x = 8, y = 2}},
    condition_flag = 2,
    set_flag = 2,
    actions = {
        { type = "replace_all", from = 8, to = 1 },
    }
}
```

守卫门初始 ID 为 8，击败全部守卫后 `replace_all` 将门变为空地（1）。

**注意**：
- 守卫必须是怪物（ID 101~150），玩家需通过战斗或炸药消灭它们
- 使用炸药消灭守卫也能正确触发此事件
- 每个事件只有一个守卫组；若楼层有多组守卫门，需创建多个事件各配不同的 `condition_flag`

---

### 3. `on_clear` — 楼层怪物清空后触发

当楼层中所有怪物被消灭后自动触发。没有额外字段（无 `tile`、`guards`）。

```lua
{
    trigger = "on_clear",
    condition_flag = 10,
    set_flag = 10,
    actions = {
        { type = "say", text = "你已消灭了本层所有怪物！" },
        { type = "replace_all", from = 8, to = 1 },
    }
}
```

**注意**：`on_clear` 在每次击杀怪物后都会检查。如果一个楼层有多个 `on_clear` 事件，只会执行第一个未标记 flag 的事件（执行后 `break`）。

---

## 动作类型

### `say` — 弹出对话框

```lua
{ type = "say", text = "显示的对话文字" }
```

弹出模态对话框，用户按任意键关闭。支持中文。

---

### `msg` — 底部消息栏

```lua
{ type = "msg", text = "守卫门已打开" }
```

在窗口底部消息栏显示一行文字提示。

---

### `replace_all` — 全图批量替换地块

```lua
{ type = "replace_all", from = 8, to = 1 }
```

将楼层中所有值为 `from` 的地块替换为 `to`。常用于：
- 守卫门打开：`from = 8, to = 1`
- 场景变化：将某种地块批量替换为另一种

内置提示：当 `from=8, to=1` 时自动显示"守卫门已打开"。

---

### `set_tile` — 设置单个地块

```lua
{ type = "set_tile", x = 5, y = 3, value = 1 }
```

将指定坐标的地块设为指定值。用于精确修改单个地块。

---

### `add_health` / `add_attack` / `add_defence` / `add_money` — 增减属性

```lua
{ type = "add_health",  value = 200 }   -- 生命 +200
{ type = "add_attack",  value = 10 }    -- 攻击 +10
{ type = "add_defence", value = 5 }     -- 防御 +5
{ type = "add_money",   value = 50 }    -- 金币 +50
```

正值增加，负值减少。

---

### `take_money` — 扣除金币

```lua
{ type = "take_money", value = 100 }
```

金币不足时不会扣除（需要配合 `choose` 的 `if_choice` 进行分支处理，见下文）。

---

### `choose` — 显示选择菜单

```lua
{ type = "choose", choices = { "生命+100", "攻击+2", "防御+4", "离开" } }
```

显示选项列表，玩家用上下方向键选择、Z 键确认、X 键取消。返回值：0=第一项、1=第二项、……、255=取消。

**必须配合 `if_choice` 使用**（见下文），以便不同选项执行不同后续动作。

---

### `call` — 执行 Lua 函数

```lua
{ type = "call", func = function()
    say("你好！")
    add_health(100)
end }
```

执行任意 Lua 代码。函数内部可使用所有注册的 Lua API（见下文）。这是最灵活的动作类型，可实现任意复杂逻辑。

---

## `if_choice` — 基于选择的分支

当一个 `choose` 动作之后紧跟带 `if_choice` 的动作时，这些动作只在用户选择了指定选项时才执行。

```lua
actions = {
    -- 第一步：弹出选择
    { type = "choose", choices = { "生命+100", "攻击+2", "离开" } },

    -- 选"生命+100"时才执行
    { type = "add_health", value = 100, if_choice = 0 },

    -- 选"攻击+2"时才执行
    { type = "add_attack", value = 2, if_choice = 1 },

    -- 选"离开"时才执行（可选：什么也不做）
}
```

`if_choice` 可以是单个数字或数字数组：

```lua
-- 选 0 或 1 都执行
{ type = "say", text = "你接受了祝福", if_choice = {0, 1} }

-- 只选 0 时执行
{ type = "add_health", value = 100, if_choice = 0 }
```

**注意**：不带 `if_choice` 的动作在任何选择后都会执行。通常你应该给 `choose` 之后的每个动作都加上 `if_choice`。

---

## Flag 机制

Flag 是一个 64 位的布尔数组（ID 0~63）。用于：

- **防止事件重复触发**：设置 `condition_flag` 让事件只触发一次
- **跨楼层状态**：flag 在所有楼层间共享，可用于"在第 5 层拿到某道具后第 10 层才开门"这类跨层谜题

```lua
-- 此事件仅在 flag 3 未设置时触发，触发后设置 flag 3
{
    trigger = "on_tile",
    tile = 151,
    condition_flag = 3,   -- 仅当 flag 3 == 0 时触发
    set_flag = 3,         -- 触发后将 flag 3 设为 1
    actions = { ... }
}
```

在 `call` 动作中也可直接操作 flag：

```lua
{ type = "call", func = function()
    if has_flag(5) then
        say("flag 5 已设置")
    else
        set_flag(5)
    end
end }
```

---

## Lua 脚本 API（call 动作可用）

以下函数可在 `call` 的 `func()` 中直接调用：

### 对话与消息

| 函数 | 说明 |
|---|---|
| `say(text)` | 弹出对话框 |
| `msg(text)` | 底部消息栏 |
| `choose_menu(a, b, c, ...)` | 显示选择菜单，最多 8 项。返回 0~7 或 255（取消） |
| `drain()` | 清空输入缓冲区 |

### 属性操作

| 函数 | 说明 |
|---|---|
| `add_health(n)` | 生命 +n |
| `add_attack(n)` | 攻击 +n |
| `add_defence(n)` | 防御 +n |
| `add_money(n)` | 金币 +n |
| `take_money(n)` | 扣除 n 金币。成功返回 `true`，不足返回 `false` |

### 地图操作

| 函数 | 说明 |
|---|---|
| `replace_all(floor, from, to)` | 指定楼层全图替换 |
| `set_tile(x, y, value)` | 设置当前楼层单格地块 |
| `count_monsters(floor)` | 返回指定楼层怪物数量 |

### 标记操作

| 函数 | 说明 |
|---|---|
| `has_flag(id)` | 查询 flag 是否已设置，返回 boolean |
| `set_flag(id)` | 设置 flag |

### 其他

| 函数 | 说明 |
|---|---|
| `player_floor()` | 返回玩家当前楼层号 |
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
        condition_flag = 1,
        set_flag = 1,
        actions = {
            { type = "say", text = "少年，前方的道路充满危险。" },
            { type = "add_health", value = 200 },
            { type = "msg", text = "获得老人的祝福，生命+200" },
        }
    }
}
```

### 示例 2：守卫门

```lua
events = {
    {
        trigger = "on_guard_kill",
        guards = {{x = 1, y = 5}, {x = 3, y = 5}},
        condition_flag = 4,
        set_flag = 4,
        actions = {
            { type = "replace_all", from = 8, to = 1 },
        }
    }
}
```

### 示例 3：祭坛（带选择的分支逻辑）

```lua
events = {
    {
        trigger = "on_tile",
        tile = 155,
        actions = {
            { type = "call", func = function()
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
            end }
        }
    }
}
```

### 示例 4：商人（坐标匹配 + 选择分支）

```lua
events = {
    {
        trigger = "on_tile",
        x = 5, y = 3,   -- 精确匹配 (5,3) 的商人
        condition_flag = 5,
        set_flag = 5,
        actions = {
            { type = "say", text = "你想买点什么？" },
            { type = "choose", choices = { "黄钥匙(10金)", "蓝钥匙(20金)", "不买" } },
            { type = "take_money", value = 10, if_choice = 0 },
            { type = "call", func = function() give(51) end, if_choice = 0 },
            { type = "take_money", value = 20, if_choice = 1 },
            { type = "call", func = function() give(52) end, if_choice = 1 },
        }
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

1. **执行优先级**：玩家移动时，引擎**先查 Lua 事件**，若该坐标有匹配的 `on_tile` 事件则走 Lua 逻辑（跳过 C++ 默认行为）；若没有则走 C++ 内置逻辑（开门/捡道具/战斗/移动）。因此 Lua 事件可以完全覆盖任何地块类型的默认行为。同一 trigger 类型的多个事件，只会执行**第一个**满足条件的（执行后 `break`）。

2. **Flag 范围**：ID 范围 0~63。不要超过 63。

3. **坐标系统**：x 是列号（0~12），y 是行号（0~12）。地图数组中第 1 行是 y=0，第 1 列是 x=0。使用坐标匹配时，不同位置的相同 NPC 可以拥有完全不同的事件逻辑，解决了"一楼层多商人"的问题。

4. **守卫门初始值**：守卫门必须在地图中写为 ID=8，否则 `replace_all` 的 `from=8` 匹配不到。

5. **call 中的错误**：Lua 语法错误会输出到 stderr，游戏不会崩溃。编写复杂逻辑时建议先在控制台 REPL 中测试。

6. **文件编码**：地图文件使用 UTF-8 编码保存。
