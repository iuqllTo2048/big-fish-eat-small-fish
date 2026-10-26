#pragma once
#include <opencv2/core.hpp>
#include <string>
class Game;

class OpenCVView
{
  public:
    cv::Mat render(const Game &game) const;
    void run(Game &game, const std::string &saveFile, int smokeFrames = 0);
    void writeEvidence(const std::string &directory) const;

  private:
    Game *game_ = nullptr;
    std::string saveFile_;
    std::string notice_;
    static void onMouse(int event, int x, int y, int flags, void *userdata);
    void act(char key);
};
