//
//  Board.cpp
//  CS32_Project 3
//
//  Created by Eric Huang on 5/25/22.
//

#include "Board.h"
#include "Game.h"
#include "Globals.h"
#include <iostream>
#include <vector>
#include <map>

using namespace std;

class BoardImpl
{
  public:
    BoardImpl(const Game& g);
    //clear the board
    void clear();
    //block 50% of the points on board
    void block();
    //unblock and blocked points on board
    void unblock();
    //place ships on board based on indicated point
    bool placeShip(Point topOrLeft, int shipId, Direction dir);
    //remove ships on board based on indicated point
    bool unplaceShip(Point topOrLeft, int shipId, Direction dir);
    //display board
    void display(bool shotsOnly) const;
    //display and record attack result
    bool attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId);
    //return true if all ships on board are being destroyed
    bool allShipsDestroyed() const;

  private:
    const Game& m_game;
    //create a 2-D array board
    char m_board[MAXROWS][MAXCOLS];
};

BoardImpl::BoardImpl(const Game& g)
: m_game(g)
{
    //interate through all board points
    for(int r = 0; r < m_game.rows(); r++) {
        for(int c = 0; c < m_game.cols(); c++) {
            //set up the board
            m_board[r][c] = '.';
        }
    }
}

void BoardImpl::clear()
{
    for(int r = 0; r < m_game.rows(); r++) {
        for(int c = 0; c < m_game.cols(); c++) {
            //clearing any symbols at the point
            m_board[r][c] = '.';
        }
    }
}

void BoardImpl::block()
{
      // Block cells with 50% probability
    for(int i = 0; i < ((m_game.rows() * m_game.cols()) / 2); i++) {
        Point temp = m_game.randomPoint();
        if(m_board[temp.r][temp.c] == '#') {
            //back track if the point is already blocked
            i--;
        } else {
            //use "#" to indicate blocked points
            m_board[temp.r][temp.c] = '#';
        }
    }
}

void BoardImpl::unblock()
{
    for (int r = 0; r < m_game.rows(); r++)
        for (int c = 0; c < m_game.cols(); c++)
        {
            //remove any blocked points
            if(m_board[r][c] == '#') {
                m_board[r][c] = '.';
            }
        }
}

bool BoardImpl::placeShip(Point topOrLeft, int shipId, Direction dir)
{
    
    //return false if shipID invalid
    if(shipId < 0 || shipId > m_game.nShips() - 1) {
        return false;
    }
    
   //return false if the point is out of bounds
    if (!m_game.isValid(topOrLeft)) {
          return false;
    }
    
    
    for(int r = 0; r < m_game.rows(); r++) {
        for(int c = 0; c < m_game.cols(); c++) {
            //return false if the ship with that ship ID has previously been placed on this Board and not yet been unplaced
            if(m_board[r][c] == m_game.shipSymbol(shipId)) {
                return false;
            }
        }
    }
    
    //stored length of each ship
    int len = m_game.shipLength(shipId);
    //vertical operation
    if(dir == VERTICAL) {
        Point top = topOrLeft;
        //check to see if ship length fits within the board
        if(top.r + len > m_game.rows()) {
            return false;
        }
        
        //return false if the placed ship would overlap with blocked points or any previously placed ships
        for(int i = 0; i < len; i++) {
            if(m_board[top.r + i][top.c] != '.') {
                return false;
            }
        }
        
        //if all conditions passed, place the ship according to the direction
        for(int i = 0; i < len; i++) {
            m_board[top.r + i][top.c] = m_game.shipSymbol(shipId);
        }
    //horizontal placement
    } else {
        Point left = topOrLeft;
        if(left.c + len > m_game.cols()) {
            return false;
        }
        
        for(int i = 0; i < len; i++) {
            if(m_board[left.r][left.c + i] != '.') {
                return false;
            }
        }
        
        for(int i = 0; i < len; i++) {
            m_board[left.r][left.c + i] = m_game.shipSymbol(shipId);
        }
    }
    return true;
}

bool BoardImpl::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
    //basically the same conditions with place ship
    if(shipId < 0 || shipId > m_game.nShips() - 1) {
        return false;
    }
        
    if(!m_game.isValid(topOrLeft)) {
        return false;
    }
    
    int len = m_game.shipLength(shipId);
    //vertical operations
    if(dir == VERTICAL) {
        Point top = topOrLeft;
        if(top.r + len > m_game.rows()) {
            return false;
        }
        
        for(int i = 0; i < len; i++) {
            if(m_board[top.r + i][top.c] != '.') {
                return false;
            }
        }
        
        for(int i = 0; i < len; i++) {
            //remove any ship symbols previously placed
            m_board[top.r + i][top.c] = '.';
        }
    //horizontal operations
    } else {
        Point left = topOrLeft;
        if(left.c + len > m_game.cols()) {
            return false;
        }
        
        for(int i = 0; i < len; i++) {
            if(m_board[left.r][left.c + i] != '.') {
                return false;
            }
        }
        
        for(int i = 0; i < len; i++) {
            m_board[left.r][left.c + i] = '.';
        }
    }
    return true;
}

