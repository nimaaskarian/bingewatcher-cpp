#include "iostream"
#include "binge.h"

BingeSeason::BingeSeason(int _watched, int _all, int _index) 
{
  all = _all;
  watched = _watched;
  index = _index;
}
BingeSeason::status BingeSeason::add(){
  if (isCompleted()) return BS_ERROR_COMPLETED;
  watched++;
  return BS_SUCCESS;
}
BingeSeason::status BingeSeason::remove(){
  if (!watched) return BS_ERROR_EMPTY;
  watched--;
  return BS_SUCCESS;
}
bool BingeSeason::isCompleted(){
  return watched >= all;
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
  changed = false;

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
int Binge::getAll(){
  int sum{};
  for (BingeSeason &season : seasons){
    sum+=season.all;
  }
  return sum;
}
int Binge::getAllWatched(){
  int sum{};
  for (BingeSeason &season : seasons){
    sum+=season.watched;
  }
  return sum;
}
void Binge::print(int index,bool extended){
  if (index != -1) std::cout << index+1 << ": ";
  int allWatched = getAllWatched();
  int all = getAll();
  std::cout << name << ":\n";
  if (!extended) {
    std::cout << all << " episodes, " << 100.0f*allWatched/all << "% watched, " << allWatched << "\n\n";
  }
  else {
    for (BingeSeason &season : seasons){
      std::cout << season.index+1 << ": " << season.watched << '/' << season.all <<'\n';
    }
    std::cout << "Episodes: " << all << '\n';
    std::cout << "Progress: "<< 100.0f*allWatched/all << "%\n\n";
  }
}
Binge::status Binge::write(std::string _path){
  if (!changed) return BINGE_ERROR_CHANGED;
  // fallback to default if path is not defined
  if (!_path.length()) _path = path;

  std::ofstream bingeFile(_path);
  if (!bingeFile.good()) return BINGE_ERROR_FILE;

  int index{};
  unsigned long int end{seasons.size()};
  for (BingeSeason &season : seasons){
    bingeFile << season.watched << '+' << season.all;
    if (index-2 != end) bingeFile << '\n';
    index++;
  }

  bingeFile.close();
  std::cout << "Write completed in file '" << _path << "'\n";
  return BINGE_SUCCESS;
}
bool Binge::isCompleted(){
  bool allTrue = true;
  for (BingeSeason &season : seasons){
    if (!allTrue) break;
    allTrue = season.isCompleted();
  }
  return allTrue;
}
void Binge::add(int times){
  if (isCompleted() || !times) return; 
  if (times < 0) return remove(-times);

  for (int i{}; i < times; i++){
    for (BingeSeason &season : seasons){
      BingeSeason::status c = season.add();
      if (c == BingeSeason::BS_SUCCESS) {
        break;
      }
    }
  }
  changed = true;
  std::cout << "Added " << times <<  " episodes to " << name << '\n';
}
void Binge::remove(int times){
  if (!getAll() || !times) return;
  if (times < 0) return add(-times);

  for (int i{}; i < times; i++){
    for(std::vector<BingeSeason>::reverse_iterator season = seasons.rbegin(); season!=seasons.rend(); season++){
      BingeSeason::status c = season->remove();
      if (c == BingeSeason::BS_SUCCESS) break;
    }
  }
  changed = true;
  std::cout << "Removed " << times <<  " episodes from " << name << '\n';
}
