#define _GLIBCXX_USE_NANOSLEEP
#include <cstdlib>
#include <curses.h>
#include <format>
#include <iostream>
#include <algorithm>
#include <future>
#include <thread>

#include <curl/curl.h>
#include <json/reader.h>
#include <json/writer.h>
#include <sstream>

#include "api.h"
#include "select.h"
// std::string newSeriesName = optarg;

static inline size_t curlWriteFunction(void* ptr, size_t size, size_t nmemb, std::string* data);
Request::Request (std::string url)
{
  CURL *curlHandle = curl_easy_init();
  if (curlHandle){
    curl_easy_setopt(curlHandle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, curlWriteFunction);
    curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &this->result);
    curl_easy_perform(curlHandle);
  }
  curl_easy_cleanup(curlHandle);
}

Json::Value Request::json()
{
  Json::Value output{};
  Json::Reader reader;

  // parse data to root with reader
  reader.parse(this->result, output);
  return output;
}

// write function for curl, increases size of string
static inline size_t curlWriteFunction(void* ptr, size_t size, size_t nmemb, std::string* data) 
{
  data->append((char*)ptr, size * nmemb);
  return size * nmemb;
}

void TvShowApi::search(std::string query)
{
  int pageIndex=1, pages=1;
  std::vector<Json::Value>tvShows{};
  Search search;
  std::future<int> idk = std::async(std::launch::async, &Search::search,search);
  do {
    Request re(getSearchUrl(query, pageIndex));
    auto jsonPage = re.json();
    Json::FastWriter writer;
    for (auto const &tvShow: jsonPage["tv_shows"]) {
      ShowName currentName;
      for (auto const &name: tvShow.getMemberNames()) {
        if (name == "name") 
          currentName.name = tvShow[name].asString();
        if (name=="permalink")
          currentName.link = tvShow[name].asString();
      }
      search.names->push_back(currentName);
      search.rewrite();
      // printw("%s",strstream.str().c_str());
    }
    tvShows.insert(tvShows.end(), jsonPage["tv_shows"].begin(), jsonPage["tv_shows"].end());

    pages = jsonPage["pages"].asInt();
    pageIndex++;
  } while (pageIndex <= pages);
}

std::string TvShowApi::getSearchUrl(std::string query, unsigned int page)
{
  return std::format("https://www.episodate.com/api/search?q={}&page={}", query, page);
}

int main(int argc, char *argv[])
{
  TvShowApi::search(argv[1]);
}


// // initial json root and reader
// Json::Value root{};
// Json::Reader reader;

// // parse data to root with reader
// reader.parse(data, root);

// // get tvShow/episodes (an array)
// auto &episodesArray = root["tvShow"]["episodes"];

// // new binge to add
// TvShow newSeries(newSeriesName);

// // a vectory array of seasons
// std::vector<int> seasons{};
// // for each episode you find, find the season and add to
// // the vector array above
// for (auto &episode : episodesArray){
//   int season{episode.get("season","").asInt()};
//   try{
//     seasons.at(season-1)++;
//   } catch (std::out_of_range){
//     seasons.push_back(1);
//   }
// }

// // for each season in seasonEpisodes, add a season
// for (int &seasonEpisodes : seasons){
//   newSeries.addSeason(seasonEpisodes);
// }
// args.newSeries.push_back();
// newSeries.print(true);
// newSeries.writeFile(home+DEFAULT_DIR+'/'+newSeriesName);
// break;

