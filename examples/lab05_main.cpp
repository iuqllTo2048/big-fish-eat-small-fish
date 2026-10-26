#include "Aquarium.h"
#include "PlayerFish.h"
#include "EnemyFish.h"
#include "MathUtils.h"
#include <iostream>

int main()
{
    std::cout << "实验五：vector与函数模板\n";
    Aquarium tank;
    tank.addFish(std::make_unique<PlayerFish>());
    tank.addFish(std::make_unique<EnemyFish>(2, "小鱼甲", 40, 30, 3));
    tank.addFish(std::make_unique<EnemyFish>(3, "小鱼乙", 50, 30, 4));
    tank.print(std::cout);
    tank.removeFish(2);
    std::cout << "删除编号2后数量：" << tank.size() << '\n';
    std::cout << "整数模板结果：" << limitValue<int>(120, 0, 100) << '\n';
    std::cout << "浮点模板结果：" << limitValue<double>(-3.5, 0.0, 100.0) << '\n';
    tank.clear();
    std::cout << "clear后数量：" << tank.size() << '\n';
}
