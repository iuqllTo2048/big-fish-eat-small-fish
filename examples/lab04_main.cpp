#include "PlayerFish.h"
#include "EnemyFish.h"
#include <iostream>
#include <memory>

int main()
{
    std::cout << "实验四：基类指针调用虚函数\n";
    auto player = std::make_unique<PlayerFish>();
    player->setTarget(60, 30);
    std::unique_ptr<Fish> school[2];
    school[0] = std::move(player);
    school[1] = std::make_unique<EnemyFish>(2, "自动游动鱼", 70, 40, 3);
    for (auto &fish : school)
    {
        fish->update(1, 100, 60);
        fish->print(std::cout);
    }
    std::cout << "同一个update调用，实际执行玩家鱼和敌鱼各自的实现。\n";
    std::cout << "Fish的虚析构函数保证通过基类指针释放时正确析构派生对象。\n";
}
