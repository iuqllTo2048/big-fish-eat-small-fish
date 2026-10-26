#include "Aquarium.h"
#include "CollisionRule.h"
#include "MathUtils.h"
#include <cmath>
#include <ostream>
#include <stdexcept>

Aquarium::Aquarium() : Aquarium("大鱼吃小鱼", 100, 60)
{
}

Aquarium::Aquarium(const std::string &name, double width, double height)
    : name_(name), width_(width), height_(height)
{
    if (name.empty() || name.size() > 60 || !std::isfinite(width) || !std::isfinite(height) ||
        width < 20 || height < 20 || width > 10000 || height > 10000)
        throw std::invalid_argument("鱼缸名称或宽高不合法");
}

void Aquarium::validateBounds(const Fish &fish) const
{
    if (fish.x() - fish.radius() < 0 || fish.y() - fish.radius() < 0 ||
        fish.x() + fish.radius() > width_ || fish.y() + fish.radius() > height_)
        throw std::invalid_argument("鱼必须完整处于鱼缸边界内");
}

void Aquarium::addFish(std::unique_ptr<Fish> fish)
{
    if (!fish)
        throw std::invalid_argument("不能添加空鱼对象");
    if (fishes_.size() >= 200)
        throw std::length_error("鱼的数量不能超过200");
    validateBounds(*fish);
    for (const auto &owned : fishes_)
        if (owned->id() == fish->id())
            throw std::invalid_argument("鱼的编号不能重复");
    fishes_.push_back(std::move(fish));
}

bool Aquarium::removeFish(int id)
{
    for (auto it = fishes_.begin(); it != fishes_.end(); ++it)
    {
        if ((*it)->id() == id)
        {
            fishes_.erase(it);
            return true;
        }
    }
    return false;
}

void Aquarium::removeDeadEnemies()
{
    for (auto it = fishes_.begin(); it != fishes_.end();)
    {
        // erase返回下一项，删除后不能再额外加一，否则会漏掉相邻死亡鱼。
        if ((*it)->typeCode() == 'E' && !(*it)->alive())
            it = fishes_.erase(it);
        else
            ++it;
    }
}

void Aquarium::clear()
{
    fishes_.clear();
}

const Fish &Aquarium::fish(int id) const
{
    for (const auto &f : fishes_)
        if (f->id() == id)
            return *f;
    throw std::out_of_range("未找到指定编号的鱼");
}

Fish &Aquarium::findMutable(int id)
{
    for (auto &f : fishes_)
        if (f->id() == id)
            return *f;
    throw std::out_of_range("未找到指定编号的鱼");
}

const Fish &Aquarium::fishAt(std::size_t index) const
{
    return *fishes_.at(index);
}

void Aquarium::updateFish(int id, const std::string &name, double x, double y, double radius,
                          int energy)
{
    for (auto &f : fishes_)
    {
        if (f->id() != id)
            continue;
        auto candidate = f->clone();
        candidate->setName(name);
        candidate->setPosition(x, y);
        candidate->setRadius(radius);
        candidate->setEnergy(energy);
        validateBounds(*candidate);
        f = std::move(candidate);
        return;
    }
    throw std::out_of_range("未找到指定编号的鱼");
}

bool Aquarium::feed(int hunterId, int preyId, const CollisionRule &rule)
{
    Fish &hunter = findMutable(hunterId);
    Fish &prey = findMutable(preyId);
    if (!hunter.eat(prey, rule))
        return false;
    const double grown =
        std::sqrt(hunter.radius() * hunter.radius() + 0.25 * prey.radius() * prey.radius());
    hunter.setRadius(std::min(grown, std::min(width_, height_) / 2));
    hunter.setPosition(limitValue(hunter.x(), hunter.radius(), width_ - hunter.radius()),
                       limitValue(hunter.y(), hunter.radius(), height_ - hunter.radius()));
    return true;
}

void Aquarium::update(double seconds)
{
    if (!std::isfinite(seconds) || seconds <= 0 || seconds > 1)
        throw std::invalid_argument("每次推进时间必须为大于0且不超过1秒");
    for (auto &f : fishes_)
        f->update(seconds, width_, height_);
}

void Aquarium::print(std::ostream &out) const
{
    out << "鱼缸{名称=" << name_ << "，宽高=" << width_ << 'x' << height_ << "，鱼数量=" << size()
        << "}\n";
    for (const auto &f : fishes_)
        f->print(out);
}
