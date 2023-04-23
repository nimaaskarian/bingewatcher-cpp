#include "directory.h"
Directory::Directory(std::string pathToDir){
  if (pathToDir.length()) dirpath = pathToDir;
   
  Directory::reload();
}

bool Directory::hasFile(std::string filename){
  // reload before checking if file exists so dir be updated
  Directory::reload();

  for (auto &path : paths){
    if (filename == basename(path)) return true;
  }
  return false;
}

std::string Directory::basename(std::string path){
  return path.substr(path.find_last_of("/\\") + 1);
}

void Directory::reload(){
  paths = std::vector<std::string>();

  // if doesn't exists, create.
  if (!std::filesystem::exists(dirpath)) {
    std::filesystem::create_directories(dirpath);
  }  
  for (const auto & entry : std::filesystem::directory_iterator(dirpath)){
    paths.push_back(entry.path());
  }

}
