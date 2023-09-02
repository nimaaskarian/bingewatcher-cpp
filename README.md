# bingewatcher-cpp

same old [bingewatcher](https://github.com/nimaaskarian/bingewatcher), but instead of python, written in cpp.  
helps you keep track of all shows, series or anything that has seasons or episodes (even can kinda handle courses?) for you. 
## better code base.
the code base is just better, its using OOP model, but don't sacrifice performance over anything. (and cpp is more fun than python to code).

## better performance
its written in a low level compiler language, no ~~censored word~~ its gonna have better performance! but its around 10-15 times faster than the python version... so yeah

## questions
- Q. why its called bingewatcher, when you keep tracking of watched series? are you dumb?  
A. I certainly am, but you can make cute scripts using this tool to automate your series watching, and keep track of downloading new episodes and all, it helps me for lots of my bingewatchings.
- Q. why its written in a compiler language? this certainly could be done using python or another scripting language lot more easily.  
A. Of course it could. but I liked to practice cpp, OOP model and clean code. the concepts of this projects are very easy to implement (with any language or tool), but not as easy as a `std::string name{};std::cin>>name; std::cout<<name;`, so its a good project to practice.
codebase shouldn't grow that big, and I don't care if I break it trying to make the code more clean (I pretend like git doesn't exist, cause I usually don't roll back "broken" projects), I wouldn't miss out anything! I'm better off not bingewatching ~~pirated~~ series no matter what aspect you looking at it from.
- Q. why  
A. idk

## mind map?
### main ideas
- file format should be easy to edit by a human being. and it is. 
in the file, each line represent a season. each line has a `<watched episodes>+<all episodes>` format. look the example below:
    ```
    10+10
    3+12
    0+12
    ```
    this means our series has 3 seasons, first season has 10 episodes, second has 12, and third has 12 as well. all season one and 3 episodes of the season one is watched.

- we want an application with no TUI integration but good CLI interface. thats why i use gnu's unistd.h  
- we want online episode info for more user-friendly interface. even us who use `awk`, `tr` and `grep` on daily basis can't argue with necessity of online api for this kind of application. but it shouldn't be forced to be used. using libcurl for the data fetching.
- we want offline series creation. you're not forced to use online feature. after all, you're not using windows 11!
- we want it to be in a single directory, so it can be easily initial a git repo inside it.
- simple, editable files, editable names, editable everything.

### classes
- theres a Series class. it has a lot of functionality, even works with a file (reading from and writing on it). can be initialized with seasons and episodes, or just a name. then load a file path for 
- theres a Season class. it has watched and all properties for it and functions
- theres a Directory class for listing a directory and all. really helps
### main function flow
loops through arguments, initializes `allBinges` Season vector array based on existence of `-f` and `-F` flags.  
selects bunch of indexes using arguments. if theres nothing selected, prints all  
if there is, does actions based on arguments on them
