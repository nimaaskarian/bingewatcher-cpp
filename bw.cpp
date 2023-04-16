#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>

class BingeSeason 
{
  public:
    int watched;
    int all;
    int index;
    BingeSeason(int w, int a, int i) {
      all = a;
      watched = w;
      index = i;
    }
    int getUnwatched(){
      return all - watched;
    }
}; 
class Binge 
{
  public:
    std::vector<BingeSeason> seasons;
    Binge(std::string path) {

      // defining variables
      std::string line{};
      int watched{}, all{};
      std::ifstream bingeFile(path);
      std::stringstream lineStream;
      int index{};

      // reading each line from the file
      while (getline (bingeFile, line))
      {
        // remove the %:% between the watched and all
        line.erase(std::remove_if(line.begin(), line.end(), [](unsigned char c){ return !std::isdigit(c) && c !='+'; }), line.end());

        lineStream.clear();
        lineStream.str(line);
        lineStream >> watched >> all;
        seasons.push_back(BingeSeason(watched, all, index));
        // std::cout << "Watched: " << watched << ", All: " << all << '\n';
        index++;
      }

      bingeFile.close();
    }
};
int main()
{

  Binge mybinge("/home/nima/.cache/bingewatcher/lost");
  std::cout << (mybinge.seasons.at(mybinge.seasons.size()-1).watched);
}
