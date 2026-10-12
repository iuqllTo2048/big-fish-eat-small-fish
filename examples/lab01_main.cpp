#include "Fish.h"

#include <iostream>
#include <stdexcept>

int main()
{
    std::cout << "实验一：类与对象\n";

    Fish defaultFish;
    Fish player(1, "玩家鱼", 20, 20, 8, 70);

    defaultFish.print(std::cout);
    player.print(std::cout);

    player.setName("大鱼");
    player.setPosition(25, 22);
    player.setRadius(9);
    player.setEnergy(80);

    std::cout << "合法修改后的对象：\n";
    player.print(std::cout);

    try
    {
        player.setRadius(-1);
    }
    catch (const std::invalid_argument &e)
    {
        std::cout << "已拒绝非法输入：" << e.what() << '\n';
    }

    std::cout << "修改失败后，半径仍为 " << player.radius() << '\n';
    return 0;
}
