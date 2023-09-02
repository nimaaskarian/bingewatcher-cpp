#include <iostream>
#include <json/value.h>
#include <sstream>
#include <string>
#include <unistd.h>
#include <algorithm>
#include <regex>
#include <vector>

#include "lib/tv-shows.h"
#include "lib/directory.h"

#define DEFAULT_DIR "/.cache/bingewatcher"

typedef struct Args {
  std::vector<std::string> searchValues{},matchValues{};
  std::vector<TvShow> newTvShows{};
  std::string dir{DEFAULT_DIR};
  int addWatched{0}, removeWatched{0};
  bool finished{0}, extended{0}, unfinished{1},episode{0},deleteSeries{0},searchFlag{0},matchFlag{0};
} Args;

void errorSeriesExists(std::string seriesName);
Args parseArgs(int argc, char *argv[]);

int main(int argc, char* argv[])
{
  Args args = parseArgs(argc, argv);
  const std::string home = getenv("HOME");

  Directory defaultDirectory(home+args.dir);

  // vector array for all series
  std::vector<TvShow> allTvShows{};

  // selected indexes (of all series) to do stuff on them
  std::vector<int> selectedIndexesOfSeries{};

  if (defaultDirectory.hasFile(newSeriesName)){
    errorSeriesExists(newSeriesName);
    continue;
  }

  {
    // if number of non-option arguments is less than 1, print all
    bool printAll{argc-optind < 1};

    // if theres a svalue, disable print all
    if (args.searchValues.size() || args.matchValues.size()) printAll = false;
    int pathsOfFilesIndex{};

    // update directory (add newly added binges)
    defaultDirectory.reload();

    for (std::string &filePath : defaultDirectory.pathsOfFiles) {
      TvShow currentSerie{};
      TvShow::status loadStatus = currentSerie.loadFile(filePath);

      // if its not loaded successfully, continue
      // potential loop break here
      if (loadStatus != TvShow::SERIES_SUCCESS) continue;

      bool bingeCompleted = currentSerie.isCompleted();

      // -f = also finished
      if (bingeCompleted && !args.finished) continue;

      // -F = only finished
      if (args.finishedOnly && !bingeCompleted) continue;

      // print all if no opts and don't check for anything else
      if (printAll) 
      {
        if(args.extended) currentSerie.printExtended(pathsOfFilesIndex);
        else if(args.episode) currentSerie.printNextEpisode();
        else currentSerie.print(pathsOfFilesIndex);

      }
      // push all series to a vector array
      allTvShows.push_back(currentSerie);

      // if theres args.searchFlag, search for the args.searchValues inside binges.
      if (args.searchFlag){
        for (auto svalue : args.searchValues)
          if (currentSerie.name.find(svalue) != std::string::npos) {
            // select the indexes of strings pushed in svalue
            selectedIndexesOfSeries.push_back(pathsOfFilesIndex);
          }
      }
      if (args.matchFlag){
        for (auto Svalue : args.matchValues)
          if (currentSerie.name == Svalue) {
            // select the indexes of strings pushed in svalue
            selectedIndexesOfSeries.push_back(pathsOfFilesIndex);
          }
      }

      pathsOfFilesIndex++;
    }

    // no series = error message !
    if (!allTvShows.size()){
      std::cerr << "You have no Series! add one with bw -o '<name of your show>' or bw -n '<name of your show,<episodes>+<seasons>' \n";
      return 1;
    }

    // return if printAll is true. we don't need any other stuff
    if (printAll) return 0;
  }

  // you can select a series either by -s flag, or by number args.
  // series selection can be different when using -f or -F options.
  for (int optIndex{ optind }; optIndex < argc; ++optIndex) {
    try {
      // if user wants the first show, we print show 0
      // this line may produce std::invalid_argument
      int serieIndex = std::stoi(argv[optIndex])-1;

      // if serieIndex is valid, push it to selectedIndexes
      selectedIndexesOfSeries.push_back(serieIndex);

    } catch (std::invalid_argument) {
      std::cerr << "Non-option argument '" << argv[optIndex] << "'\n";
    } 
  }

  for (auto &selectedIndex : selectedIndexesOfSeries) {
    try {
      // this line may produce std::out_of_range
      TvShow &currentSerie = allTvShows.at(selectedIndex);

      // if a,r or d flag, print beforehand too
      if (args.addWatched || args.removeWatched || args.deleteSeries) {
        if (args.extended)
          currentSerie.printExtended(selectedIndex);
        if (args.episode)
          currentSerie.printNextEpisode();
        if (!args.episode && !args.extended)
          currentSerie.print(selectedIndex);
      }
      if (args.deleteSeries) {
        currentSerie.deleteFile();
        continue;
      }
      currentSerie.addWatchedEpisodes(args.addWatched);
      currentSerie.removeWatchedEpisodes(args.removeWatched);
      currentSerie.print(selectedIndex);
      currentSerie.writeFile();
    }
    catch (std::out_of_range) {
      std::cerr << "You don't show " << selectedIndex+1 << ". You have show 1 to " << allTvShows.size() <<".\n";
      return 1;
    }
  }
  return 0;
}

Args parseArgs(int argc, char *argv[])
{
  Args args;
  // int c, temp variable for arguments
  char currentOpt;

  // iterating over arguments and get a and d flags value
  while ((currentOpt = getopt(argc, argv, "a:r:fEFen:do:s:S:")) != -1) {
    switch (currentOpt) {
      case 'e':
        args.episode = true;
        break;
      case 's': {
        args.searchFlag = true;
        args.searchValues.push_back(optarg);
        break;
      }
      case 'S': {
        args.matchFlag = true;
        args.matchValues.push_back(optarg);
        break;
      }
      case 'o':{
      }
      case 'f':
        args.finished = true;
        break;
      case 'd':
        args.deleteSeries = true;
        break;
      case 'E':
        args.extended = true;
        break;
      case 'n': {
        std::string optargString{optarg};
        // "name,seasons*episodes"
        std::regex pattern("([^,]+),([^*]+)\\*(.+)");
        std::smatch matches;
        if (std::regex_match(optargString, matches, pattern)) {
          std::string name = matches[1];
          int seasons = std::stoi(matches[2]);
          int episodes = std::stoi(matches[3]);
          std::cout << "HELLO\n";

          // error if exists
          if (defaultDirectory.hasFile(name)){
            errorSeriesExists(name);
            continue;
          }
          args.newTvShows.push_back(TvShow(name, episodes, seasons));
        }
        break;
      }
      case 'F':
        args.unfinished = false;
        args.finished = true;
        break;
      case 'a':
        args.addWatched = atoi(optarg);
        break;
      case 'r':
        args.removeWatched = atoi(optarg);
        break;
      default:
        break;
    }
  }
  return args;
}

// print that a show exists, doesn't actually check if it exists
void errorSeriesExists(std::string seriesName)
{
  std::cerr << "You have the show '" << seriesName << "' in your files.\n";
}
