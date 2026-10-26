#define NOMINMAX
#include "OpenCVView.h"
#include "Game.h"
#include "SaveManager.h"
#include "MathUtils.h"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <windows.h>
#include <chrono>
#include <cmath>
#include <cstring>
#include <filesystem>
#include <sstream>
#include <vector>

namespace
{
const int CanvasWidth = 1000;
const int HeaderHeight = 140;
const int WaterHeight = 600;
const char *WindowName = "大鱼吃小鱼";

std::wstring widen(const std::string &text)
{
    if (text.empty())
        return {};
    const int size = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, nullptr, 0);
    std::wstring result(size, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, result.data(), size);
    result.pop_back();
    return result;
}

struct TextLine
{
    std::wstring text;
    int x, y, size;
    COLORREF color;
};

// OpenCV内置字体不能显示中文，使用Windows字体绘制文字，再交回OpenCV显示。
void drawChinese(cv::Mat &image, const std::vector<TextLine> &lines)
{
    cv::Mat bgra;
    cv::cvtColor(image, bgra, cv::COLOR_BGR2BGRA);
    BITMAPINFO info{};
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = image.cols;
    info.bmiHeader.biHeight = -image.rows;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    HDC dc = CreateCompatibleDC(nullptr);
    void *pixels = nullptr;
    HBITMAP bitmap = CreateDIBSection(dc, &info, DIB_RGB_COLORS, &pixels, nullptr, 0);
    if (!dc || !bitmap)
    {
        if (bitmap)
            DeleteObject(bitmap);
        if (dc)
            DeleteDC(dc);
        throw std::runtime_error("中文文字绘制失败");
    }
    HGDIOBJ oldBitmap = SelectObject(dc, bitmap);
    std::memcpy(pixels, bgra.data, bgra.total() * bgra.elemSize());
    SetBkMode(dc, TRANSPARENT);
    for (const auto &line : lines)
    {
        HFONT font = CreateFontW(-line.size, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                 DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                 ANTIALIASED_QUALITY, DEFAULT_PITCH, L"Microsoft YaHei");
        HGDIOBJ oldFont = SelectObject(dc, font);
        SetTextColor(dc, line.color);
        TextOutW(dc, line.x, line.y, line.text.c_str(), static_cast<int>(line.text.size()));
        SelectObject(dc, oldFont);
        DeleteObject(font);
    }
    GdiFlush();
    std::memcpy(bgra.data, pixels, bgra.total() * bgra.elemSize());
    SelectObject(dc, oldBitmap);
    DeleteObject(bitmap);
    DeleteDC(dc);
    cv::cvtColor(bgra, image, cv::COLOR_BGRA2BGR);
}
} // namespace

