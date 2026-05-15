#pragma once
#include <SDL3/SDL.h>

class Player;

// 创建 3D 窗口
bool run_3d_window(SDL_Window* main_win, Player& player);

// 主循环中每帧调用
void render_3d_frame(const Player& player);

// 通知移动方向（更新玩家朝向）
void notify_3d_move(int dx, int dy);

// 关闭 3D 窗口
void shutdown_3d_window();
