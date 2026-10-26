#pragma once

#include <iosfwd>
#include <string>

class CollisionRule;

class Fish
{
  public:
    Fish();
    Fish(int id, std::string name, double x, double y, double radius, int energy = 100);

    int id() const
    {
        return id_;
    }

    const std::string &name() const
    {
        return name_;
    }

    double x() const
    {
        return x_;
    }

    double y() const
    {
        return y_;
    }

    double radius() const
    {
        return radius_;
    }

    int energy() const
    {
        return energy_;
    }

    int eatenCount() const
    {
        return eatenCount_;
    }

    bool alive() const
    {
        return energy_ > 0;
    }

    void setName(const std::string &name);
    void setPosition(double x, double y);
    void setRadius(double radius);
    void setEnergy(int energy);
    void print(std::ostream &out) const;

    // 捕食会改变猎物的状态，因此这里必须传引用。
    bool eat(Fish &prey, const CollisionRule &rule);

  private:
    int id_;
    std::string name_;
    double x_;
    double y_;
    double radius_;
    int energy_;
    int eatenCount_;
};
