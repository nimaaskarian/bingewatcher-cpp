#include "directory.h"
Directory::Directory(std::string pathToDir){
  for (const auto & entry : std::filesystem::directory_iterator(pathToDir)){
    paths.push_back(entry.path());
  }
}

bool Directory::hasFile(std::string filename){
  for (auto &path : paths){
    if (filename == basename(path)) return true;
  }
  return false;
}

std::string Directory::basename(std::string path){
  return path.substr(path.find_last_of("/\\") + 1);
}
