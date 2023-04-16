#include <iostream>
#include "lib/binge.h"
#include "lib/directory.h"
#define DEFAULT_DIR "/.cache/bingewatcher"

int main()
{
  const std::string home = getenv("HOME");
  std::vector<Binge> binges{};

  Directory myfiles(home+DEFAULT_DIR);
  for (std::string &path : myfiles.paths) {
    Binge currentBinge{};
    Binge::status c = currentBinge.load(path);
    if (c == Binge::BINGE_SUCCESS && !currentBinge.isCompleted()){
      currentBinge.print();
      binges.push_back(currentBinge);
    }
  }
  // for testing 
  int inputIndex{},inputAdd;
  std::cin >> inputIndex;
  Binge binge = binges.at(inputIndex);
  binge.print();
  std::cin >> inputAdd;

  binge.add(inputAdd);
  binge.write();
  binge.print();

}
