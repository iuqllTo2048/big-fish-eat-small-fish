#include "Game.h"
#include "SaveManager.h"
#include <iostream>
#include <string>

int main(int argc, char **argv)
{
    try
    {
        const std::string path = argc > 1 ? argv[1] : "data/lab06_demo.dat";
        Game game;
        game.start();
        game.moveTarget(25, 30);
        game.step(0.5);
        game.togglePause();
        std::cout << "实验六：保存前\n";
        game.print(std::cout);
        SaveManager::save(game, path);
        Game restored;
        SaveManager::load(restored, path);
        std::cout << "读取后\n";
        restored.print(std::cout);
        std::cout << "已恢复玩家/敌鱼的派生类型、状态和游戏进度。\n";
    }
    catch (const std::exception &error)
    {
        std::cout << "文件操作失败：" << error.what() << '\n';
        return 1;
    }
}
