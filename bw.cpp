#include <iostream>
#include <sstream>
#include <string>
#include "lib/binge.h"
#include "lib/directory.h"
#include <unistd.h>
#define DEFAULT_DIR "/.cache/bingewatcher"

int main(int argc, char* argv[])
{
  // argc used to calculate if program should print all or not
  int printArgc{argc-optind};

  // int variables for flag values.
  int avalue{}, dvalue{};
  // bool variables for flag existence.
  bool fflag{}, Lflag{}, Fflag{},eflag{};
  {
    
    // int c, temp variable for arguments
    char c;
    // iterating over arguments and get a and d flags value
    while ((c = getopt(argc, argv, "a:d:fLFe")) != -1) {
      switch (c) {
        case 'e':
          eflag = true;
          break;
        case 'f':
          fflag = true;
          break;
        case 'L':
          Lflag = true;
          break;
        case 'F':
          Fflag = true;
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
  }
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

        bool bingeCompleted = currentBinge.isCompleted();
        // -f = also finished
        if (bingeCompleted && !fflag) continue;
        // -F = only finished
        if (Fflag && !bingeCompleted) continue;
        // no args = listing all!
        if (printArgc < 2) currentBinge.print(i,Lflag,eflag);
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
      if (avalue || dvalue) binge.print(bingeIndex, Lflag,eflag);
      binge.add(avalue);
      binge.remove(dvalue);
      binge.print(bingeIndex, Lflag,eflag);
      binge.write();

    } catch (std::invalid_argument) {
      std::cout << "Non-option argument '" << argv[index] << "'\n";
    } catch (std::out_of_range) {
      std::cout << "You don't show " << argv[index] << ". You have show 1 to " << binges.size() <<".\n";
      return 1;
    }
}
