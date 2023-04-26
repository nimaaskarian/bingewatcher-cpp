#ifndef BINGE_H
#define BINGE_H
#include <string>
#include <vector>

class Season 
{
  public:
    Season(int watched, int all, int index=-1);

    int watched{};
    int all{};
    int index{};

    enum status {
      SEASON_SUCCESS = 0,
      SEASON_ERROR_COMPLETED,
      SEASON_ERROR_EMPTY,
    };

    bool isCompleted();
    status addWatchedEpisode();
    status removeWatchedEpisode();
}; 

class Series 
{
  bool isChanged = true;
  Season &firstUncompleteSeason();
  std::string path;

  public:
    Series(std::string name="", int episodes=0, int seasons=0);

    enum status {
      SERIES_SUCCESS = 0,
      SERIES_ERROR_PATH_UNDEFINED,
      SERIES_ERROR_NOT_CHANGED,
      SERIES_ERROR_NO_EPISODE,
      SERIES_ERROR_NO_SEASON,
    };

    std::string name;
    std::vector<Season> seasons;
    bool isCompleted();

    int getAllEpisodes();
    int getWatchedEpisodes();

    void print(bool isExtended=false, bool isOnlyNextEpisode=false,int index=-1);

    void addWatchedEpisodes(int count);
    void removeWatchedEpisodes(int count);

    void addSeason(int episodes, int seasonIndex=-1);
    void removeSeason(int seasonIndex=-1);
    void editSeason(int episodes, int seasonIndex=-1);

    status loadFile(std::string path);
    status writeFile(std::string path="");
    status deleteFile();

};
#endif
