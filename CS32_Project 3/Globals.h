//
//  Globals.h
//  CS32_Project 3
//
//  Created by Eric Huang on 5/25/22.
//

#ifndef Globals_h
#define Globals_h
#include <random>

const int MAXROWS = 10;
const int MAXCOLS = 10;

enum Direction {
    //an orientation for a ship
    HORIZONTAL, VERTICAL
};

class Point //a position on the board.
{
  public:
    Point() : r(0), c(0) {}
    Point(int rr, int cc) : r(rr), c(cc) {}
    int r;
    int c;
};

  // Return a uniformly distributed random int from 0 to limit-1
inline int randInt(int limit)
{
    static std::random_device rd;
    static std::mt19937 generator(rd());
    if (limit < 1)
        limit = 1;
    std::uniform_int_distribution<> distro(0, limit-1);
    return distro(generator);
}

#endif /* Globals_h */