cv::Mat OpenCVView::render(const Game &game) const
{
    cv::Mat frame(HeaderHeight + WaterHeight, CanvasWidth, CV_8UC3, cv::Scalar(28, 23, 16));
    for (int y = HeaderHeight; y < frame.rows; ++y)
    {
        const double t = double(y - HeaderHeight) / WaterHeight;
        cv::line(frame, {0, y}, {CanvasWidth, y}, cv::Scalar(80 - t * 35, 63 - t * 30, 24 - t * 8));
    }
    for (int i = 0; i < 18; ++i)
    {
        const int x = (i * 137 + 51) % 990;
        const int y = HeaderHeight + (i * 97 + 40) % 580;
        cv::circle(frame, {x, y}, 2 + i % 4, cv::Scalar(95, 80, 40), 1, cv::LINE_AA);
    }
    for (int i = 0; i < 4; ++i)
        cv::rectangle(frame, cv::Rect(24 + i * 125, 53, 113, 34), cv::Scalar(72, 60, 37),
                      cv::FILLED);
    const Aquarium &tank = game.aquarium();
    for (std::size_t i = 0; i < tank.size(); ++i)
    {
        const Fish &fish = tank.fishAt(i);
        if (!fish.alive())
            continue;
        const int x = static_cast<int>(fish.x() / tank.width() * CanvasWidth);
        const int y = HeaderHeight + static_cast<int>(fish.y() / tank.height() * WaterHeight);
        const int r = std::max(5, static_cast<int>(fish.radius() / tank.width() * CanvasWidth));
        const bool player = fish.typeCode() == 'P';
        const bool dangerous = fish.radius() >= tank.fish(1).radius();
        const cv::Scalar color = player      ? cv::Scalar(95, 214, 239)
                                 : dangerous ? cv::Scalar(90, 104, 222)
                                             : cv::Scalar(191, 188, 91);
        std::vector<cv::Point> tail = {
            {x - r + 3, y}, {x - r - r / 2, y - r / 2}, {x - r - r / 2, y + r / 2}};
        cv::fillConvexPoly(frame, tail, color, cv::LINE_AA);
        cv::ellipse(frame, {x, y}, {r, std::max(4, r * 2 / 3)}, 0, 0, 360, color, cv::FILLED,
                    cv::LINE_AA);
        cv::circle(frame, {x + r / 2, y - r / 6}, std::max(2, r / 8), cv::Scalar(22, 26, 35),
                   cv::FILLED, cv::LINE_AA);
        if (player)
            cv::ellipse(frame, {x, y}, {r + 4, r * 2 / 3 + 4}, 0, 0, 360, cv::Scalar(225, 245, 250),
                        2, cv::LINE_AA);
    }
    const auto &player = tank.fish(1);
    cv::rectangle(frame, {650, 57, 210, 20}, cv::Scalar(52, 48, 36), cv::FILLED);
    cv::rectangle(frame, {650, 57, player.energy() * 210 / 100, 20}, cv::Scalar(146, 209, 121),
                  cv::FILLED);
    std::ostringstream status;
    status << game.phaseText() << "    捕食 " << game.score() << '/' << game.targetScore()
           << "    能量 " << player.energy() << "    时间 " << static_cast<int>(game.elapsed())
           << "秒";
    std::vector<TextLine> lines = {
        {L"大鱼吃小鱼", 24, 14, 27, RGB(236, 244, 245)},
        {widen(status.str()), 235, 20, 18, RGB(199, 219, 222)},
        {L"开始 / 暂停", 32, 60, 18, RGB(233, 240, 239)},
        {L"重新开始", 164, 60, 18, RGB(233, 240, 239)},
        {L"保存进度", 289, 60, 18, RGB(233, 240, 239)},
        {L"读取进度", 414, 60, 18, RGB(233, 240, 239)},
        {L"能量", 601, 58, 18, RGB(210, 227, 222)},
        {L"鼠标移动 / WASD方向   空格开始或暂停   K保存  L读取  R重开  Q退出", 24, 101, 17,
         RGB(173, 199, 199)},
        {widen(notice_.empty() ? game.message() : notice_), 25, HeaderHeight + WaterHeight - 32, 18,
         RGB(216, 232, 229)}};
    if (game.phase() != GamePhase::Running)
    {
        cv::Mat shade = frame.clone();
        cv::rectangle(shade, {240, 320, 520, 155}, cv::Scalar(25, 30, 24), cv::FILLED);
        cv::addWeighted(shade, 0.9, frame, 0.1, 0, frame);
        lines.push_back({widen(game.phaseText()), 410, 346, 32, RGB(244, 238, 204)});
        const std::wstring hint = game.phase() == GamePhase::Won || game.phase() == GamePhase::Lost
                                      ? L"按 R 或点击重新开始，再玩一局"
                                      : L"移动鼠标控制玩家鱼，按空格开始或继续";
        lines.push_back({hint, 310, 408, 20, RGB(209, 225, 219)});
    }
    drawChinese(frame, lines);
    return frame;
}

