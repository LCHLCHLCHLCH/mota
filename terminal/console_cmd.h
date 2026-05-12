#pragma once

class Player;

// 启动时显示欢迎信息
void console_welcome();

// 非阻塞处理控制台输入（每帧调用）
void console_poll(Player& player);
