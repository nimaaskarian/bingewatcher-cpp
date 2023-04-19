#include <iostream>
#include <json/value.h>
#include <sstream>
#include <string>
#include <unistd.h>
#include <algorithm>

#include <json/json.h>
#include <curl/curl.h>

#include "lib/binge.h"
#include "lib/directory.h"

#define DEFAULT_DIR "/.cache/bingewatcher"

// trim from start (in place with reference)
static inline void trimStart(std::string &str) {
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

size_t writeFunction(void* ptr, size_t size, size_t nmemb, std::string* data) {
    data->append((char*)ptr, size * nmemb);
    return size * nmemb;
}

int main(int argc, char* argv[])
{
  const std::string home = getenv("HOME");

  // int variables for flag values.
  int avalue{}, rvalue{};

  // svalue for search flag value
  std::vector<std::string> svalues{};

  // bool variables for flag existence.
  bool fflag{}, Lflag{}, Fflag{},eflag{},dflag{},sflag{};
  {
    // int c, temp variable for arguments
    char currentOpt;

    // iterating over arguments and get a and d flags value
    while ((currentOpt = getopt(argc, argv, "a:r:fLFen:d:o:s:")) != -1) {
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
          std::string url {"https://www.episodate.com/api/show-details?q="+newBingeName};
          // convert spaces to '-' so url be gud
          for (char &ch : url){
            if (ch==' ') ch = '-';
          }
          CURL *curlHandle = curl_easy_init();
          if (curlHandle){
            auto res = curl_easy_setopt(curlHandle, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, writeFunction);
            curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &data);
            curl_easy_perform(curlHandle);
          }
          Json::Value root{};
          Json::Reader reader;
          reader.parse(data, root);
          auto &episodesArray = root["tvShow"]["episodes"];
          Binge newBinge(newBingeName);

          int episodeOfSeason{}, currentSeason{1};
          for (auto &episode : episodesArray){
            int season{episode.get("season","").asInt()};
            if (currentSeason != season){
              newBinge.addSeason(episodeOfSeason);

              currentSeason = season;
              episodeOfSeason=0;
            }
            episodeOfSeason++;
          }
          if (newBinge.seasons.size()) newBinge.addSeason(episodeOfSeason);

          newBinge.print(true);
          newBinge.write(home+DEFAULT_DIR+'/'+newBingeName);
          break;
        }
        case 'f':
          fflag = true;
          break;
        case 'd':
          dflag = true;
          break;
        case 'L':
          Lflag = true;
          break;
        case 'n': {
          // variable for new binges seasons and episodes
          int newBingeSeasons{}, newBingeEpisodes{};
          // variable for new binges name
          std::string newBingeName{};
          // "name,episodes+seasons"
          std::string strOptarg{optarg};
          int index{};
          for (int i{}; i < strOptarg.length(); ++i){
            if (strOptarg[i] == ',')
              index=i;
          }
          newBingeName = strOptarg.substr(0,index);
          // rest of string to extract episodes and seasons from
          std::string restOfString = strOptarg.substr(index+1,strOptarg.length());
          // left trim it
          trimStart(restOfString);

          // argStream for episodes and seasons
          std::stringstream argStream{};
          argStream.str(restOfString);
          // stream it into our two variables
          argStream >> newBingeEpisodes >> newBingeSeasons;

          // print binge and write it to a file
          Binge newBinge(newBingeName,newBingeEpisodes,newBingeSeasons);
          newBinge.print(true);
          newBinge.write(home+DEFAULT_DIR+'/'+newBingeName);
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
          // abort ();
      }
    }
  }
  std::vector<Binge> binges{};

  Directory myfiles(home+DEFAULT_DIR);
  {

    // bool var used to determine if program should print all or not
    // decrease fflag and Fflag (booleans, when true they == 1)
    // so doing -f or -F alone print all
    const bool printAll{argc-optind-fflag-Fflag < 1};
    int i{};
    for (std::string &path : myfiles.paths) {
      Binge currentBinge{};
      Binge::status loadStatus = currentBinge.load(path);

      // fflag means show finished shows aswell.
      if (loadStatus == Binge::BINGE_SUCCESS){
        // std::cout << (currentBinge.isCompleted()) << ',' << !fflag << '\n';

        bool bingeCompleted = currentBinge.isCompleted();
        // -f = also finished
        if (bingeCompleted && !fflag) continue;
        // -F = only finished
        if (Fflag && !bingeCompleted) continue;
        // no args = listing all!
        if (printAll) currentBinge.print(Lflag,eflag,i);
        if (sflag){
          for (auto svalue : svalues)
            if ((currentBinge.name == svalue)) {
              currentBinge.print(Lflag,eflag,i);
              binges.push_back(currentBinge);
              i++;
            }
          continue;
        }

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

      // if theres a r or a value print beforehand too
      if (avalue || rvalue || dflag) binge.print(Lflag,eflag,bingeIndex);
      if (dflag) {
        binge.deleteFile();
        continue;
      }
      binge.add(avalue);
      binge.remove(rvalue);
      binge.print(Lflag,eflag,bingeIndex);
      binge.write();

    } catch (std::invalid_argument) {
      std::cout << "Non-option argument '" << argv[index] << "'\n";
    } catch (std::out_of_range) {
      std::cout << "You don't show " << argv[index] << ". You have show 1 to " << binges.size() <<".\n";
      return 1;
    }
}
