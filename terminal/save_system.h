#pragma once

class Player;
class EventManager;

// 保存到 saves/<name>.sav，返回成功与否
bool save_game(const char* name, const Player& player, const EventManager& events);

// 从 saves/<name>.sav 读取，返回成功与否
bool load_game(const char* name, Player& player, EventManager& events);
