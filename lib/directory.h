#ifndef FILE_H
#define FILE_H
#include <fstream>
#include <vector>
#include <iostream>
#include <filesystem>

class Directory 
{
  std::string dirpath{};
  public:
    std::vector<std::string> paths{};
    Directory(std::string pathToDir);
    bool hasFile(std::string filename);
    void reload();

    static std::string basename(std::string path);
};
#endif
