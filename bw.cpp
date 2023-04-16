#include <iostream>
#include "lib/binge.h"
#include "lib/directory.h"
#define DEFAULT_DIR "/.cache/bingewatcher"

int main()
{
  const std::string home = getenv("HOME");
  Directory myfiles(home+DEFAULT_DIR);
  for (std::string &path : myfiles.paths) {
    Binge mybinge{};
    Binge::status c = mybinge.load(path);
    if (c == Binge::BINGE_SUCCESS){
      std::cout << "Series: " << mybinge.name << '\n';
      for (auto &season : mybinge.seasons) {
        std::cout << "Season: " << season.index+1 << '\n';
        std::cout << season.watched << '/'<< season.all << '\n';
      }
    }
  }

}
