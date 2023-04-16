#include "iostream"
#include "binge.h"

BingeSeason::BingeSeason(int _watched, int _all, int _index) 
{
  all = _all;
  watched = _watched;
  index = _index;
}
int BingeSeason::getUnwatched(){
  return all - watched;
}

Binge::Binge(std::string _name, int _seasons, int _episodes){
  // don't proceed if theres no name, season length or episodes
  if (!_name.length() || !_seasons || ! _episodes) return;

  // set binge name to input name
  name = _name;
  for (int i{}; i < _seasons; i++){
    seasons.push_back(BingeSeason(0,_episodes,i));
  }

}
Binge::status Binge::load(std::string _path) {

  // setting name and path
  path = _path;
  name = path.substr(path.find_last_of("/\\") + 1);

  // defining variables
  int watched{}, all{};
  std::ifstream bingeFile(path);
  if (!bingeFile.good()) return BINGE_ERROR_FILE;

  // variables for iterating the file:
  // line for string of current line 
  // streamstream to stream line into int variables
  // index for obvious reasons
  std::string line{};
  std::stringstream lineStream;
  int index{};

  while (getline (bingeFile, line))
  {
    // remove the %:% between the watched and all
    line.erase(std::remove_if(line.begin(), line.end(), [](unsigned char c){ return !std::isdigit(c) && c !='+'; }), line.end());

    lineStream.clear();
    lineStream.str(line);
    lineStream >> watched >> all;
    

    // if no episodes exist, throw a error
    if (!all) return BINGE_ERROR_EPISODE;

    seasons.push_back(BingeSeason(watched, all, index));
    /* std::cout << "Watched: " << watched << ", All: " << all << '\n'; */
    index++;
  }
  if (index == 0) return BINGE_ERROR_SEASON;

  // closing the filestream
  bingeFile.close();
  return BINGE_SUCCESS;
}
