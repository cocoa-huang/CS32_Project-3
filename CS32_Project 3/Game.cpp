//
//  Game.cpp
//  CS32_Project 3
//
//  Created by Eric Huang on 5/25/22.
//

#include "Game.h"
#include "Board.h"
#include "Player.h"
#include "Globals.h"
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cctype>

using namespace std;

class GameImpl
{
public:
    //game constructor
    GameImpl(int nRows, int nCols);
    int rows() const;
    int cols() const;
    //determine if a point is within the board or not
    bool isValid(Point p) const;
    //return a random point inside the limit of the board
    Point randomPoint() const;
    //add new ship
    bool addShip(int length, char symbol, string name);
    int nShips() const;
    int shipLength(int shipId) const;
    char shipSymbol(int shipId) const;
    string shipName(int shipId) const;
    Player* play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause);
    ~GameImpl();
private:
    int m_rows;
    int m_cols;
    int m_nShips;
    //private struct storing ship data
    struct Ship {
        Ship(int id, int l, char c, string n):s_id(id), s_length(l), s_symbol(c), s_name(n){};
        int s_id;
        int s_length;
        char s_symbol;
        string s_name;
    };
    
    //vectors of ships pointers storing ships
    //vector index corresponds to each shipID
    vector<Ship*> m_ships;
};

void waitForEnter()
{
    cout << "Press enter to continue: ";
    cin.ignore(10000, '\n');
}

//constructor
GameImpl::GameImpl(int nRows, int nCols): m_rows(nRows), m_cols(nCols), m_nShips(0), m_ships({})
{
    
}

//destructor deleting ship pointers
GameImpl::~GameImpl() {
    Ship* temp;
    while(!m_ships.empty()) {
        temp = m_ships.back();
        //deleting ship pointers one by one
        m_ships.pop_back();
        delete temp;
    }
}

int GameImpl::rows() const
{
    return m_rows;
}

int GameImpl::cols() const
{
    return m_cols;
}

bool GameImpl::isValid(Point p) const
{
    return p.r >= 0  &&  p.r < rows()  &&  p.c >= 0  &&  p.c < cols();
}

Point GameImpl::randomPoint() const
{
    return Point(randInt(rows()), randInt(cols()));
}

bool GameImpl::addShip(int length, char symbol, string name)
{
    //add a new ship into the vector and increase the number of ships corresponding to the shipID
    Ship* new_ship = new Ship(m_nShips++, length, symbol, name);
    m_ships.push_back(new_ship);
    return true;
}

int GameImpl::nShips() const
{
    return m_nShips;
}

int GameImpl::shipLength(int shipId) const
{
    return m_ships[shipId]->s_length;
}

char GameImpl::shipSymbol(int shipId) const
{
    return m_ships[shipId]->s_symbol;
}

string GameImpl::shipName(int shipId) const
{
    return m_ships[shipId]->s_name;
}

