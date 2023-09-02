#include "tv-shows.h"
#include <string>
#include <json/json.h>

#ifndef BINGEWATCHER_API_H
#define BINGEWATCHER_API_H

class Request {
std::string result;

public:
  Request(std::string url);
  Json::Value json();
};

class TvShowApi {
public:
  static std::string getSearchUrl(std::string query, unsigned int page);
  static void search(std::string query);
  TvShow parseDetail(std::string link);
};

#endif // !BINGEWATCHER_API_H
