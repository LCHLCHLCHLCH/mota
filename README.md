# 魔塔 (Magic Tower)

经典的魔塔游戏，使用 C++ 开发，基于 SDL3 + SDL_ttf 渲染。

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

### 构建

```bash
git clone https://github.com/LCHLCHLCHLCH/mota.git
cd mota
mkdir build && cd build
cmake -G "MinGW Makefiles" ..
cmake --build .
```

## 项目结构

```
src/             游戏逻辑
  game/          地图、怪物、玩家、道具
  event/         事件系统
  render/        渲染层（帧绘制、区域显示）
  ui/            背包界面
terminal/        SDL3 终端渲染和平台入口
  sdl3/          SDL3 + SDL_ttf 预编译库
  console_cmd    调试控制台
  save_system    存档系统
```

## 控制台命令

游戏运行时可在命令行窗口输入命令：

| 命令 | 说明 |
|---|---|
| `help` | 显示帮助 |
| `info` | 显示玩家状态 |
| `set health 999` | 设置属性 |
| `save <name>` | 保存游戏 |
| `load <name>` | 读取存档 |
| `light on/off` | 切换浅色模式 |

## 许可

C++ 源代码基于 MIT 许可证发布。详见 [LICENSE](LICENSE)。

SDL3 / SDL_ttf 库文件按各自许可证分发。
