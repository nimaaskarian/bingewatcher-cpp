#include <algorithm>
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <filesystem>
#include "series.h"
#include "directory.h"

Season::Season(int watched, int all, int index)
{
  this->all = all;
  this->watched = watched;
  this->index = index;
}
Season::status Season::addWatchedEpisode()
{
  if (isCompleted()) return SEASON_ERROR_COMPLETED;
  watched++;
  return SEASON_SUCCESS;
}
Season::status Season::removeWatchedEpisode()
{
  if (!watched) return SEASON_ERROR_EMPTY;
  watched--;
  return SEASON_SUCCESS;
}
bool Season::isCompleted()
{
  return watched == all;
}

Season &Series::firstUncompleteSeason()
{
  for (Season &season : seasons){
    if (!season.isCompleted()) return season;
  }
  return seasons.at(seasons.size()-1);
}
Series::Series(std::string name,int episodes ,int seasons)
{
  // changed is true by default.
  // so we don't need to set it to true in constructor

  // don't proceed if theres no name
  if (!name.length()) return;

  // set series name to input name
  this->name = name;

  // don't proceed if theres no seasons or episodes
  if (!seasons || ! episodes) return;

  // for each season, add a season
  for (int i{}; i < seasons; i++){
    addSeason(episodes,i);
  }

}
Series::status Series::loadFile(std::string path) 
{
  isChanged = false;

  // setting name and path
  this->path = path;
  name = Directory::basename(path);

  // defining variables
  int watched{}, all{};
  std::ifstream seriesFile(path);
  if (!seriesFile.good()) return SERIES_ERROR_PATH_UNDEFINED;

  // variables for iterating the file:
  // line for string of current line 
  // streamstream to stream line into int variables
  // index for obvious reasons
  std::string fileLineContent{};
  std::stringstream fileLineStream;
  int fileLineIndex{};

  while (getline (seriesFile, fileLineContent))
  {
    // clear the lineStream
    fileLineStream.clear();
    fileLineStream.str(fileLineContent);
    fileLineStream >> watched >> all;
    

    // if no episodes exist, throw a error
    if (!all) return SERIES_ERROR_NO_EPISODE;

    seasons.push_back(Season(watched, all, fileLineIndex));
    /* std::cout << "Watched: " << watched << ", All: " << all << '\n'; */
    fileLineIndex++;
  }

  // if fileLineIndex is equal to zero, means theres no 
  // content in the file, or something happened during the
  // for loop
  if (fileLineIndex == 0) return SERIES_ERROR_NO_SEASON;

  // closing the filestream
  seriesFile.close();

  // returning success status
  return SERIES_SUCCESS;
}
int Series::getAllEpisodes()
{
  int sum{};
  for (Season &season : seasons){
    sum+=season.all;
  }
  return sum;
}
int Series::getWatchedEpisodes()
{
  int sum{};
  for (Season &season : seasons){
    sum+=season.watched;
  }
  return sum;
}
void Series::print(bool isExtended, bool isOnlyNextEpisode,int index)
{
  if (isOnlyNextEpisode) {
    // no next episode if show is finished ofc!
    if (isCompleted()) return;

    auto &season = firstUncompleteSeason();
    printf("S%02dE%02d",season.index+1,season.watched+1);
    return;
  }
  if (index != -1) std::cout << index+1 << ": ";
  int allWatched = getWatchedEpisodes();
  int all = getAllEpisodes();
  std::cout << name << ":\n";
  if (isExtended) {
    for (auto &season : seasons){
      if(season.isCompleted()) continue;
      std::cout << season.index+1 << ": " << season.watched << '/' << season.all <<'\n';
    }
    std::cout << "Episodes: " << all << '\n';
    std::cout << "Next Episode: ";
    print(false, true);
    std::cout << '\n';
    std::cout << "Progress: "<< 100.0f*allWatched/all << "%\n\n";
    return;
  }
  std::cout << all << " episodes, " << 100.0f*allWatched/all << "% watched, next is ";
  
  print(false, true);
  std::cout << "\n\n";

}
Series::status Series::writeFile(std::string path)
{
  if (!isChanged) return SERIES_ERROR_NOT_CHANGED;
  // fallback to default if path is not defined
  if (!path.length()) path = this->path;

  std::ofstream seriesFileStream(path);
  if (!seriesFileStream.good()) return SERIES_ERROR_PATH_UNDEFINED;

  int index{};
  unsigned long int end{seasons.size()};
  for (Season &season : seasons){
    seriesFileStream << season.watched << '+' << season.all;
    if (index-2 != end) seriesFileStream << '\n';
    index++;
  }

  seriesFileStream.close();
  std::cout << "Write completed in file '" << path << "'\n";
  return SERIES_SUCCESS;
}
bool Series::isCompleted()
{
  bool allTrue = true;
  for (Season &season : seasons){
    if (!allTrue) break;
    allTrue = season.isCompleted();
  }
  return allTrue;
}
void Series::addWatchedEpisodes(int count)
{
  if (isCompleted() || !count) return; 
  if (count < 0) return removeWatchedEpisodes(-count);

  for (int i{}; i < count; i++){
    for (Season &season : seasons){
      Season::status c = season.addWatchedEpisode();
      if (c == Season::SEASON_SUCCESS) {
        break;
      }
    }
  }
  isChanged = true;
  std::cout << "Added " << count <<  " episodes to " << name << '\n';
}
void Series::removeWatchedEpisodes(int count)
{
  if (!getAllEpisodes() || !count) return;
  if (count < 0) return addWatchedEpisodes(-count);

  for (int i{}; i < count; i++){
    for(std::vector<Season>::reverse_iterator season = seasons.rbegin(); season!=seasons.rend(); season++){
      Season::status c = season->removeWatchedEpisode();
      if (c == Season::SEASON_SUCCESS) break;
    }
  }
  isChanged = true;
  std::cout << "Removed " << count <<  " episodes from " << name << '\n';
}
Series::status Series::deleteFile()
{
  if (!std::filesystem::exists(path)) return SERIES_ERROR_PATH_UNDEFINED;
  std::filesystem::remove(path);

  std::cout << "File '" << path << "' deleted.\n";
  return SERIES_SUCCESS;
}

void Series::addSeason(int episodes, int seasonIndex){
  if (seasonIndex==-1) seasonIndex = seasons.size();

  Season newSeason(0, episodes,seasonIndex);
  seasons.push_back(newSeason);
}
void Series::setName(std::string name)
{
  if (!name.length()) return;
  this->name = name;

  if (!path.length()) return;
  // this->path
  path = Directory::parentDirectoryPath(path)+name;

  std::cout << path << '\n';
}