void BoardImpl::display(bool shotsOnly) const
{
    //output top row numbers
    cout << "  ";
    for(int i = 0; i < m_game.cols(); i++) {
        cout << i;
    }
    cout << endl;
    
    //if set to not display only shots
    if(!shotsOnly) {
        for(int r = 0; r < m_game.rows(); r++) {
            cout << r << " ";
            for(int c = 0; c < m_game.cols(); c++) {
                //display what the point originally stores
                cout << m_board[r][c];
            }
            cout << endl;
        }
    //if set to display only shots
    } else {
        for(int r = 0; r < m_game.rows(); r++) {
            cout << r << " ";
            for(int c = 0; c < m_game.cols(); c++) {
                //hide any ship symbols
                if(m_board[r][c] != 'X' && m_board[r][c] != 'o' && m_board[r][c] != '.') {
                    cout << '.';
                } else {
                    cout << m_board[r][c];
                }
            }
            cout << endl;
        }
    }
}

//attack a point on the board
bool BoardImpl::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
    //return false if point attached out of bounds
    if(p.r < 0 || p.r > m_game.rows() - 1 || p.c < 0 || p.c > m_game.cols() - 1) {
        return false;
    }
    
    //return false is attacked a point previously being attacked
    if(m_board[p.r][p.c] == 'o' || m_board[p.r][p.c] == 'X') {
        return false;
    //if hit a point that's empty and had not previously being attacked
    } else if(m_board[p.r][p.c] == '.') {
        shotHit = false;
        //set the point to display the missed symbol
        m_board[p.r][p.c] = 'o';
    //if hit something
    } else {
        //set shotHit to true
        shotHit = true;
        //store the symbol of the ship that gets hit
        char symbol = m_board[p.r][p.c];
        //set the point to display the hit symbol
        m_board[p.r][p.c] = 'X';
        
        //return false if all components of that ship are being destroyed
        bool isShipDestroyed = true;
        for(int r = 0; r < m_game.rows(); r++) {
            for(int c = 0; c < m_game.cols(); c++) {
                //if the there's still symbol of that ship exist on the board
                if(symbol == m_board[r][c]) {
                    //that ship is not being completely destroyed
                    isShipDestroyed = false;
                    //break from loop
                    break;
                }
            }
            if(isShipDestroyed == false) {
                break;
            }
        }
        
        if(isShipDestroyed == false) {
            //set variable to false if ship is not completely destroyed
            shipDestroyed = false;
        } else {
            //set variable to true if ship is completely destroyed
            shipDestroyed = true;
            for(int i = 0; i < m_game.nShips(); i++) {
                //store the id of that ship
                if(m_game.shipSymbol(i) == symbol) {
                    shipId = i;
                }
            }
        }
        
        
    }
    
    return true;
}


bool BoardImpl::allShipsDestroyed() const
{
    for(int r = 0; r < m_game.rows(); r++) {
        for(int c = 0; c < m_game.cols(); c++) {
            //if the board still has ship symbols present
            if(m_board[r][c] != 'X' && m_board[r][c] != 'o' && m_board[r][c] != '.') {
                //that means not all ships have been destroyed
                return false;
            }
        }
    }
    //else all ships being destroyed
    return true;
}

//******************** Board functions ********************************

// These functions simply delegate to BoardImpl's functions.
// You probably don't want to change any of this code.

Board::Board(const Game& g)
{
    m_impl = new BoardImpl(g);
}

Board::~Board()
{
    delete m_impl;
}

void Board::clear()
{
    m_impl->clear();
}

void Board::block()
{
    return m_impl->block();
}

void Board::unblock()
{
    return m_impl->unblock();
}
 
bool Board::placeShip(Point topOrLeft, int shipId, Direction dir)
{
    return m_impl->placeShip(topOrLeft, shipId, dir);
}

bool Board::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
    return m_impl->unplaceShip(topOrLeft, shipId, dir);
}

void Board::display(bool shotsOnly) const
{
    m_impl->display(shotsOnly);
}

bool Board::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
    return m_impl->attack(p, shotHit, shipDestroyed, shipId);
}

bool Board::allShipsDestroyed() const
{
    return m_impl->allShipsDestroyed();
}

