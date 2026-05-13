#pragma once

class Player;
class EventManager;

// 启动欢迎信息和控制台输入线程
void console_welcome();

// 关闭控制台线程
void console_cmd_shutdown();

// 主线程轮询（每帧调用，处理已排队的命令）
void console_poll(Player& player, EventManager& events);
