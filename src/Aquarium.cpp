#include "Aquarium.h"
#include "CollisionRule.h"

#include <cmath>
#include <ostream>
#include <stdexcept>

Aquarium::Aquarium() : Aquarium("演示鱼缸", 100, 60)
{
}

Aquarium::Aquarium(std::string name, double width, double height)
    : name_(name), width_(width), height_(height), fishCount_(0)
{
    if (!std::isfinite(width) || !std::isfinite(height) || width <= 0 || height <= 0 ||
        width > 10000 || height > 10000)
    {
        throw std::invalid_argument("鱼缸宽高必须为大于0且不超过10000的有限数值");
    }

    setName(name_);
}

void Aquarium::setName(const std::string &name)
{
    if (name.empty() || name.size() > 60 || name.find_first_of("\r\n\t") != std::string::npos)
    {
        throw std::invalid_argument("鱼缸名称需为1至60个字节，不能包含换行或制表符");
    }

    name_ = name;
}

void Aquarium::validateBounds(const Fish &fish) const
{
    if (fish.x() - fish.radius() < 0 || fish.y() - fish.radius() < 0 ||
        fish.x() + fish.radius() > width_ || fish.y() + fish.radius() > height_)
    {
        throw std::invalid_argument("鱼必须完整处于鱼缸边界内");
    }
}

void Aquarium::addFish(const Fish &fish)
{
    validateBounds(fish);

    for (int i = 0; i < fishCount_; ++i)
    {
        if (fishes_[i].id() == fish.id())
        {
            throw std::invalid_argument("鱼的编号不能重复");
        }
    }

    if (fishCount_ >= Capacity)
    {
        throw std::length_error("鱼缸已满，最多容纳20条鱼");
    }

    fishes_[fishCount_] = fish;
    ++fishCount_;
}

bool Aquarium::removeFish(int id)
{
    for (int i = 0; i < fishCount_; ++i)
    {
        if (fishes_[i].id() == id)
        {
            for (int j = i; j < fishCount_ - 1; ++j)
            {
                fishes_[j] = fishes_[j + 1];
            }

            --fishCount_;
            fishes_[fishCount_] = Fish();
            return true;
        }
    }

    return false;
}

const Fish &Aquarium::fish(int id) const
{
    for (int i = 0; i < fishCount_; ++i)
    {
        if (fishes_[i].id() == id)
        {
            return fishes_[i];
        }
    }

    throw std::out_of_range("未找到指定编号的鱼");
}

Fish &Aquarium::findMutable(int id)
{
    for (int i = 0; i < fishCount_; ++i)
    {
        if (fishes_[i].id() == id)
        {
            return fishes_[i];
        }
    }

    throw std::out_of_range("未找到指定编号的鱼");
}

void Aquarium::updateFish(int id, const std::string &name, double x, double y, double radius,
                          int energy)
{
    Fish &original = findMutable(id);
    Fish candidate = original;

    candidate.setName(name);
    candidate.setPosition(x, y);
    candidate.setRadius(radius);
    candidate.setEnergy(energy);
    validateBounds(candidate);

    // 所有数据校验通过后再替换，修改失败时保留原对象。
    original = candidate;
}

bool Aquarium::feed(int hunterId, int preyId, const CollisionRule &rule)
{
    Fish &hunter = findMutable(hunterId);
    Fish &prey = findMutable(preyId);

    return hunter.eat(prey, rule);
}

void Aquarium::print(std::ostream &out) const
{
    out << "鱼缸{名称=" << name_ << "，宽高=" << width_ << 'x' << height_
        << "，鱼数量=" << fishCount_ << "，容量=" << Capacity << "}\n";

    for (int i = 0; i < fishCount_; ++i)
    {
        fishes_[i].print(out);
    }
}
