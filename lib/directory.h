#ifndef FILE_H
#define FILE_H
#include <fstream>
#include <vector>
#include <iostream>
#include <filesystem>

class Directory 
{
  std::string path{};
  public:
    std::vector<std::string> pathsOfFiles{};
    Directory(std::string pathToDir);
    bool hasFile(std::string filename);
    void reload();

    static std::string basename(std::string path);
    static std::string parentDirectoryPath(std::string path);
};
#endif
