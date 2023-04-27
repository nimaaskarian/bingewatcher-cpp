#include "directory.h"
Directory::Directory(std::string path)
{
  if (path.length()) this->path = path;
   
  Directory::reload();
}

bool Directory::hasFile(std::string filename)
{
  // reload before checking if file exists so dir be updated
  // in case a newly file was created from the point of initialization
  Directory::reload();

  for (auto &path : pathsOfFiles){
    if (filename == basename(path)) return true;
  }
  return false;
}

std::string Directory::basename(std::string path)
{
  return path.substr(path.find_last_of("/\\") + 1);
}

std::string Directory::parentDirectoryPath(std::string path)
{
  return path.substr(0,path.find_last_of("/\\") + 1);
}

void Directory::reload()
{
  pathsOfFiles = std::vector<std::string>();

  // if doesn't exists, create.
  if (!std::filesystem::exists(path)) {
    std::filesystem::create_directories(path);
    // ofc when you create it, the dir is empty. theres nothing to list
    // we quit the function then!
    return;
  }
  for (const auto & entry : std::filesystem::directory_iterator(path)){
    pathsOfFiles.push_back(entry.path());
  }

}
