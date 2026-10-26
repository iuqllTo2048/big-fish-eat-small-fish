#include "CollisionRule.h"
#include "Fish.h"

#include <cmath>

bool CollisionRule::canEat(const Fish &hunter, const Fish &prey) const
{
    if (&hunter == &prey || !hunter.alive() || !prey.alive() || hunter.radius() <= prey.radius())
    {
        return false;
    }

    // 完整包围小鱼才允许捕食，恰好内切也满足条件。
    const double distance = std::hypot(hunter.x() - prey.x(), hunter.y() - prey.y());

    return distance <= hunter.radius() - prey.radius();
}
