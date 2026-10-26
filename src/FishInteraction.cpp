#include "Fish.h"
#include "CollisionRule.h"
#include <stdexcept>

bool Fish::eat(Fish &prey, const CollisionRule &rule)
{
    if (!rule.canEat(*this, prey))
        return false;
    if (eatenCount_ >= 1000000)
        throw std::overflow_error("捕食次数达到上限");
    // prey必须传引用，才能改变鱼缸中原始猎物的状态。
    prey.energy_ = 0;
    energy_ = 100;
    ++eatenCount_;
    return true;
}
