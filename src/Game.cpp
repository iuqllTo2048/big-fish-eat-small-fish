#include "Game.h"
#include "PlayerFish.h"
#include "EnemyFish.h"
#include "CollisionRule.h"
#include "MathUtils.h"
#include <cmath>
#include <ostream>
#include <random>
#include <stdexcept>

Game::Game()
{
    restart();
}

void Game::restart(unsigned seed)
{
    world_ = Aquarium();
    world_.addFish(std::make_unique<PlayerFish>());
    std::mt19937 random(seed);
    std::uniform_real_distribution<double> px(45, 85), py(13, 47);
    for (int id = 2; id <= 9; ++id)
    {
        const double radius = id >= 8 ? 11.0 + (id - 8) : 2.0 + (id % 4) * 0.7;
        const double x = id == 2 ? 32 : px(random);
        const double y = id == 2 ? 30 : py(random);
        world_.addFish(std::make_unique<EnemyFish>(id, id >= 8 ? "危险大鱼" : "小鱼", x, y, radius,
                                                   100, id % 2 ? 3 : -3, id % 3 ? 1.5 : -1.5));
    }
    phase_ = GamePhase::Ready;
    elapsed_ = 0;
    seed_ = seed;
    targetScore_ = 3;
    message_ = "吃到3条鱼即可获胜，小心更大的敌鱼";
}

const char *Game::phaseText() const
{
    switch (phase_)
    {
    case GamePhase::Ready:
        return "准备开始";
    case GamePhase::Running:
        return "游戏进行中";
    case GamePhase::Paused:
        return "已暂停";
    case GamePhase::Won:
        return "挑战成功";
    case GamePhase::Lost:
        return "挑战失败";
    }
    return "未知状态";
}

int Game::score() const
{
    return world_.fish(1).eatenCount();
}

void Game::start()
{
    if (phase_ == GamePhase::Ready)
    {
        phase_ = GamePhase::Running;
        message_ = "移动玩家鱼，寻找能吃的小鱼";
    }
}

void Game::togglePause()
{
    if (phase_ == GamePhase::Ready)
        start();
    else if (phase_ == GamePhase::Running)
    {
        phase_ = GamePhase::Paused;
        message_ = "已暂停，可保存当前进度";
    }
    else if (phase_ == GamePhase::Paused)
    {
        phase_ = GamePhase::Running;
        message_ = "继续游戏";
    }
}

void Game::moveTarget(double x, double y)
{
    if (!std::isfinite(x) || !std::isfinite(y))
        throw std::invalid_argument("目标位置不合法");
    auto &player = static_cast<PlayerFish &>(world_.findMutable(1));
    player.setTarget(limitValue(x, player.radius(), world_.width() - player.radius()),
                     limitValue(y, player.radius(), world_.height() - player.radius()));
}

void Game::checkOutcome()
{
    if (!world_.fish(1).alive())
    {
        phase_ = GamePhase::Lost;
        message_ = "玩家鱼死亡，按R重新挑战";
    }
    else if (score() >= targetScore_)
    {
        phase_ = GamePhase::Won;
        message_ = "已完成捕食目标，按R再玩一次";
    }
}

void Game::step(double seconds)
{
    if (!std::isfinite(seconds) || seconds <= 0 || seconds > 1)
        throw std::invalid_argument("时间步长应为(0,1]秒");
    if (phase_ != GamePhase::Running)
        return;
    world_.update(seconds);
    CollisionRule rule;
    for (std::size_t i = 0; i < world_.size() && world_.fish(1).alive(); ++i)
    {
        const Fish &enemy = world_.fishAt(i);
        if (enemy.typeCode() != 'E' || !enemy.alive())
            continue;
        const int enemyId = enemy.id();
        if (world_.feed(1, enemyId, rule))
            message_ = "成功吃到一条小鱼，能量已恢复";
        else if (world_.feed(enemyId, 1, rule))
            message_ = "被更大的鱼捕食";
    }
    world_.removeDeadEnemies();
    elapsed_ += seconds;
    checkOutcome();
}

void Game::addEnemy(int id, const std::string &name, double x, double y, double radius, int energy)
{
    world_.addFish(std::make_unique<EnemyFish>(id, name, x, y, radius, energy));
}

void Game::editFish(int id, const std::string &name, double x, double y, double radius, int energy)
{
    world_.updateFish(id, name, x, y, radius, energy);
    checkOutcome();
}

bool Game::removeEnemy(int id)
{
    if (id == 1)
        throw std::invalid_argument("不能删除玩家鱼");
    return world_.removeFish(id);
}

bool Game::eat(int hunter, int prey)
{
    if (phase_ == GamePhase::Won || phase_ == GamePhase::Lost)
        throw std::logic_error("本局已经结束，请先重开");
    CollisionRule rule;
    const bool result = world_.feed(hunter, prey, rule);
    world_.removeDeadEnemies();
    checkOutcome();
    return result;
}

void Game::print(std::ostream &out) const
{
    out << "状态：" << phaseText() << "，得分：" << score() << '/' << targetScore_ << "，已用时间："
        << elapsed_ << "秒\n";
    world_.print(out);
}
