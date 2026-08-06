# 事件标记（Flag）机制

## 每层独立 Flag 空间

系统维护 `flags_[51][16]` 二维数组：51 个楼层 × 每层 16 个 flag。不同楼层之间的 flag 完全隔离，不会互相干扰。

## `once = true` —— 推荐的一次性触发

只需在事件中加 `once = true`，系统自动分配内部 flag，无需手动管理编号：

```lua
{
    trigger = "on_tile",
    x = 3, y = 7,
    once = true,        -- 自动一次性触发
    run = function()
        -- 事件逻辑
    end
}
```

**实现原理**：内部使用事件在 `events` 数组中的索引（0~15）作为隐式 flag ID，存储在当前楼层的独立空间中。

**限制**：每层最多 16 个事件（`MAX_FLAGS = 16`）。

## `condition_flag` / `set_flag` —— 手动指定（兼容旧版）

仍可使用手动 flag 编号，范围为 0~15，仅影响当前楼层：

```lua
{
    trigger = "on_guard_kill",
    guards = {{x = 1, y = 5}, {x = 3, y = 5}},
    condition_flag = 4,   -- 仅当本层 flag 4 未设置时触发
    set_flag = 4,         -- 触发后将本层 flag 4 置为 1
    run = function()
        -- 事件逻辑
    end
}
```

## 优先级

`once` 和 `condition_flag` 同时存在时，**`once` 优先**（`condition_flag` 被忽略）。

## Lua API

```lua
has_flag(id)   -- 查询当前楼层 flag id 是否已设置（id: 0~15）
set_flag(id)   -- 设置当前楼层 flag id 为 1
```

这两个函数操作的始终是**当前玩家所在楼层**的 flag 空间。

## 当前事件清单

| 楼层 | 位置/条件 | once | 类型 | 说明 |
|------|-----------|------|------|------|
| 2 | 小偷 (3, 7) | ✓ | on_tile | 小偷对话后消失，开通暗道 |
| 2 | 监狱守卫门 | ✓ | on_guard_kill | 击败中级卫兵×2 → 开门 |
| 3 | 老人 (tile 151) | ✓ | on_tile | 怪物手册提示 |
| 3 | (5, 9) | ✓ | on_tile | 魔王埋伏——警卫×4+魔王，黑屏演出 |
| 4 | 老人 (tile 151) | ✓ | on_tile | 守卫门说明 |
| 4 | 祭坛 (tile 155) | ✗ | on_tile | 可重复供奉 |
| 6 | 老人 (4, 8) | ✓ | on_tile | 提示对话 |
| 6 | 商人 (8, 4) | ✗ | on_tile | 卖蓝钥匙，两次对话 |
| 7 | 商人 (6, 1) | ✗ | on_tile | 卖五把黄钥匙，两次对话 |
| 8 | 巫师守卫门 | ✓ | on_guard_kill | 击败高级法师×2 → 开门 |
| 10 | (6, 5) | ✓ | on_tile | 埋伏战——围困+骷髅群 |
| 10 | 骷髅守卫组 | ✓ | on_guard_kill | 杀出重围 → 决斗 |
| 10 | 骷髅队长 (6, 1) | ✓ | on_guard_kill | 掉宝石/血瓶/钥匙/楼梯，开怪物门 |
| 10 | 小偷 (tile 154) | ✓ | on_tile | 提示银盾/银剑位置 |
| 11 | 巫师守卫门 | ✓ | on_guard_kill | 击败高级法师×2 → 开门 |
| 12 | 商人 (1, 1) | ✗ | on_tile | 卖红钥匙，两次对话 |
| 12 | 商人 (11, 1) | ✗ | on_tile | 暗道揭示 + 卖黄钥匙 |
| 12 | 祭坛 (tile 155) | ✗ | on_tile | 可重复供奉 |
| 14 | 石魔守卫组 | ✓ | on_guard_kill | 开门并露出一把红钥匙 |
| 15 | 巨型章鱼 (6, 7) | ✓ | on_guard_kill | 岩浆退去，露出镐子 |
| 15 | 商人 (11, 11) | ✗ | on_tile | 卖蓝钥匙，两次对话 |
| 32 | 祭坛 (tile 155) | ✗ | on_tile | 可重复供奉 |
| 46 | 祭坛 (tile 155) | ✗ | on_tile | 可重复供奉 |

大部分事件使用 `once = true`；商人、祭坛等可重复交互的事件不带 `once`，改用 `has_flag` / `set_flag` 在函数内管理对话状态。
