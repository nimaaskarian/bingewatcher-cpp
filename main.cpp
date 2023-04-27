#include <iostream>
#include <json/value.h>
#include <sstream>
#include <string>
#include <unistd.h>
#include <algorithm>

#include <json/json.h>
#include <curl/curl.h>
#include <vector>

#include "lib/series.h"
#include "lib/directory.h"

#define DEFAULT_DIR "/.cache/bingewatcher"

// trim from start (in place with reference)
static inline void trimStart(std::string &str) {
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// write function for curl, increases size of string
size_t writeFunction(void* ptr, size_t size, size_t nmemb, std::string* data) {
    data->append((char*)ptr, size * nmemb);
    return size * nmemb;
}

// print that a show exists, doesn't actually check if it exists
void errorSeriesExists(std::string seriesName){
  std::cerr << "You have the show '" << seriesName << "' in your files.\n";
}

int main(int argc, char* argv[])
{
  const std::string home = getenv("HOME");

  Directory defaultDirectory(home+DEFAULT_DIR);

  // int variables for flag values.
  int avalue{}, rvalue{};

  // svalue for search flag value
  std::vector<std::string> svalues{};

  // bool variables for flag existence.
  bool fflag{}, Eflag{}, Fflag{},eflag{},dflag{},sflag{};
  {
    // int c, temp variable for arguments
    char currentOpt;

    // iterating over arguments and get a and d flags value
    while ((currentOpt = getopt(argc, argv, "a:r:fEFen:do:s:")) != -1) {
      switch (currentOpt) {
        case 'e':
          eflag = true;
          break;
        case 's': {
          sflag = true;
          svalues.push_back(optarg);
          break;
        }
        case 'o':{
          std::string data ;
          std::string newBingeName = optarg;

          // printBingeExists and skip if binge exists
          if (defaultDirectory.hasFile(newBingeName)){
            errorSeriesExists(newBingeName);
            continue;
          }
          std::string url {"https://www.episodate.com/api/show-details?q="+newBingeName};
          // convert spaces to '-' so url be gud
          for (char &ch : url){
            if (ch==' ') ch = '-';
          }

          // curl handler object
          CURL *curlHandle = curl_easy_init();
          if (curlHandle){
            auto res = curl_easy_setopt(curlHandle, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, writeFunction);
            curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &data);
            curl_easy_perform(curlHandle);
          }
          // cleanup curl
          curl_easy_cleanup(curlHandle);

          // initial json root and reader
          Json::Value root{};
          Json::Reader reader;

          // parse data to root with reader
          reader.parse(data, root);

          // get tvShow/episodes (an array)
          auto &episodesArray = root["tvShow"]["episodes"];

          // new binge to add
          Series newSeries(newBingeName);

          // a vectory array of seasons
          std::vector<int> seasons{};
          // for each episode you find, find the season and add to
          // the vector array above
          for (auto &episode : episodesArray){
            int season{episode.get("season","").asInt()};
            try{
              seasons.at(season-1)++;
            } catch (std::out_of_range){
              seasons.push_back(1);
            }
          }

          // for each season in seasonEpisodes, add a season
          for (int &seasonEpisodes : seasons){
            newSeries.addSeason(seasonEpisodes);
          }

          newSeries.print(true);
          newSeries.writeFile(home+DEFAULT_DIR+'/'+newBingeName);
          break;
        }
        case 'f':
          fflag = true;
          break;
        case 'd':
          dflag = true;
          break;
        case 'E':
          Eflag = true;
          break;
        case 'n': {
          // variable for new binges seasons and episodes
          int newSeasons{}, newEpisodes{};
          // variable for new series name
          std::string newSeriesName{};
          // "name,episodes+seasons"
          std::string strOptarg{optarg};
          int index{};
          for (int i{}; i < strOptarg.length(); ++i){
            if (strOptarg[i] == ',')
              index=i;
          }
          newSeriesName = strOptarg.substr(0,index);

          // printBingeExists and skip if binge exists
          if (defaultDirectory.hasFile(newSeriesName)){
            errorSeriesExists(newSeriesName);
            continue;
          }
          // rest of string to extract episodes and seasons from
          std::string restOfString = strOptarg.substr(index+1,strOptarg.length());
          // left trim it
          trimStart(restOfString);

          // argStream for episodes and seasons
          std::stringstream argStream{};
          argStream.str(restOfString);
          // stream it into our two variables
          argStream >> newEpisodes >> newSeasons;

          // print binge and write it to a file
          Series newBinge(newSeriesName,newEpisodes,newSeasons);
          newBinge.print(true);
          newBinge.writeFile(home+DEFAULT_DIR+'/'+newSeriesName);
          break;
        }
        case 'F':
          Fflag = true;
          fflag = true;
          break;
        case 'a':
          avalue = atoi(optarg);
          break;
        case 'r':
          rvalue = atoi(optarg);
          break;
        default:
          break;
      }
    }
  }

  // vector array for all series
  std::vector<Series> allSeries{};

  // selected indexes (of all series) to do stuff on them
  std::vector<int> selectedIndexesOfSeries{};

  {
    // if number of non-option arguments is less than 1, print all
    bool printAll{argc-optind < 1};

    // if theres a svalue, disable print all
    if (svalues.size()) printAll = false;
    int pathsOfFilesIndex{};

    // update directory (add newly added binges)
    defaultDirectory.reload();

    for (std::string &filePath : defaultDirectory.pathsOfFiles) {
      Series currentSeries{};
      Series::status loadStatus = currentSeries.loadFile(filePath);

      // if its not loaded successfully, continue
      // potential loop break here
      if (loadStatus != Series::SERIES_SUCCESS) continue;

      bool bingeCompleted = currentSeries.isCompleted();

      // -f = also finished
      if (bingeCompleted && !fflag) continue;

      // -F = only finished
      if (Fflag && !bingeCompleted) continue;

      // print all if no opts and don't check for anything else
      if (printAll) currentSeries.print(Eflag,eflag,pathsOfFilesIndex);

      // push all series to a vector array
      allSeries.push_back(currentSeries);

      // if theres sflag, search for the svalues inside binges.
      if (sflag){
        for (auto svalue : svalues)
          if ((currentSeries.name == svalue)) {
            // select the indexes of strings pushed in svalue
            selectedIndexesOfSeries.push_back(pathsOfFilesIndex);
          }
      }

      pathsOfFilesIndex++;
    }

    // no series = error message !
    if (!allSeries.size()){
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
      Series &currentSerie = allSeries.at(selectedIndex);

      // if a,r or d flag, print beforehand too
      if (avalue || rvalue || dflag) currentSerie.print(Eflag,eflag,selectedIndex);
      if (dflag) {
        currentSerie.deleteFile();
        continue;
      }
      currentSerie.addWatchedEpisodes(avalue);
      currentSerie.removeWatchedEpisodes(rvalue);
      currentSerie.print(Eflag,eflag, selectedIndex);
      currentSerie.writeFile();
    }
    catch (std::out_of_range) {
      std::cerr << "You don't show " << selectedIndex+1 << ". You have show 1 to " << allSeries.size() <<".\n";
      return 1;
    }
  }
  return 0;
}
