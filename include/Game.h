#pragma once
#include "Aquarium.h"
#include <string>
class SaveManager;

enum class GamePhase
{
    Ready,
    Running,
    Paused,
    Won,
    Lost
};

class Game
{
  public:
    Game();
    void restart(unsigned seed = 20261026);
    void start();
    void togglePause();
    void step(double seconds);
    void moveTarget(double x, double y);
    void addEnemy(int id, const std::string &name, double x, double y, double radius, int energy);
    void editFish(int id, const std::string &name, double x, double y, double radius, int energy);
    bool removeEnemy(int id);
    bool eat(int hunter, int prey);

    const Aquarium &aquarium() const
    {
        return world_;
    }

    GamePhase phase() const
    {
        return phase_;
    }

    const char *phaseText() const;

    double elapsed() const
    {
        return elapsed_;
    }

    int score() const;

    int targetScore() const
    {
        return targetScore_;
    }

    const std::string &message() const
    {
        return message_;
    }

    void print(std::ostream &out) const;

  private:
    Aquarium world_;
    GamePhase phase_;
    double elapsed_;
    unsigned seed_;
    int targetScore_;
    std::string message_;
    void checkOutcome();
    friend class SaveManager;
};
