#include <iostream>
#include <sstream>
#include <string>
#include "lib/binge.h"
#include "lib/directory.h"
#include <unistd.h>
#define DEFAULT_DIR "/.cache/bingewatcher"

int main(int argc, char* argv[])
{
  // iterating over arguments and get a and d flags value
  int c;
  int avalue{}, dvalue{};
  bool fflag{};
  while ((c = getopt(argc, argv, "a:d:f")) != -1) {
    switch (c) {
      case 'f':
        fflag = true;
        break;
      case 'a':
        avalue = atoi(optarg);
        break;
      case 'd':
        dvalue = atoi(optarg);
        break;
      default:
        break;
        // abort ();
    }
  }
  const bool printAll = argc < 2 || (argc == 2 && fflag);

  const std::string home = getenv("HOME");
  std::vector<Binge> binges{};

  Directory myfiles(home+DEFAULT_DIR);
  {
    int i{};
    for (std::string &path : myfiles.paths) {
      Binge currentBinge{};
      Binge::status c = currentBinge.load(path);

      // fflag means show finished shows aswell.
      if (c == Binge::BINGE_SUCCESS){
        // std::cout << (currentBinge.isCompleted()) << ',' << !fflag << '\n';

        if (currentBinge.isCompleted() && !fflag) continue;
        // no args = listing all!
        if (printAll) currentBinge.print(i);
        binges.push_back(currentBinge);
        i++;
      }
    }
  }


  for (int index = optind; index < argc; index++)
    try {
      // if user wants the first show, we print show 0
      int bingeIndex = std::stoi(argv[index])-1;
      Binge binge = binges.at(bingeIndex);

      // if theres a d or a value print beforehand too
      if (avalue || dvalue) binge.print(bingeIndex);
      binge.add(avalue);
      binge.remove(dvalue);
      binge.print(bingeIndex);
      binge.write();

    } catch (std::invalid_argument) {
      std::cout << "Non-option argument '" << argv[index] << "'\n";
    } catch (std::out_of_range) {
      std::cout << "You don't have " << argv[index] << " shows. You have " << binges.size() <<".\n";
      return 1;
    }
}
