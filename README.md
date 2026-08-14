# 魔塔 (Magic Tower)

经典的魔塔游戏，基于 SDL3 + SDL_ttf 渲染，Lua 脚本驱动。

## 操作

| 按键 | 功能 |
|---|---|
| `↑` `↓` `←` `→` | 移动 |
| `Z` | 确认 / 对话 |
| `X` | 取消 / 打开背包（选中物品后按 Z 使用） |
| `V` | 冰冻四周岩浆 |
| `Q` / `E` | 上传送 / 下传送（需获得楼层传送器） |

## 编译

**前置要求**: MinGW-w64 (GCC 16+)、CMake 3.28+

```bash
git clone https://github.com/LCHLCHLCHLCH/mota.git
cd mota
mkdir build && cd build
cmake -G "MinGW Makefiles" ..
cmake --build .
```

## Lua 控制台

游戏运行时可在命令行窗口直接输入 Lua 代码：

```lua
set("health", 99999)    -- 设置属性
set("attack", 500)
info()                  -- 查看状态
tp(15)                  -- 传送到 15 层
give(66)                -- 给予神圣剑
save("mygame")          -- 保存
load("mygame")          -- 读档

print(2 ^ 10)           -- 任意 Lua 表达式
```

输入 `help` 查看完整命令列表。

## 启动脚本

游戏启动时会自动执行 `scripts/startup.lua`——它是一个普通 Lua 脚本，可在其中调用 `set`/`give`/`add_health` 等任意 API 配置初始状态。删除或重命名该文件即回到默认开局（1000 生命 / 100 攻击 / 100 防御 / 0 金币 / 0 钥匙）。

```lua
set("health", 100000)   -- 设置属性
set("attack", 5000)
set("yellow", 100)      -- 钥匙
give(78)                -- 给予地震卷轴
```

## 项目结构

```
src/             游戏逻辑
  game/          地图、怪物、玩家、道具
  event/         事件系统
  render/        渲染层
  ui/            背包
terminal/        SDL3 终端 + 控制台 + 存档
scripts/         Lua 脚本（items.lua / startup.lua）
  map/           各楼层地图与事件（fN.lua）
lib/             SDL3 / SDL_ttf / Lua 预编译库与源码
```

## 许可

C++ 源代码基于 MIT 许可证。SDL3 / SDL_ttf 按各自许可证分发。
