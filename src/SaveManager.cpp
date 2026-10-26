#define NOMINMAX
#include "SaveManager.h"
#include "Game.h"
#include "PlayerFish.h"
#include "EnemyFish.h"
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <limits>
#include <stdexcept>
#ifdef _WIN32
#include <windows.h>
#endif

void SaveManager::save(const Game &game, const std::string &filename)
{
    if (filename.empty())
        throw std::invalid_argument("存档文件名不能为空");
    const auto path = std::filesystem::u8path(filename);
    if (!path.parent_path().empty())
        std::filesystem::create_directories(path.parent_path());
    auto temporary = path;
    temporary += ".tmp";
    std::ofstream out(temporary, std::ios::trunc);
    if (!out)
        throw std::runtime_error("无法打开存档文件进行写入");
    out << "BIG_FISH_SAVE 1\nWORLD " << std::setprecision(17) << game.world_.width() << ' '
        << game.world_.height() << ' ' << std::quoted(game.world_.name()) << "\nGAME "
        << static_cast<int>(game.phase_) << ' ' << game.elapsed_ << ' ' << game.seed_ << ' '
        << game.targetScore_ << "\nFISHES " << game.world_.size() << '\n';
    for (const auto &fish : game.world_.fishes_)
        fish->write(out);
    out << "END\n";
    out.close();
    if (!out)
        throw std::runtime_error("写入存档失败，旧存档未替换");
#ifdef _WIN32
    if (!MoveFileExW(temporary.c_str(), path.c_str(),
                     MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
        throw std::runtime_error("无法替换存档，旧存档仍保留");
#else
    std::filesystem::rename(temporary, path);
#endif
}

void SaveManager::load(Game &game, const std::string &filename)
{
    std::ifstream in(std::filesystem::u8path(filename));
    if (!in)
        throw std::runtime_error("无法打开存档文件");
    std::string tag, name;
    int version;
    if (!(in >> tag >> version) || tag != "BIG_FISH_SAVE" || version != 1)
        throw std::runtime_error("存档标识或版本不正确");
    double width, height;
    if (!(in >> tag >> width >> height >> std::quoted(name)) || tag != "WORLD")
        throw std::runtime_error("鱼缸数据不完整");
    Game candidate;
    candidate.world_ = Aquarium(name, width, height);
    int phase;
    long long seed;
    if (!(in >> tag >> phase >> candidate.elapsed_ >> seed >> candidate.targetScore_) ||
        tag != "GAME" || phase < 0 || phase > 4 || !std::isfinite(candidate.elapsed_) ||
        candidate.elapsed_ < 0 || candidate.elapsed_ > 1000000 || seed < 0 ||
        seed > std::numeric_limits<unsigned>::max() || candidate.targetScore_ != 3)
        throw std::runtime_error("游戏状态不合法");
    candidate.phase_ = static_cast<GamePhase>(phase);
    candidate.seed_ = static_cast<unsigned>(seed);
    int count;
    if (!(in >> tag >> count) || tag != "FISHES" || count < 1 || count > 200)
        throw std::runtime_error("鱼的数量不合法");
    int players = 0;
    for (int i = 0; i < count; ++i)
    {
        char type;
        int id, energy, eaten;
        double x, y, radius;
        if (!(in >> type >> id >> std::quoted(name) >> x >> y >> radius >> energy >> eaten))
            throw std::runtime_error("鱼对象数据缺失");
        if (type == 'P')
        {
            double tx, ty, clock;
            if (!(in >> tx >> ty >> clock) || id != 1 || ++players > 1)
                throw std::runtime_error("玩家鱼数据不合法");
            auto player = std::make_unique<PlayerFish>(id, name, x, y, radius, energy, eaten);
            if (tx < 0 || tx > width || ty < 0 || ty > height)
                throw std::runtime_error("玩家目标位置越界");
            player->setTarget(tx, ty);
            player->restoreHungerClock(clock);
            candidate.world_.addFish(std::move(player));
        }
        else if (type == 'E')
        {
            double vx, vy;
            if (!(in >> vx >> vy) || id == 1)
                throw std::runtime_error("敌鱼数据不合法");
            candidate.world_.addFish(
                std::make_unique<EnemyFish>(id, name, x, y, radius, energy, vx, vy, eaten));
        }
        else
            throw std::runtime_error("存档包含未知鱼类型");
    }
    if (players != 1 || !(in >> tag) || tag != "END" || (in >> tag))
        throw std::runtime_error("存档缺少玩家或结束标记，或含有多余数据");
    const auto &player = candidate.world_.fish(1);
    if ((candidate.phase_ == GamePhase::Lost && player.alive()) ||
        (candidate.phase_ != GamePhase::Lost && !player.alive()) ||
        (candidate.phase_ == GamePhase::Won && player.eatenCount() < 3) ||
        (candidate.phase_ != GamePhase::Won && candidate.phase_ != GamePhase::Lost &&
         player.eatenCount() >= 3))
        throw std::runtime_error("存档状态与玩家数据不一致");
    candidate.message_ = "已读取存档";
    // 完整读取并校验后才替换当前游戏，失败不会破坏正在进行的状态。
    game = std::move(candidate);
}
