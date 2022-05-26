//
//  Player.h
//  CS32_Project 3
//
//  Created by Eric Huang on 5/25/22.
//

#ifndef Player_h
#define Player_h
#include <string>

class Point;
class Board;
class Game;

class Player
{
  public:
    //Initialize a player whose name is nm
    Player(std::string nm, const Game& g)
     : m_name(nm), m_game(g)
    {}

    virtual ~Player() {}
    //Return the name of the player.
    std::string name() const { return m_name; }
    //Return a constant reference to the game the player is being used in
    const Game& game() const { return m_game; }
    //Return true if the player is a human, false otherwise
    virtual bool isHuman() const { return false; }
    //returns true if all ships could be placed, and false otherwise
    virtual bool placeShips(Board& b) = 0;
    //return a point that indicates the position on the opponent's board that the player would like to attack next.
    virtual Point recommendAttack() = 0;
    //let an intelligent non-human player update its data members in a way that will let the next call of recommendAttack make a good choice
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                        bool shipDestroyed, int shipId) = 0;
    virtual void recordAttackByOpponent(Point p) = 0;
      // We prevent any kind of Player object from being copied or assigned
    Player(const Player&) = delete;
    Player& operator=(const Player&) = delete;

  private:
    std::string m_name;
    const Game& m_game;
};

Player* createPlayer(std::string type, std::string nm, const Game& g);

#endif /* Player_h */
