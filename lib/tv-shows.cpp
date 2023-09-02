#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <sstream>
#include <fstream>
#include <filesystem>
#include "tv-shows.h"
#include "directory.h"

Season::Season(int watched, int all, int index)
{
  this->all = all;
  this->watched = watched;
  this->index = index;
}

Season &TvShow::firstUncompleteSeason()
{
  for (Season &season : seasons){
    if (!season.isCompleted()) return season;
  }
  return seasons.at(seasons.size()-1);
}
TvShow::TvShow(std::string name)
{
  // don't proceed if theres no name
  if (!name.length()) 
    return;

  this->name = name;
}
void TvShow::loadFile(std::string path) 
{
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
int TvShow::getAllEpisodes()
{
  int sum{};
  for (Season &season : seasons){
    sum+=season.all;
  }
  return sum;
}
int TvShow::getWatchedEpisodes()
{
  int sum{};
  for (Season &season : seasons){
    sum+=season.watched;
  }
  return sum;
}
void TvShow::print(int index)
{
  if (index != -1) std::cout << index+1 << ": ";
  std::cout << name << ":\n";
  std::cout << getAllEpisodes() << " episodes, " << 100.0f*getAllEpisodes()/getWatchedEpisodes() << "% watched, next is ";
  
  printNextEpisode();
  std::cout << "\n\n";

}
void TvShow::printExtended(int index)
{
  if (index != -1) std::cout << index+1 << ": ";
  std::cout << name << ":\n";
  for (auto &season : seasons){
    if(season.isCompleted()) continue;
    std::cout << season.index+1 << ": " << season.watched << '/' << season.all <<'\n';
  }
  std::cout << "Episodes: " << getAllEpisodes() << '\n';
  std::cout << "Next Episode: ";
  printNextEpisode();
  std::cout << '\n';
  std::cout << "Progress: "<< 100.0f*getWatchedEpisodes()/getAllEpisodes() << "%\n\n";
}
void TvShow::printNextEpisode()
{
  // no next episode if show is finished ofc!
  if (isCompleted()) return;

  auto &season = firstUncompleteSeason();
  printf("S%02dE%02d",season.index+1,season.watched+1);
}
void TvShow::writeFile(std::string path)
{
  std::ofstream seriesFileStream(path);

  if (!seriesFileStream.good())
    throw std::invalid_argument("Tv Show path is not good.");

  int index{};
  unsigned long int end{seasons.size()};
  for (Season &season : seasons){
    seriesFileStream << season.watched << '+' << season.all;
    if (index-2 != end) seriesFileStream << '\n';
    index++;
  }

  seriesFileStream.close();
  std::cout << "Write completed in file '" << path << "'\n";
}
bool TvShow::isCompleted()
{
  bool allTrue = true;
  for (Season &season : seasons){
    if (!allTrue) break;
    allTrue = season.isCompleted();
  }
  return allTrue;
}
void TvShow::addWatchedEpisodes(int count)
{
  if (isCompleted() || !count) return; 
  if (count < 0) return removeWatchedEpisodes(-count);

  for (int i{}; i < count; i++){
    for (Season &season : seasons){
      if (!season.isCompleted()) {
        season.watched++;
        break;
      }
    }
  }
  std::cout << "Added " << count <<  " episodes to " << name << '\n';
}
void TvShow::removeWatchedEpisodes(int count)
{
  if (!getAllEpisodes() || !count) return;
  if (count < 0) return addWatchedEpisodes(-count);

  for (int i{}; i < count; i++){
    for(std::vector<Season>::reverse_iterator season = seasons.rbegin(); season!=seasons.rend(); season++){
      if (season->watched != 0) {
        season->watched--;
        break;
      }
    }
  }
  std::cout << "Removed " << count <<  " episodes from " << name << '\n';
}
void TvShow::deleteFile()
{
  if (!std::filesystem::exists(path)) return SERIES_ERROR_PATH_UNDEFINED;
  std::filesystem::remove(path);

  std::cout << "File '" << path << "' deleted.\n";
  return SERIES_SUCCESS;
}

void TvShow::addSeason(int episodes, int seasonIndex){
  if (seasonIndex==-1) seasonIndex = seasons.size();

  Season newSeason(0, episodes,seasonIndex);
  seasons.push_back(newSeason);
}
void TvShow::setName(std::string name)
{
  if (!name.length()) return;
  this->name = name;

  if (!path.length()) return;
  // this->path
  path = Directory::parentDirectoryPath(path)+name;

  std::cout << path << '\n';
}