Player* GameImpl::play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause)
{
    //return nullptr if cannot place ship
    if(p1->placeShips(b1) == false || p2->placeShips(b2) == false) {
        return nullptr;
    }
    
    //play continues until one of the player's ships get all destroyed
    while(!b1.allShipsDestroyed() || !b2.allShipsDestroyed()) {
        //set necessary parameters to false
        bool isHit = false, isDestroyed = false, isValidShot = false;
        //initialize shipID
        int shipID = 0;
        
        //p1's turn first
        cout << p1->name() << " 's turn. Board for " << p2->name() << " :" << endl;
        //display p1 board
        b2.display(p1->isHuman());
        
        //prompt attack action and record point being attacked
        Point ptAttacked = p1->recommendAttack();
        //check if the attack is valid and record the attack on board
        isValidShot = b2.attack(ptAttacked, isHit, isDestroyed, shipID);
        //record the result of the attack
        p1->recordAttackResult(ptAttacked, isValidShot, isHit, isDestroyed, shipID);
        //record attack by opponent
        p2->recordAttackByOpponent(ptAttacked);
        
        //display result of attack
        //if attack a point previously been attack (only possible for human player)
        if(p1->isHuman() && isValidShot == false) {
            cout << p1->name() << " wasted a shot at (" << ptAttacked.r << ", " << ptAttacked.c << ")." << endl;
        //if attack is valid
        } else {
            //display result depending on result of attack
            if(isHit && isDestroyed) {
                cout << p1->name() << " attacked (" << ptAttacked.r << ", " << ptAttacked.c << ") and destroyed the " << this->shipName(shipID) << " resulting in: "<< endl;
            } else if (isHit) {
                cout << p1->name() << " attacked (" << ptAttacked.r << ", " << ptAttacked.c << ") and hit something, resulting in: " << endl;
            } else {
                cout << p1->name() << " attacked (" << ptAttacked.r << ", " << ptAttacked.c << ") and missed, resulting in: " << endl;
            }
            //display
            b2.display(p1->isHuman());
        }
        
        //check if anyone has won
        if(b1.allShipsDestroyed() || b2.allShipsDestroyed()) {
            break;
        }
        
        //pause if needed
        if(shouldPause) {
            waitForEnter();
        }
        
        //p2 turn
        cout << p2->name() << " 's turn. Board for " << p1->name() << " :" << endl;
        b1.display(p2->isHuman());
        
        //attack
        ptAttacked = p2->recommendAttack();
        isValidShot = b1.attack(ptAttacked, isHit, isDestroyed, shipID);
        p2->recordAttackResult(ptAttacked, isValidShot, isHit, isDestroyed, shipID);
        p1->recordAttackByOpponent(ptAttacked);
        
        //display result of attack
        //if attack a point previously been attack (only possible for human player)
        if(isValidShot == false) {
            cout << p2->name() << " wasted a shot at (" << ptAttacked.r << ", " << ptAttacked.c << ")." << endl;
        //if attack is valid
        } else {
            // different output based on result of attack
            if(isHit && isDestroyed) {
                cout << p2->name() << " attacked (" << ptAttacked.r << ", " << ptAttacked.c << ") and destroyed the " << this->shipName(shipID) << " resulting in: "<< endl;
            } else if (isHit) {
                cout << p2->name() << " attacked (" << ptAttacked.r << ", " << ptAttacked.c << ") and hit something, resulting in: " << endl;
            } else {
                cout << p2->name() << " attacked (" << ptAttacked.r << ", " << ptAttacked.c << ") and missed, resulting in: " << endl;
            }
            b1.display(p2->isHuman());
        }
        
        //check if anyone has won the game
        if(b1.allShipsDestroyed() || b2.allShipsDestroyed()) {
            break;
        }
        
        if(shouldPause) {
            waitForEnter();
        }
    }
    
    //set pointer to winner
    Player* winner;
    if(b1.allShipsDestroyed()) {
        winner = p2;
    } else {
        winner = p1;
    }
    
    cout << winner->name() << " wins!" << endl;
    //return winner pointer
    return winner;
}

//******************** Game functions *******************************

// These functions for the most part simply delegate to GameImpl's functions.
// You probably don't want to change any of the code from this point down.

Game::Game(int nRows, int nCols)
{
    if (nRows < 1  ||  nRows > MAXROWS)
    {
        cout << "Number of rows must be >= 1 and <= " << MAXROWS << endl;
        exit(1);
    }
    if (nCols < 1  ||  nCols > MAXCOLS)
    {
        cout << "Number of columns must be >= 1 and <= " << MAXCOLS << endl;
        exit(1);
    }
    m_impl = new GameImpl(nRows, nCols);
}

Game::~Game()
{
    delete m_impl;
}

int Game::rows() const
{
    return m_impl->rows();
}

int Game::cols() const
{
    return m_impl->cols();
}

bool Game::isValid(Point p) const
{
    return m_impl->isValid(p);
}

Point Game::randomPoint() const
{
    return m_impl->randomPoint();
}

bool Game::addShip(int length, char symbol, string name)
{
    if (length < 1)
    {
        cout << "Bad ship length " << length << "; it must be >= 1" << endl;
        return false;
    }
    if (length > rows()  &&  length > cols())
    {
        cout << "Bad ship length " << length << "; it won't fit on the board"
             << endl;
        return false;
    }
    if (!isascii(symbol)  ||  !isprint(symbol))
    {
        cout << "Unprintable character with decimal value " << symbol
             << " must not be used as a ship symbol" << endl;
        return false;
    }
    if (symbol == 'X'  ||  symbol == '.'  ||  symbol == 'o')
    {
        cout << "Character " << symbol << " must not be used as a ship symbol"
             << endl;
        return false;
    }
    int totalOfLengths = 0;
    for (int s = 0; s < nShips(); s++)
    {
        totalOfLengths += shipLength(s);
        if (shipSymbol(s) == symbol)
        {
            cout << "Ship symbol " << symbol
                 << " must not be used for more than one ship" << endl;
            return false;
        }
    }
    if (totalOfLengths + length > rows() * cols())
    {
        cout << "Board is too small to fit all ships" << endl;
        return false;
    }
    return m_impl->addShip(length, symbol, name);
}

int Game::nShips() const
{
    return m_impl->nShips();
}

int Game::shipLength(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipLength(shipId);
}

char Game::shipSymbol(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipSymbol(shipId);
}

string Game::shipName(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipName(shipId);
}

Player* Game::play(Player* p1, Player* p2, bool shouldPause)
{
    if (p1 == nullptr  ||  p2 == nullptr  ||  nShips() == 0)
        return nullptr;
    Board b1(*this);
    Board b2(*this);
    return m_impl->play(p1, p2, b1, b2, shouldPause);
}

