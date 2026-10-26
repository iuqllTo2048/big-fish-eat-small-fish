#include "Fish.h"

#include <cmath>
#include <ostream>
#include <stdexcept>

Fish::Fish() : Fish(0, "未命名", 10.0, 10.0, 2.0, 100)
{
}

Fish::Fish(int id, std::string name, double x, double y, double radius, int energy)
    : id_(id), name_(name), x_(x), y_(y), radius_(radius), energy_(energy), eatenCount_(0)
{
    if (id < 0)
    {
        throw std::invalid_argument("编号不能为负数");
    }

    setName(name_);
    setPosition(x, y);
    setRadius(radius);
    setEnergy(energy);
}

void Fish::setName(const std::string &name)
{
    if (name.empty() || name.size() > 60 || name.find_first_of("\r\n\t") != std::string::npos)
    {
        throw std::invalid_argument("名称需为1至60个字节，不能包含换行或制表符");
    }

    name_ = name;
}

void Fish::setPosition(double x, double y)
{
    if (!std::isfinite(x) || !std::isfinite(y) || x < 0 || y < 0 || x > 10000 || y > 10000)
    {
        throw std::invalid_argument("坐标必须为0至10000之间的有限数值");
    }

    // 两个坐标都合法后再赋值，避免只修改了一部分数据。
    x_ = x;
    y_ = y;
}

void Fish::setRadius(double radius)
{
    if (!std::isfinite(radius) || radius <= 0 || radius > 1000)
    {
        throw std::invalid_argument("半径必须为大于0且不超过1000的有限数值");
    }

    radius_ = radius;
}

void Fish::setEnergy(int energy)
{
    if (energy < 0 || energy > 100)
    {
        throw std::invalid_argument("能量必须在0至100之间");
    }

    energy_ = energy;
}

void Fish::print(std::ostream &out) const
{
    out << "鱼{编号=" << id_ << "，名称=" << name_ << "，位置=(" << x_ << ',' << y_ << ')'
        << "，半径=" << radius_ << "，能量=" << energy_ << "，捕食次数=" << eatenCount_
        << "，状态=" << (alive() ? "存活" : "死亡") << "}\n";
}
