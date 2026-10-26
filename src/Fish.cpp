#include "Fish.h"
#include "MathUtils.h"
#include <cmath>
#include <iomanip>
#include <ostream>
#include <stdexcept>

Fish::Fish(int id, const std::string &name, double x, double y, double radius, int energy,
           int eatenCount)
    : id_(id), name_(name), x_(x), y_(y), radius_(radius), energy_(energy), eatenCount_(eatenCount)
{
    if (id < 0 || eatenCount < 0 || eatenCount > 1000000)
        throw std::invalid_argument("编号或捕食次数不合法");
    setName(name);
    setPosition(x, y);
    setRadius(radius);
    setEnergy(energy);
}

Fish::~Fish() = default;

void Fish::setName(const std::string &name)
{
    if (name.empty() || name.size() > 60 || name.find_first_of("\r\n\t") != std::string::npos)
        throw std::invalid_argument("名称需为1至60个字节，不能包含换行或制表符");
    name_ = name;
}

void Fish::setPosition(double x, double y)
{
    if (!std::isfinite(x) || !std::isfinite(y) || x < 0 || y < 0 || x > 10000 || y > 10000)
        throw std::invalid_argument("坐标必须为0至10000之间的有限数值");
    x_ = x;
    y_ = y;
}

void Fish::setRadius(double radius)
{
    if (!std::isfinite(radius) || radius <= 0 || radius > 1000)
        throw std::invalid_argument("半径必须为大于0且不超过1000的有限数值");
    radius_ = radius;
}

void Fish::setEnergy(int energy)
{
    if (energy < 0 || energy > 100)
        throw std::invalid_argument("能量必须在0至100之间");
    energy_ = energy;
}

void Fish::consumeEnergy(int amount)
{
    if (amount < 0 || amount > 100)
        throw std::invalid_argument("能量消耗量应在0至100之间");
    energy_ = limitValue<int>(energy_ - amount, 0, 100);
}

void Fish::moveInside(double dx, double dy, double width, double height)
{
    setPosition(limitValue(x_ + dx, radius_, width - radius_),
                limitValue(y_ + dy, radius_, height - radius_));
}

void Fish::print(std::ostream &out) const
{
    out << "鱼{类型=" << (typeCode() == 'P' ? "玩家鱼" : "敌鱼") << "，编号=" << id_
        << "，名称=" << name_ << "，位置=(" << x_ << ',' << y_ << ")，半径=" << radius_
        << "，能量=" << energy_ << "，捕食次数=" << eatenCount_
        << "，状态=" << (alive() ? "存活" : "死亡") << "}\n";
}

void Fish::writeCommon(std::ostream &out) const
{
    out << typeCode() << ' ' << id_ << ' ' << std::quoted(name_) << ' ' << std::setprecision(17)
        << x_ << ' ' << y_ << ' ' << radius_ << ' ' << energy_ << ' ' << eatenCount_;
}
