#include "EnemyFish.h"
#include <cmath>
#include <ostream>
#include <stdexcept>

EnemyFish::EnemyFish(int id, const std::string &name, double x, double y, double radius, int energy,
                     double vx, double vy, int eatenCount)
    : Fish(id, name, x, y, radius, energy, eatenCount), vx_(vx), vy_(vy)
{
    if (!std::isfinite(vx) || !std::isfinite(vy) || std::abs(vx) > 30 || std::abs(vy) > 30)
        throw std::invalid_argument("敌鱼速度必须为-30至30之间的有限数值");
}

void EnemyFish::update(double seconds, double width, double height)
{
    if (!std::isfinite(seconds) || seconds <= 0 || seconds > 1)
        throw std::invalid_argument("时间步长应为(0,1]秒");
    if (!alive())
        return;
    const double nextX = x() + vx_ * seconds;
    const double nextY = y() + vy_ * seconds;
    if (nextX < radius() || nextX > width - radius())
        vx_ = -vx_;
    if (nextY < radius() || nextY > height - radius())
        vy_ = -vy_;
    moveInside(vx_ * seconds, vy_ * seconds, width, height);
}

std::unique_ptr<Fish> EnemyFish::clone() const
{
    return std::make_unique<EnemyFish>(*this);
}

void EnemyFish::write(std::ostream &out) const
{
    writeCommon(out);
    out << ' ' << vx_ << ' ' << vy_ << '\n';
}
