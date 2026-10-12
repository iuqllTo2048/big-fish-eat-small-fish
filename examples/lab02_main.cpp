#include "Aquarium.h"
#include "CollisionRule.h"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

static Aquarium makeDemo()
{
    Aquarium tank("大鱼吃小鱼", 100, 60);
    tank.addFish(Fish(1, "玩家鱼", 30, 30, 8, 70));
    tank.addFish(Fish(2, "小鱼", 33, 30, 2));
    tank.addFish(Fish(3, "远处的鱼", 80, 30, 3));

    return tank;
}

static void requireEnd(std::istringstream &input)
{
    std::string extra;

    if (input >> extra)
    {
        throw std::invalid_argument("命令末尾存在多余参数");
    }
}

static void demo()
{
    Aquarium tank = makeDemo();
    CollisionRule rule;

    std::cout << "实验二：组合与依赖关系\n捕食前：\n";
    tank.print(std::cout);

    std::cout << "捕食附近的小鱼：" << (tank.feed(1, 2, rule) ? "成功" : "不满足条件") << '\n';
    std::cout << "捕食远处的鱼：" << (tank.feed(1, 3, rule) ? "成功" : "不满足条件") << '\n';

    std::cout << "捕食后：\n";
    tank.print(std::cout);
}

int main(int argc, char **argv)
{
    if (argc > 1 && std::string(argv[1]) == "--demo")
    {
        demo();
        return 0;
    }

    Aquarium tank = makeDemo();
    CollisionRule rule;

    std::cout << "大鱼吃小鱼——实验二\n"
              << "请按以下英文命令输入：\n"
              << "list                         查看所有鱼\n"
              << "add ID NAME X Y R ENERGY     添加鱼\n"
              << "edit ID NAME X Y R ENERGY    修改鱼\n"
              << "remove ID                    删除鱼\n"
              << "eat HUNTER_ID PREY_ID        大鱼捕食小鱼\n"
              << "reset                        恢复初始鱼缸\n"
              << "quit                         退出程序\n"
              << "ID为编号，NAME为名称，X/Y为坐标，R为半径，ENERGY为能量。\n"
              << "名称不能包含空格，坐标使用逻辑单位。\n";

    std::string line;

    while (std::cout << "请输入命令> " && std::getline(std::cin, line))
    {
        std::istringstream input(line);
        std::string command;
        input >> command;

        if (command.empty())
        {
            continue;
        }

        try
        {
            if (command == "quit")
            {
                requireEnd(input);
                break;
            }
            else if (command == "list")
            {
                requireEnd(input);
                tank.print(std::cout);
            }
            else if (command == "reset")
            {
                requireEnd(input);
                tank = makeDemo();
                std::cout << "已恢复初始鱼缸\n";
            }
            else if (command == "add" || command == "edit")
            {
                int id;
                int energy;
                std::string name;
                double x;
                double y;
                double radius;

                if (!(input >> id >> name >> x >> y >> radius >> energy))
                {
                    throw std::invalid_argument("请依次输入 ID NAME X Y R ENERGY");
                }

                requireEnd(input);

                if (command == "add")
                {
                    tank.addFish(Fish(id, name, x, y, radius, energy));
                }
                else
                {
                    tank.updateFish(id, name, x, y, radius, energy);
                }

                std::cout << "操作成功\n";
            }
            else if (command == "remove")
            {
                int id;

                if (!(input >> id))
                {
                    throw std::invalid_argument("请输入需要删除的鱼的编号 ID");
                }

                requireEnd(input);
                std::cout << (tank.removeFish(id) ? "删除成功\n" : "未找到该编号\n");
            }
            else if (command == "eat")
            {
                int hunter;
                int prey;

                if (!(input >> hunter >> prey))
                {
                    throw std::invalid_argument("请依次输入捕食者编号和猎物编号");
                }

                requireEnd(input);
                bool eaten = tank.feed(hunter, prey, rule);
                std::cout << (eaten ? "捕食成功\n" : "不满足捕食条件\n");
            }
            else
            {
                throw std::invalid_argument("未知命令，请使用菜单中的英文命令");
            }
        }
        catch (const std::exception &e)
        {
            std::cout << "操作失败：" << e.what() << '\n';
        }
    }

    return 0;
}
