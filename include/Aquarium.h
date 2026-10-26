#pragma once
#include "Fish.h"
#include <memory>
#include <string>
#include <vector>
class CollisionRule;
class Game;
class SaveManager;

class Aquarium
{
  public:
    Aquarium();
    Aquarium(const std::string &name, double width, double height);

    const std::string &name() const
    {
        return name_;
    }

    double width() const
    {
        return width_;
    }

    double height() const
    {
        return height_;
    }

    std::size_t size() const
    {
        return fishes_.size();
    }

    void addFish(std::unique_ptr<Fish> fish);
    bool removeFish(int id);
    void removeDeadEnemies();
    void clear();
    const Fish &fish(int id) const;
    const Fish &fishAt(std::size_t index) const;
    void updateFish(int id, const std::string &name, double x, double y, double radius, int energy);
    bool feed(int hunterId, int preyId, const CollisionRule &rule);
    void update(double seconds);
    void print(std::ostream &out) const;

  private:
    std::string name_;
    double width_;
    double height_;
    // 存基类智能指针，保留派生类型，避免把派生对象复制成基类而产生切片。
    std::vector<std::unique_ptr<Fish>> fishes_;
    Fish &findMutable(int id);
    void validateBounds(const Fish &fish) const;
    friend class Game;
    friend class SaveManager;
};
