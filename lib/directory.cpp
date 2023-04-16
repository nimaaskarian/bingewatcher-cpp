#include "directory.h"
Directory::Directory(std::string pathToDir){
  for (const auto & entry : std::filesystem::directory_iterator(pathToDir)){
    paths.push_back(entry.path());
  }

}

