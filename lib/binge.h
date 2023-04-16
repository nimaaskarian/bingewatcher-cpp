#ifndef BINGE_H
#define BINGE_H
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>

class BingeSeason 
{
  public:
    int watched{};
    int all{};
    int index{};
    BingeSeason(int watched, int all, int index=-1);
    int getUnwatched();
}; 

class Binge 
{
  public:
    enum status {
      BINGE_SUCCESS = 0,
      BINGE_ERROR_FILE,
      BINGE_ERROR_EPISODE,
      BINGE_ERROR_SEASON,
    };

    std::string name;
    std::string path;
    std::vector<BingeSeason> seasons;
    Binge(std::string name="", int seasons=0, int episodes=0);
    Binge::status load(std::string path);
};
#endif
