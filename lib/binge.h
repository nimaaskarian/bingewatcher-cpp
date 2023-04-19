#ifndef BINGE_H
#define BINGE_H
#include <string>
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
  bool changed = true;
  BingeSeason &firstUncompleteSeason();
  public:
    enum status {
      BINGE_SUCCESS = 0,
      BINGE_ERROR_FILE,
      BINGE_ERROR_CHANGED,
      BINGE_ERROR_EPISODE,
      BINGE_ERROR_SEASON,
    };

    Binge(std::string name="", int episodes=0, int seasons=0);

    std::string name;
    std::string path;
    std::vector<BingeSeason> seasons;
    bool isCompleted();

    int getAll();
    int getAllWatched();

    void print(bool extended=false, bool nextEpisode=false,int index=-1);
    void add(int times);
    void remove(int times);

    status load(std::string path);
    status write(std::string path="");
    status deleteFile();

    void addSeason(int episodes, int seasonIndex=-1);
};
#endif
