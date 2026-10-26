#include "PlayerFish.h"
#include "MathUtils.h"
#include <cmath>
#include <ostream>
#include <stdexcept>

PlayerFish::PlayerFish() : PlayerFish(1, "玩家鱼", 20, 30, 8)
{
}

PlayerFish::PlayerFish(int id, const std::string &name, double x, double y, double radius,
                       int energy, int eatenCount)
    : Fish(id, name, x, y, radius, energy, eatenCount), targetX_(x), targetY_(y), hungerClock_(0)
{
}

void PlayerFish::setTarget(double x, double y)
{
    if (!std::isfinite(x) || !std::isfinite(y) || x < 0 || y < 0 || x > 10000 || y > 10000)
        throw std::invalid_argument("目标坐标不合法");
    targetX_ = x;
    targetY_ = y;
}

void PlayerFish::restoreHungerClock(double value)
{
    if (!std::isfinite(value) || value < 0 || value >= 3)
        throw std::invalid_argument("饥饿计时不合法");
    hungerClock_ = value;
}

void PlayerFish::update(double seconds, double width, double height)
{
    if (!std::isfinite(seconds) || seconds <= 0 || seconds > 1)
        throw std::invalid_argument("时间步长应为(0,1]秒");
    if (!alive())
        return;
    const double tx = limitValue(targetX_, radius(), width - radius());
    const double ty = limitValue(targetY_, radius(), height - radius());
    const double distance = std::hypot(tx - x(), ty - y());
    if (distance > 0)
    {
        const double step = std::min(distance, 18.0 * seconds);
        moveInside((tx - x()) / distance * step, (ty - y()) / distance * step, width, height);
    }
    hungerClock_ += seconds;
    while (hungerClock_ >= 3)
    {
        consumeEnergy(1);
        hungerClock_ -= 3;
    }
}

std::unique_ptr<Fish> PlayerFish::clone() const
{
    return std::make_unique<PlayerFish>(*this);
}

void PlayerFish::write(std::ostream &out) const
{
    writeCommon(out);
    out << ' ' << targetX_ << ' ' << targetY_ << ' ' << hungerClock_ << '\n';
}
