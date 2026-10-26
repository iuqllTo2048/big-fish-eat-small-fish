#include "Game.h"
#include "OpenCVView.h"
#include "SaveManager.h"
#include <filesystem>
#include <iostream>
#include <string>

int main(int argc, char **argv)
{
    try
    {
        Game game;
        OpenCVView view;
        if (argc > 1 && std::string(argv[1]) == "--evidence")
        {
            view.writeEvidence(argc > 2 ? argv[2] : "evidence/final/screens");
            std::cout << "已输出开始、运行、暂停、读取、胜利、失败的真实渲染帧。\n";
            return 0;
        }
        const bool smoke = argc > 1 && std::string(argv[1]) == "--smoke";
        const std::string saveFile = smoke ? "data/smoke_save.dat" : "data/autosave.dat";
        if (!smoke && std::filesystem::exists(std::filesystem::u8path(saveFile)))
        {
            try
            {
                SaveManager::load(game, saveFile);
            }
            catch (const std::exception &error)
            {
                std::cout << "旧存档读取失败，已开始新游戏：" << error.what() << '\n';
            }
        }
        view.run(game, saveFile, smoke ? 45 : 0);
        SaveManager::save(game, saveFile);
        return 0;
    }
    catch (const std::exception &error)
    {
        std::cout << "程序运行失败：" << error.what() << '\n';
        return 1;
    }
}
