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
light("on")             -- 浅色模式

print(2 ^ 10)           -- 任意 Lua 表达式
```

输入 `help` 查看完整命令列表。

## 项目结构

```
src/             游戏逻辑
  game/          地图、怪物、玩家、道具
  event/         事件系统
  render/        渲染层
  ui/            背包
terminal/        SDL3 终端 + 控制台 + 存档
lib/             SDL3 / SDL_ttf 预编译库
lua-5.5.0/       Lua 5.5.0 源码
```

## 许可

C++ 源代码基于 MIT 许可证。SDL3 / SDL_ttf 按各自许可证分发。
