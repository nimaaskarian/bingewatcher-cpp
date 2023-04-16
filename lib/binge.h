#ifndef BINGE_H
#define BINGE_H
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>

class BingeSeason 
{
  public:
    enum status {
      BS_SUCCESS = 0,
      BS_ERROR_COMPLETED,
      BS_ERROR_EMPTY,
    };
    int watched{};
    int all{};
    int index{};
    BingeSeason(int watched, int all, int index=-1);
    status add();
    status remove();
    bool isCompleted();
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

    Binge(std::string name="", int seasons=0, int episodes=0);

    std::string name;
    std::string path;
    std::vector<BingeSeason> seasons;
    bool isCompleted();

    int getAll();
    int getAllWatched();

    void print(bool extended=false);
    void add(int times);
    void remove(int times);

    status load(std::string path);
    status write(std::string path="");
};
#endif
