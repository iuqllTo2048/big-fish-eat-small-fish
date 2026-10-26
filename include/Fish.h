#pragma once
#include <iosfwd>
#include <memory>
#include <string>
class CollisionRule;

class Fish
{
  public:
    Fish(int id, const std::string &name, double x, double y, double radius, int energy = 100,
         int eatenCount = 0);
    virtual ~Fish();

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
    void consumeEnergy(int amount);
    bool eat(Fish &prey, const CollisionRule &rule);
    void print(std::ostream &out) const;

    // 实验三、四：派生类通过同一接口提供不同的运动和存储行为。
    virtual char typeCode() const = 0;
    virtual void update(double seconds, double width, double height) = 0;
    virtual std::unique_ptr<Fish> clone() const = 0;
    virtual void write(std::ostream &out) const = 0;

  protected:
    void moveInside(double dx, double dy, double width, double height);
    void writeCommon(std::ostream &out) const;

  private:
    int id_;
    std::string name_;
    double x_;
    double y_;
    double radius_;
    int energy_;
    int eatenCount_;
};
