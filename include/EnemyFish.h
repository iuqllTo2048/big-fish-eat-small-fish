#pragma once
#include "Fish.h"

class EnemyFish : public Fish
{
  public:
    EnemyFish(int id, const std::string &name, double x, double y, double radius, int energy = 100,
              double vx = -4, double vy = 2, int eatenCount = 0);

    char typeCode() const override
    {
        return 'E';
    }

    double vx() const
    {
        return vx_;
    }

    double vy() const
    {
        return vy_;
    }

    void update(double seconds, double width, double height) override;
    std::unique_ptr<Fish> clone() const override;
    void write(std::ostream &out) const override;

  private:
    double vx_;
    double vy_;
};
