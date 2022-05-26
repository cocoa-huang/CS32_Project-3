//
//  Game.h
//  CS32_Project 3
//
//  Created by Eric Huang on 5/25/22.
//

#ifndef Game_h
#define Game_h
#include <string>
#include <cassert>

class Point;
class Player;
class GameImpl;

class Game //  run a complete game between two players
{
  public:
    //Construct a game with the indicated number of rows and columns,
    Game(int nRows, int nCols);
    
    ~Game();
    
    //Return the number of rows in the game board
    int rows() const;
    //Return the number of columns in the game board.
    int cols() const;
    //Return true if and only if the point denotes a position on the game board.
    bool isValid(Point p) const;
    //Return a random point on the game board.
    Point randomPoint() const;
    //Add a new type of ship to the game.
    bool addShip(int length, char symbol, std::string name);
    //Return the number of ship types in the game,
    int nShips() const;
    //Return the number of positions occupied by the ship whose ID is shipId.
    int shipLength(int shipId) const;
    //Return the character that, when displaying a board, will be used to represent the ship whose ID is shipId.
    char shipSymbol(int shipId) const;
    //Return the string that, when writing messages, will be used to denote the ship whose ID is shipId.
    std::string shipName(int shipId) const;
    Player* play(Player* p1, Player* p2, bool shouldPause = true);
      // We prevent a Game object from being copied or assigned
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

  private:
    GameImpl* m_impl;
};


#endif /* Game_h */
