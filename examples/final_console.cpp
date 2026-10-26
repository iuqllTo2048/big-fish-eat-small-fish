#include "Game.h"
#include "SaveManager.h"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

static void endOfCommand(std::istringstream &input)
{
    std::string extra;
    if (input >> extra)
        throw std::invalid_argument("命令末尾存在多余参数");
}

int main()
{
    Game game;
    std::cout << "大鱼吃小鱼——实验三至六完整版\n"
              << "list 查看状态；start 开始；pause 暂停/继续；reset 重开\n"
              << "move X Y 设置移动目标；step N 推进N秒(1至1000)\n"
              << "add/edit ID NAME X Y R ENERGY 添加/编辑；remove ID 删除敌鱼\n"
              << "eat HUNTER_ID PREY_ID 捕食；save/load FILE 保存/读取；quit 退出\n"
              << "名称含空格时使用英文双引号，例如：add 20 \"小鱼 甲\" 30 30 2 100\n";
    std::string line;
    while (std::cout << "请输入命令> " && std::getline(std::cin, line))
    {
        std::istringstream input(line);
        std::string command;
        input >> command;
        if (command.empty())
            continue;
        try
        {
            if (command == "quit")
            {
                endOfCommand(input);
                break;
            }
            else if (command == "list")
            {
                endOfCommand(input);
                game.print(std::cout);
            }
            else if (command == "start")
            {
                endOfCommand(input);
                game.start();
            }
            else if (command == "pause")
            {
                endOfCommand(input);
                game.togglePause();
            }
            else if (command == "reset")
            {
                endOfCommand(input);
                game.restart();
            }
            else if (command == "move")
            {
                double x, y;
                if (!(input >> x >> y))
                    throw std::invalid_argument("请输入move X Y");
                endOfCommand(input);
                game.moveTarget(x, y);
            }
            else if (command == "step")
            {
                int count;
                if (!(input >> count) || count < 1 || count > 1000)
                    throw std::invalid_argument("推进秒数必须为1至1000");
                endOfCommand(input);
                for (int i = 0; i < count; ++i)
                    game.step(1);
                std::cout << game.phaseText() << '\n';
            }
            else if (command == "add" || command == "edit")
            {
                int id, energy;
                double x, y, radius;
                std::string name;
                if (!(input >> id >> std::quoted(name) >> x >> y >> radius >> energy))
                    throw std::invalid_argument("请依次输入ID NAME X Y R ENERGY");
                endOfCommand(input);
                if (command == "add")
                    game.addEnemy(id, name, x, y, radius, energy);
                else
                    game.editFish(id, name, x, y, radius, energy);
            }
            else if (command == "remove")
            {
                int id;
                if (!(input >> id))
                    throw std::invalid_argument("请输入编号");
                endOfCommand(input);
                std::cout << (game.removeEnemy(id) ? "删除成功\n" : "未找到该编号\n");
            }
            else if (command == "eat")
            {
                int hunter, prey;
                if (!(input >> hunter >> prey))
                    throw std::invalid_argument("请输入捕食双方编号");
                endOfCommand(input);
                std::cout << (game.eat(hunter, prey) ? "捕食成功\n" : "不满足捕食条件\n");
            }
            else if (command == "save" || command == "load")
            {
                std::string path;
                if (!(input >> std::quoted(path)))
                    throw std::invalid_argument("请输入文件名");
                endOfCommand(input);
                if (command == "save")
                    SaveManager::save(game, path);
                else
                    SaveManager::load(game, path);
                std::cout << "文件操作成功\n";
            }
            else
                throw std::invalid_argument("未知命令");
        }
        catch (const std::exception &error)
        {
            std::cout << "操作失败：" << error.what() << '\n';
        }
    }
}
