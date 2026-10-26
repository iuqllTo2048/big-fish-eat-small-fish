#pragma once

#include "Fish.h"

#include <string>
#include <vector>

class CollisionRule;

class Aquarium
{
  public:
    Aquarium();
    Aquarium(std::string name, double width, double height);

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

    void setName(const std::string &name);
    void addFish(const Fish &fish);
    bool removeFish(int id);
    const Fish &fish(int id) const;
    void updateFish(int id, const std::string &name, double x, double y, double radius, int energy);
    bool feed(int hunterId, int preyId, const CollisionRule &rule);
    void print(std::ostream &out) const;

  private:
    std::string name_;
    double width_;
    double height_;
    std::vector<Fish> fishes_;

    Fish &findMutable(int id);
    void validateBounds(const Fish &fish) const;
};
