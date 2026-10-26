#include "PlayerFish.h"
#include "EnemyFish.h"
#include <iostream>

int main()
{
    std::cout << "实验三：继承与派生\n";
    PlayerFish player;
    EnemyFish enemy(2, "小鱼", 40, 30, 3);
    player.setName("玩家一号");
    player.setTarget(40, 30);
    player.update(1, 100, 60);
    enemy.update(1, 100, 60);
    player.print(std::cout);
    enemy.print(std::cout);
    std::cout << "派生类复用Fish的数据校验和输出接口，并增加不同的移动行为。\n";
}
