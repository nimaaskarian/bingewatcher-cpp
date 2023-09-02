#include "select.h"
#include <algorithm>
#include <curses.h>
#include <boost/algorithm/string/find.hpp>
#include <fstream>
#include <sstream>

#define CTRL_KEY(k) ((k) & 0x1f)
#define MY_KEY_BACKSPACE 127
#define MY_KEY_RESIZE 410

void writeSearchTerm(std::string *searchTerm, unsigned long searchCount)
{
  mvprintw(getmaxy(stdscr)-1, 0, "Search between %lu series: %s", searchCount ,searchTerm->c_str());
}

Match findStringIC(const std::string & strHaystack, const std::string & strNeedle)
{
  auto it = std::search(
    strHaystack.begin(), strHaystack.end(),
    strNeedle.begin(),   strNeedle.end(),
    [](unsigned char ch1, unsigned char ch2) { return std::toupper(ch1) == std::toupper(ch2); }
  );
  Match output;
  if (it != strHaystack.end()) {
    output.unmatched = strHaystack.length()-strNeedle.length();
    output.ismatching = true;
  }
  return output;
}

std::string ShowName::str()
{
  std::stringstream ss;
  ss << "name: " << name << ", link: " << link;
  return ss.str();
}
Match ShowName::find(std::string needle)
{
  // if (boost::ifind_first(link,needle) || boost::ifind_first(name, needle))
  Match empty{};
  if (needle.length() == 0)
    return empty;

  Match linkMatch = findStringIC(link,needle);
  Match nameMatch = findStringIC(name,needle);
  if (linkMatch.ismatching) {
    return linkMatch;
  }
  if (nameMatch.ismatching) {
    return nameMatch;
  }
  return empty;
}

void delchar(int delCount=1)
{
  while (delCount-- > 0) {
    wmove(stdscr, stdscr->_cury , stdscr->_curx-1);
    delch();
    wmove(stdscr, stdscr->_cury , stdscr->_curx+1);
    refresh();
  }
}

bool sortMatches(Match a, Match b) {
  return a.unmatched < b.unmatched;
}


void Search::rematch()
{
  matches.clear();
  for (unsigned int i = 0; i < names->size() ; i++) {
    Match currentMatch = names->at(i).find(*searchTerm);
    if (currentMatch.ismatching || searchTerm->length() == 0) {
      currentMatch.index = i;
      matches.push_back(currentMatch);
    }
  }
  if (searchTerm->length())
    std::sort(matches.begin(), matches.end(), sortMatches);
}

void Search::rewrite()
{
  auto before = matches;
  rematch();
  lastSearched = *searchTerm;
  unsigned long maxy = getmaxy(stdscr);
  auto size = std::min(matches.size(), maxy)-1;

  if (matches.size() != 0) {
    if (beforeSize != size || !std::equal(matches.begin(), matches.begin()+size, before.begin())) {
      beforeSize = size;
      clear();
      for (int i = 0; i < getmaxy(stdscr)-1; i++) {
        if (i == matches.size())
          break;
        mvprintw(i,0,"%s\n",names->at(matches[i].index).str().c_str());
      }
    }
  } else {
    beforeSize = 0;
    clear();
  }

  writeSearchTerm(searchTerm, names->size());
  refresh();
}

Search::~Search()
{
  delete searchTerm;
  delete names;
}
Search::Search()
{
  searchTerm = new std::string{};
  names = new std::vector<ShowName>{};
}
int Search::search()
{
  initscr();
  cbreak();
  noecho();

  int ch;

  writeSearchTerm(searchTerm, names->size());
  refresh();

  while ((ch = getch()) != '\n') {
    switch (ch) {
      case KEY_RESIZE: {
        rewrite();
        refresh();
        break;
      }
      case MY_KEY_BACKSPACE: {
        if (searchTerm->length() > 0) {
          delchar();
          searchTerm->pop_back();
        }
        break;
      }
      case CTRL_KEY('u'): {
        delchar(searchTerm->length());
        *searchTerm = "";
        clrtoeol();
        break;
      }
      default: {
        searchTerm->push_back(ch);
        break;
      }
    }
    rewrite();
    // mvprintw(0,0,"Hello %lu\n", strings->size());
  }
  endwin();
  std::cout << searchTerm->c_str();
  exit(1);
  // clear();
  std::vector<std::string> matchingStrings;
  // for (const auto& str : *names) {
  //   if (str.find(searchTerm) != std::string::npos) {
  //    matchingStrings.push_back(str);
  //   }
  // }

  if (matchingStrings.empty()) {
    printw("No matching strings found.\n");
  } else {
    printw("Matching strings:\n");
    for (int i = 0; i < matchingStrings.size(); i++) {
     printw("%d. %s\n", i + 1, matchingStrings[i].c_str());
    }
  }

  getch();

  endwin();
}
