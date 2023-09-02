#ifndef BINGEWATCHER_TV_SHOWS_H
#define BINGEWATCHER_TV_SHOWS_H
#include <string>
#include <vector>

// class Season 
// {
//   public:
//     Season(int watched, int all, int index=-1);

//     int watched{};
//     int all{};
//     int index{};

//     enum status {
//       SEASON_SUCCESS = 0,
//       SEASON_ERROR_COMPLETED,
//       SEASON_ERROR_EMPTY,
//     };

//     
//     status addWatchedEpisode();
//     status removeWatchedEpisode();
// }; 
class Season {
  public:
    int watched{}, all{}, index{};
    Season(int watched, int all, int index=-1);
    bool isCompleted();
};

class TvShow 
{
  bool isChanged = true;
  Season &firstUncompleteSeason();

  public:
    TvShow(std::string name="");

    std::string name;
    std::vector<Season> seasons;
    bool isCompleted();

    int getAllEpisodes();
    int getWatchedEpisodes();

    void print(int index=-1);
    void printExtended(int index=-1);
    void printNextEpisode();

    void addWatchedEpisodes(int count);
    void removeWatchedEpisodes(int count);

    void addSeason(int episodes, int seasonIndex=-1);
    void removeSeason(int seasonIndex=-1);
    void editSeason(int episodes, int seasonIndex=-1);

    void setName(std::string name);

    void loadFile(std::string path);
    void writeFile(std::string path="");
    void deleteFile();

};
#endif
