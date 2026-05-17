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
    actions = { ... }
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
    actions = { ... }
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
| 11 | 巫师守卫门 | ✓ | on_guard_kill | 击败高级法师×2 → 开门 |

所有事件均使用 `once = true`，无手动 flag 编号，无跨层冲突。