void OpenCVView::act(char key)
{
    try
    {
        notice_.clear();
        if (key == ' ' || key == 'p')
            game_->togglePause();
        else if (key == 'r')
            game_->restart();
        else if (key == 'k')
        {
            SaveManager::save(*game_, saveFile_);
            notice_ = "进度已保存";
        }
        else if (key == 'l')
        {
            SaveManager::load(*game_, saveFile_);
            notice_ = "进度已读取";
        }
        else if (key == 'w' || key == 'a' || key == 's' || key == 'd')
        {
            const auto &fish = game_->aquarium().fish(1);
            game_->moveTarget(fish.x() + (key == 'a'   ? -8
                                          : key == 'd' ? 8
                                                       : 0),
                              fish.y() + (key == 'w'   ? -8
                                          : key == 's' ? 8
                                                       : 0));
        }
    }
    catch (const std::exception &error)
    {
        notice_ = error.what();
    }
}

void OpenCVView::onMouse(int event, int x, int y, int, void *userdata)
{
    auto &view = *static_cast<OpenCVView *>(userdata);
    if (event == cv::EVENT_LBUTTONDOWN && y >= 53 && y <= 87 && x >= 24 && x < 512)
    {
        const int button = (x - 24) / 125;
        const char keys[] = {' ', 'r', 'k', 'l'};
        if (button >= 0 && button < 4)
            view.act(keys[button]);
    }
    if (event == cv::EVENT_MOUSEMOVE && y >= HeaderHeight)
    {
        view.game_->moveTarget(double(x) / CanvasWidth * view.game_->aquarium().width(),
                               double(y - HeaderHeight) / WaterHeight *
                                   view.game_->aquarium().height());
    }
}

void OpenCVView::run(Game &game, const std::string &saveFile, int smokeFrames)
{
    game_ = &game;
    saveFile_ = saveFile;
    cv::namedWindow(WindowName, cv::WINDOW_AUTOSIZE);
    cv::setMouseCallback(WindowName, onMouse, this);
    auto previous = std::chrono::steady_clock::now();
    int frames = 0;
    while (true)
    {
        const auto now = std::chrono::steady_clock::now();
        const double dt =
            limitValue(std::chrono::duration<double>(now - previous).count(), 0.001, 0.1);
        previous = now;
        game.step(dt);
        cv::imshow(WindowName, render(game));
        const int key = cv::waitKey(16);
        if (key == 'q' || key == 'Q' || key == 27)
            break;
        if (key >= 0)
            act(static_cast<char>(key >= 'A' && key <= 'Z' ? key + 32 : key));
        if (cv::getWindowProperty(WindowName, cv::WND_PROP_VISIBLE) < 1)
            break;
        if (smokeFrames > 0)
        {
            ++frames;
            if (frames == 5 || frames == 20 || frames == 35)
                act(' ');
            if (frames == 15)
                act('k');
            if (frames == 25)
                act('l');
            if (frames == 30)
                act('r');
            if (frames >= smokeFrames)
                break;
        }
    }
    cv::destroyAllWindows();
    game_ = nullptr;
}

void OpenCVView::writeEvidence(const std::string &directory) const
{
    std::filesystem::create_directories(directory);
    auto capture = [&](const Game &game, const char *name)
    {
        if (!cv::imwrite(directory + "/" + name + ".png", render(game)))
            throw std::runtime_error("运行界面保存失败");
    };
    Game game;
    capture(game, "ready");
    game.start();
    game.moveTarget(30, 30);
    game.step(0.5);
    capture(game, "running");
    game.togglePause();
    capture(game, "paused");
    SaveManager::save(game, directory + "/preview_save.dat");
    game.restart();
    SaveManager::load(game, directory + "/preview_save.dat");
    capture(game, "loaded");
    game.restart();
    game.start();
    for (int id = 2; id <= 4; ++id)
    {
        game.editFish(id, "演示小鱼", 20, 30, 2, 100);
        game.eat(1, id);
    }
    capture(game, "won");
    game.restart();
    game.start();
    game.editFish(1, "玩家鱼", 20, 30, 8, 0);
    capture(game, "lost");
}
