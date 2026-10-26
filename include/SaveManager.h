#pragma once
#include <string>
class Game;

// 实验六：文件读写独立于游戏和图形界面，便于验证损坏文件的处理。
class SaveManager
{
  public:
    static void save(const Game &game, const std::string &filename);
    static void load(Game &game, const std::string &filename);
};
