# 魔塔 (Magic Tower)

经典的魔塔游戏，使用 C++ 开发。

## 游戏操作

| 按键 | 功能 |
|---|---|
| `↑` `↓` `←` `→` | 移动 |
| `Z` | 确认 / 对话 |
| `X` | 取消 / 打开背包 |
| `V` | 冰冻（融化周围岩浆） |
| `Q` | 向下传送（需先获得楼层传送器） |
| `E` | 向上传送 |
| 点击窗口关闭按钮 | 退出游戏 |

## 编译

### 前置要求

- MinGW-w64 (GCC 16+)
- CMake 3.28+
- SDL3 (已内置在 `terminal/sdl3/` 中)

### 构建步骤

```bash
git clone https://github.com/LCHLCHLCHLCH/mota.git
cd mota
mkdir build && cd build
cmake -G "MinGW Makefiles" ..
cmake --build .
```

项目包含两个构建目标：

| 目标 | 说明 | 渲染方式 |
|---|---|---|
| `MagicalTower` | 控制台版本 | ncurses |
| `MotaSDL` | 图形窗口版本 | SDL3 + GDI |

构建全部目标：`cmake --build .`
仅构建 SDL3 版本：`cmake --build . --target MotaSDL`

### Linux

暂不支持。SDL3 版本依赖 Windows GDI 进行 CJK 字体渲染，若需 Linux 支持需改用 SDL3_ttf 或 FreeType。

## 项目结构

```
src/             游戏逻辑（与渲染后端无关）
  game/          地图、怪物、玩家、数值计算
  event/         事件系统（对话、触发器、祭坛）
  render/        渲染抽象层（ncurses / SDL3 共用）
  ui/            背包界面
terminal/        SDL3 终端渲染后端
  sdl3/          SDL3 预编译库 (3.4.8)
lib/             ncurses 预编译库
```

## 许可

本项目中的 C++ 源代码基于 MIT 许可证发布。详见 [LICENSE](LICENSE)。

SDL3 库文件 (`terminal/sdl3/`) 按 SDL 的 zlib 许可证分发，详见其 LICENSE.txt。
