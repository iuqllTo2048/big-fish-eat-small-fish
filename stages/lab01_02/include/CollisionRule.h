#pragma once

class Fish;

// 只读取鱼的属性，不持有鱼对象。
class CollisionRule
{
  public:
    bool canEat(const Fish &hunter, const Fish &prey) const;
};
