//
//  Board.h
//  CS32_Project 3
//
//  Created by Eric Huang on 5/25/22.
//

#ifndef Board_h
#define Board_h
#include "Globals.h"

class Game;
class BoardImpl;

class Board
{
  public:
    //Initialize a board.
    Board(const Game& g);
    ~Board();
    //clears the board so it is empty, ready to be populated with ships
    void clear();
    //blocks exactly half randomly chosen positions on the board
    void block();
    //unblocks all the blocked positions on the board
    void unblock();
    //attempts to place the specified ship at the specified coordinate, in the specified direction.
    //returns false if the ship cannot be placed
    bool placeShip(Point topOrLeft, int shipId, Direction dir);
    // attempts to remove the specified ship from the board, so the positions it occupied may be used to place other ships
    bool unplaceShip(Point topOrLeft, int shipId, Direction dir);
    //displays the board
    void display(bool shotsOnly) const;
    //submit an attack against the board
    // return false if the attack is invalid
    //returns true if the attack is valid, regardless of whether or not any ship is damaged.
    bool attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId);
    // returns true if all ships have been completely destroyed on the current board and false otherwise.
    bool allShipsDestroyed() const;
    
      // We prevent a Board object from being copied or assigned
    Board(const Board&) = delete;
    Board& operator=(const Board&) = delete;

  private:
    BoardImpl* m_impl;
};


#endif /* Board_h */
