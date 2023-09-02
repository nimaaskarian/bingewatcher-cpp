#ifndef BINGEWATCHER_SELECT_H
#define BINGEWATCHER_SELECT_H
#include <iostream>
#include <vector>
#include <memory>
#include <ncurses.h>

typedef struct Match {
  bool ismatching = false;
  int unmatched = -1;
  int index;
  bool operator==(Match const & rhs) const {
    return this->index == rhs.index;
  }
} Match;

class ShowName {
public:
  std::string link, name;
  Match find(std::string needle);
  std::string str();
};

class Search {
  std::string *searchTerm;
  std::string lastSearched{};
  bool isWrote = false;
  unsigned long beforeSize = 0;
  std::vector<Match> matches;
public:
  std::vector<ShowName> *names;
  int search();
  void rewrite();
  void rematch();
  ~Search();
  Search();
};

#endif // !BINGEWATCHER_SELECT_H
