#include "Fish.h"
#include "CollisionRule.h"

#include <climits>
#include <stdexcept>

bool Fish::eat(Fish &prey, const CollisionRule &rule)
{
    if (!rule.canEat(*this, prey))
    {
        return false;
    }

    if (eatenCount_ == INT_MAX)
    {
        throw std::overflow_error("捕食次数超出整数范围");
    }

    // 修复传参问题：prey 按引用传递，修改的是鱼缸中的原对象。
    prey.energy_ = 0;
    energy_ = 100;
    ++eatenCount_;

    return true;
}
