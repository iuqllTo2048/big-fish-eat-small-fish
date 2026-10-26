#pragma once
#include "Fish.h"

class PlayerFish : public Fish
{
  public:
    PlayerFish();
    PlayerFish(int id, const std::string &name, double x, double y, double radius, int energy = 100,
               int eatenCount = 0);

    char typeCode() const override
    {
        return 'P';
    }

    void setTarget(double x, double y);

    double targetX() const
    {
        return targetX_;
    }

    double targetY() const
    {
        return targetY_;
    }

    double hungerClock() const
    {
        return hungerClock_;
    }

    void restoreHungerClock(double value);
    void update(double seconds, double width, double height) override;
    std::unique_ptr<Fish> clone() const override;
    void write(std::ostream &out) const override;

  private:
    double targetX_;
    double targetY_;
    double hungerClock_;
};
