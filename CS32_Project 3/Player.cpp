//
//  Player.cpp
//  CS32_Project 3
//
//  Created by Eric Huang on 5/25/22.
//

#include "Player.h"
#include "Board.h"
#include "Game.h"
#include "Globals.h"
#include <iostream>
#include <string>
#include <vector>
#include <stack>

using namespace std;

//*********************************************************************
//  AwfulPlayer
//*********************************************************************

class AwfulPlayer : public Player
{
  public:
    AwfulPlayer(string nm, const Game& g);
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                                bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
  private:
    Point m_lastCellAttacked;
};

AwfulPlayer::AwfulPlayer(string nm, const Game& g)
 : Player(nm, g), m_lastCellAttacked(0, 0)
{}

bool AwfulPlayer::placeShips(Board& b)
{
      // Clustering ships is bad strategy
    for (int k = 0; k < game().nShips(); k++)
        if ( ! b.placeShip(Point(k,0), k, HORIZONTAL))
            return false;
    return true;
}

Point AwfulPlayer::recommendAttack()
{
    if (m_lastCellAttacked.c > 0)
        m_lastCellAttacked.c--;
    else
    {
        m_lastCellAttacked.c = game().cols() - 1;
        if (m_lastCellAttacked.r > 0)
            m_lastCellAttacked.r--;
        else
            m_lastCellAttacked.r = game().rows() - 1;
    }
    return m_lastCellAttacked;
}

void AwfulPlayer::recordAttackResult(Point /* p */, bool /* validShot */,
                                     bool /* shotHit */, bool /* shipDestroyed */,
                                     int /* shipId */)
{
      // AwfulPlayer completely ignores the result of any attack
}

void AwfulPlayer::recordAttackByOpponent(Point /* p */)
{
      // AwfulPlayer completely ignores what the opponent does
}

//*********************************************************************
//  HumanPlayer
//*********************************************************************

bool getLineWithTwoIntegers(int& r, int& c)
{
    bool result(cin >> r >> c);
    if (!result)
        cin.clear();  // clear error state so can do more input operations
    cin.ignore(10000, '\n');
    return result;
}

class HumanPlayer : public Player {
public:
    //constructor
    HumanPlayer(string nm, const Game& g);
    virtual ~HumanPlayer();
    //human player place ships
    virtual bool placeShips(Board& b);
    //return true if the player is human
    virtual bool isHuman() const;
    //prompt human player to attack
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
    
};


HumanPlayer::HumanPlayer(string nm, const Game& g):Player(nm, g){};

HumanPlayer::~HumanPlayer(){};

bool HumanPlayer::placeShips(Board &b) {
    //initialize necessary variables
    string d;
    int r = -1, c = -1;
    bool dirValid = false;
    bool placeValid = false;
    bool ptValid = false;
    Direction dir;
    
    //prompt human player to place ship
    cout << Player::name() << " must place " << game().nShips() << " ship" << endl;
    for(int i = 0; i < game().nShips(); i++) {
        b.display(false);
        //prompt player to enter direction
        while(!dirValid) {
            cout << "Enter h or v for direction of " << game().shipName(i) << " (length " << game().shipLength(i) << "): ";
            //receive input for direction
            getline(cin, d);
            //prompt error message if direction received is invalid
            if(d != "h" && d != "v") {
                cout << "Direction must be h or v." << endl;
            } else {
                dirValid = true;
            }
        }
        dirValid = false;
        
        if(d == "h") {
            dir = HORIZONTAL;
        } else {
            dir = VERTICAL;
        }
        
        //prompt player to enter the coordinate to place
        while(!placeValid) {
            cout << "Enter row and column of ";
            if(dir == HORIZONTAL) {
                cout << "leftmost ";
            } else {
                cout << "topmost ";
            }
            cout << "cell (e,g., 3 5): ";
            
            while(!ptValid) {
                //prompt error message if received coordinate is invalid
                if(!getLineWithTwoIntegers(r, c)) {
                    cout << "You must enter two integers." << endl;
                } else {
                    ptValid = true;
                }
            }
            ptValid = false;
            
            if(!b.placeShip(Point(r, c), i, dir)) {
                //prompt error message if received coordinate is out of bounds
                cout << "The ship can not be placed there." << endl;
            } else {
                placeValid = true;
            }
        }
        placeValid = false;
    }
    return true;
}

bool HumanPlayer::isHuman() const {
    return true;
}

Point HumanPlayer::recommendAttack() {
    int r, c;
    //prompt player to enter the attack coordinate
    cout << "Enter the row and column to attack (e.g. 3 5): ";
    getLineWithTwoIntegers(r, c);
    Point p(r, c);
    return p;
}

void HumanPlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId) {
    
}

void HumanPlayer::recordAttackByOpponent(Point p) {
    
}


//*********************************************************************
//  MediocrePlayer
//*********************************************************************

// TODO:  You need to replace this with a real class declaration and
//        implementation.
//typedef AwfulPlayer MediocrePlayer;
class MediocrePlayer : public Player {
public:
    //constructor
    MediocrePlayer(string nm, const Game& g);
    ~MediocrePlayer();
    virtual bool placeShips(Board& b);
    
    //placeship helper function
    bool placeAllShip(int shipsLeft, Board& b);
    
    
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId);
    
    
    //helper functions
    //generate the cross and store them in a vector
    void generateHits(Point p);
    //check if a point is already being shot
    bool isAttacked(Point p, vector<Point>& v);
    
    virtual void recordAttackByOpponent(Point p);
private:
    //store the the point generated by recommend attack that used to attack
    Point m_pt;
    //store player state
    int m_state;
    //store all possible points on the cross
    vector<Point> validHits;
    //store all points that's being shot on the board
    vector<Point> shotsTaken;
};

MediocrePlayer::MediocrePlayer(string nm, const Game& g) : Player(nm, g), m_state(1) {}

MediocrePlayer::~MediocrePlayer() {};

bool MediocrePlayer::placeShips(Board &b) {
    //return true if all ships are placed
    bool allPlaced = false;
    //counter to make sure placeship does not try more than 50 times
    int counter = 0;
    //total ships on board
    int shipsLeft = game().nShips() - 1;
    
    while(!allPlaced && counter < 50) {
        //block 50% of ships
        b.block();
        //attempt to place all ships
        allPlaced = placeAllShip(shipsLeft, b);
        //unblock board after attempt to place all ships
        b.unblock();
        counter++;
    }
    
    return allPlaced;
}

bool MediocrePlayer::placeAllShip(int shipsLeft, Board &b) {
    bool shipPlaced = true;
    
    if(shipsLeft >= 0) {
        for(int r = 0; r < game().rows(); r ++) {
            for(int c = 0; c < game().cols(); c++) {
                //shiplaced set to true if able to place this ship at a point on the board horizontally
                shipPlaced = b.placeShip(Point(r, c), shipsLeft, HORIZONTAL);
                //if able to place ship
                if(shipPlaced) {
                    //recursively call to see if able to place the another ship in a different point
                    shipPlaced = placeAllShip(shipsLeft - 1, b);
                    //base case
                    //if ultimately shipPlaced is true, return true
                    if (shipPlaced) {
                        return true;
                    }
                }
                if(!shipPlaced){
                    //back track if unable to place ship horizontally
                    b.unplaceShip(Point(r, c), shipsLeft, HORIZONTAL);
                    //this time try vertial direction
                    shipPlaced = b.placeShip(Point(r, c), shipsLeft, VERTICAL);
                    if(shipPlaced) {
                        //if able to place ship then try place it for a different ship
                        shipPlaced = placeAllShip(shipsLeft - 1, b);
                        //base case
                        if (shipPlaced) {
                            return true;
                        }
                    } else {
                        //if not able to place ship, back track
                        b.unplaceShip(Point(r, c), shipsLeft, VERTICAL);
                    }
                }
            }
        }
    }
    
    return shipPlaced;
}

Point MediocrePlayer::recommendAttack() {
    bool isShot = true;
    Point recPt;
    
    //if the old point is being shot
    while(isShot == true) {
        //generate a random point on the board
        recPt = game().randomPoint();
        
        if(shotsTaken.size() > 0) {
            //check if this random point is being shot previously
            if(isAttacked(recPt, shotsTaken)) {
                isShot = true;
            } else {
                isShot = false;
            }
        } else {
            isShot = false;
        }
        //if the player is in state 1 and the recommended point has not been shot already
        if(m_state == 1 && !isShot) {
            //record that recommended point to be shot already
            shotsTaken.push_back(recPt);
            return recPt;
        }
        
        //if player in state 2
        if(m_state == 2) {
            //generate the cross based on a point
            generateHits(m_pt);
            //create a random number less then the size of the cross
            int rand = randInt(int(validHits.size()));
            //randomly access a point  in the cross
            Point tempPt = validHits[rand];
            recPt = tempPt;
            //remove that point from the cross since it is being used
            validHits.erase(validHits.begin() + rand);
            //record that point as being shot already
            shotsTaken.push_back(recPt);
            return recPt;
        }
    }
    shotsTaken.push_back(recPt);
    return recPt;
}

void MediocrePlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId) {
    
    
    if(m_state == 1) {
        //if in state 1 and the point does not hit something
        if(!shotHit) {
            //stays in state 1
            m_state = 1;
        }
        
        if(shotHit && shipDestroyed) {
            m_state = 1;
        }
        
        if(shotHit && !shipDestroyed) {
            m_state = 2;
            m_pt = p;
        }
    } else if(m_state == 2) {
        if(!shotHit) {
            m_state = 2;
        }
        
        if(shotHit && !shipDestroyed) {
            m_state = 2;
        }
        
        if(shotHit && shipDestroyed) {
            m_state = 1;
        }
        
        //if the points in the cross are used up
        //it means the player has destroyed at least one ship
        //need to swicth to state 1 and regenerate a new cross
        if(validHits.size() == 0) {
            m_state = 1;
        }
    }
}

void MediocrePlayer::generateHits(Point p) {
    //clear the cross generated by the previous point
    validHits.clear();
    //the length of one end of the cross should be 4
    for(int i = 1; i < 5; i++) {
        //if the cross is able to go up and not get out of bounds and the point is not being shot already
        if(p.r - i >= 0 && !isAttacked(Point(p.r - i, p.c), shotsTaken)) {
            //added it into the vector storing the points in the cross
            validHits.push_back(Point(p.r - i, p.c));
        }
        //if the cross is able to go down and not get out of bounds and the point is not being shot already
        if (p.r + i <= game().rows() - 1 && !isAttacked(Point(p.r + i, p.c), shotsTaken)) {
            validHits.push_back(Point(p.r + i, p.c));
        }
        //if the cross is able to go left and not get out of bounds and the point is not being shot already
        if (p.c - i >= 0 && !isAttacked(Point(p.r, p.c - i), shotsTaken)) {
            validHits.push_back(Point(p.r, p.c - i));
        }
        
        //if the cross is able to go right and not get out of bounds and the point is not being shot already
        if (p.c + i <= game().cols() - 1 && !isAttacked(Point(p.r, p.c + i), shotsTaken)) {
            validHits.push_back(Point(p.r, p.c + i));
        }
    }
}

bool MediocrePlayer::isAttacked(Point p, vector<Point> &v) {
    for(int i = 0; i < v.size(); i ++) {
        //if the point passed in is equal to the points stored in the shotsTaken vector
        if(v[i].r == p.r && v[i].c == p.c) {
            //return the point is being shot previously
            return true;
        }
    }
    return false;
}

void MediocrePlayer::recordAttackByOpponent(Point p) {};

// Remember that Mediocre::placeShips(Board& b) must start by calling
// b.block(), and must call b.unblock() just before returning.

//*********************************************************************
//  GoodPlayer
//*********************************************************************

// TODO:  You need to replace this with a real class declaration and
//        implementation.
class GoodPlayer : public Player {
public:
    //constructor
    GoodPlayer(string nm, const Game& g);
    virtual ~GoodPlayer();
    virtual bool placeShips(Board& b);
    bool placeAllShip(int shipsLeft, Board& b);
    virtual Point recommendAttack();
    Point doState2();
    void generateHits(Point p);
    void buildCheckerBoard();
    bool isAttacked(Point p, vector<Point> &v);
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
private:
    Point m_pt;
    int m_state;
    
    //0 is up, 1 is right, 2 is down, 3 is left
    int attackDir;
    int attackCounter;
    vector<Point> validHits;
    vector<Point> shotsTaken;
};

GoodPlayer::GoodPlayer(string nm, const Game& g): Player(nm, g), m_state(1), attackDir(0), attackCounter(1) {
//    for(int r = 0; r < game().rows(); r++) {
//        for(int c = 0; c <game().cols(); c++) {
//            if(c % 2 == 0) {
//                checkerBoard.push_back(Point(r, c));
//            }
//        }
//    }
    
}

GoodPlayer::~GoodPlayer(){};

bool GoodPlayer::placeShips(Board& b) {
    //return true if all ships are placed
    bool allPlaced = false;
    //counter to make sure placeship does not try more than 50 times
    int counter = 0;
    //total ships on board
    int shipsLeft = game().nShips() - 1;
    
    if((game().rows() == 10 && game().cols() == 10) && game().nShips() == 5) {
        if( b.placeShip(Point(1, 5), 0, HORIZONTAL) &&  b.placeShip(Point(8, 6), 1, HORIZONTAL) && b.placeShip(Point(2, 2), 2, VERTICAL) && b.placeShip(Point(5, 4), 3, VERTICAL) && b.placeShip(Point(8, 1), 4, VERTICAL)) {
            allPlaced = true;
        } else {
            allPlaced = false;
        }
    } else {
        while(!allPlaced && counter < 50) {
            //block 50% of ships
            b.block();
            //attempt to place all ships
            allPlaced = placeAllShip(shipsLeft, b);
            //unblock board after attempt to place all ships
            b.unblock();
            counter++;
        }
    }
    
    return allPlaced;
}

bool GoodPlayer::placeAllShip(int shipsLeft, Board &b) {
    bool shipPlaced = true;
    if(shipsLeft >= 0) {
        for(int r = 0; r < game().rows(); r ++) {
            for(int c = 0; c < game().cols(); c++) {
                //shiplaced set to true if able to place this ship at a point on the board horizontally
                shipPlaced = b.placeShip(Point(r, c), shipsLeft, HORIZONTAL);
                //if able to place ship
                if(shipPlaced) {
                    //recursively call to see if able to place the another ship in a different point
                    shipPlaced = placeAllShip(shipsLeft - 1, b);
                    //base case
                    //if ultimately shipPlaced is true, return true
                    if (shipPlaced) {
                        return true;
                    }
                }
                if(!shipPlaced){
                    //back track if unable to place ship horizontally
                    b.unplaceShip(Point(r, c), shipsLeft, HORIZONTAL);
                    //this time try vertial direction
                    shipPlaced = b.placeShip(Point(r, c), shipsLeft, VERTICAL);
                    if(shipPlaced) {
                        //if able to place ship then try place it for a different ship
                        shipPlaced = placeAllShip(shipsLeft - 1, b);
                        //base case
                        if (shipPlaced) {
                            return true;
                        }
                    } else {
                        //if not able to place ship, back track
                        b.unplaceShip(Point(r, c), shipsLeft, VERTICAL);
                    }
                }
            }
        }
    }
    return shipPlaced;
}

Point GoodPlayer::recommendAttack() {
    Point recPt;
    
    if(m_state == 1) {
        int counter = 0;
        bool isShot = true;
        while (isShot && counter < 50) {
            bool onChecker = false;
            while(!onChecker) {
                recPt = game().randomPoint();
                if((recPt.r - recPt.c) % 2 == 0) {
                    onChecker = true;
                }
            }
            if(isAttacked(recPt, shotsTaken)) {
                counter++;
                isShot = true;
            } else {
                isShot = false;
            }
        }

        if(counter >= 50) {
            bool isShot = true;
            while(isShot) {
                bool notChecker = false;
                while(!notChecker) {
                    recPt = game().randomPoint();
                    if((recPt.r - recPt.c) % 2 != 0) {
                        notChecker = true;
                    }
                }
                if(isAttacked(recPt, shotsTaken)) {
                    isShot = true;
                } else {
                    isShot = false;
                }
            }
        }
        shotsTaken.push_back(recPt);
        return recPt;
    } else if (m_state == 2) {
        generateHits(m_pt);
        recPt = doState2();
    } else if(m_state == 3){
        validHits.clear();
        attackCounter += 1;
        if(attackDir == 0) {
            if(game().isValid(Point(m_pt.r - attackCounter, m_pt.c))){
                recPt = Point(m_pt.r - attackCounter, m_pt.c);
                if(isAttacked(recPt, shotsTaken)) {
                    attackDir = 2;
                    attackCounter = 1;
                    recPt = Point(m_pt.r + attackCounter, m_pt.c);
                    if(isAttacked(recPt, shotsTaken) || !game().isValid(recPt)) {
                        m_state = 2;
                        recPt = doState2();
                    }
                }
            } else {
                m_state = 2;
                recPt = doState2();
            }
        } else if (attackDir == 1) {
            if(game().isValid(Point(m_pt.r, m_pt.c + attackCounter))){
                recPt = Point(m_pt.r, m_pt.c + attackCounter);
                if(isAttacked(recPt, shotsTaken)) {
                    attackDir = 3;
                    attackCounter = 1;
                    recPt = Point(m_pt.r, m_pt.c - attackCounter);
                    if(isAttacked(recPt, shotsTaken) || !game().isValid(recPt)) {
                        m_state = 2;
                        recPt = doState2();
                    }
                }
            } else {
                m_state = 2;
                recPt = doState2();
            }
        } else if (attackDir == 2) {
            if(game().isValid(Point(m_pt.r + attackCounter, m_pt.c))){
                recPt = Point(m_pt.r + attackCounter, m_pt.c);
                if(isAttacked(recPt, shotsTaken)) {
                    attackDir = 0;
                    attackCounter = 1;
                    recPt =Point(m_pt.r - attackCounter, m_pt.c);
                    if(isAttacked(recPt, shotsTaken) || !game().isValid(recPt)) {
                        m_state = 2;
                        recPt = doState2();
                    }
                }
            } else {
                m_state = 2;
                recPt = doState2();
            }
        } else if (attackDir == 3) {
            if(game().isValid(Point(m_pt.r, m_pt.c - attackCounter))){
                recPt = Point(m_pt.r, m_pt.c - attackCounter);
                if(isAttacked(recPt, shotsTaken)) {
                    attackDir = 1;
                    attackCounter = 1;
                    recPt = Point(m_pt.r, m_pt.c + attackCounter);
                    if(isAttacked(recPt, shotsTaken) || !game().isValid(recPt)) {
                        m_state = 2;
                        recPt = doState2();
                    }
                }
            } else {
                m_state = 2;
                recPt = doState2();
            }
        }
        shotsTaken.push_back(recPt);
        return recPt;
    }
    shotsTaken.push_back(recPt);
    return recPt;
}

void GoodPlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId) {
    
    if(m_state == 1) {
        if(!shotHit) {
            m_state = 1;
        }

        if(shotHit && !shipDestroyed) {
            m_pt = p;
            m_state = 2;
        }

        if(shotHit && shipDestroyed) {
            m_state = 1;
        }
    } else if(m_state == 2) {
        if(!shotHit) {
            m_state = 2;
        }

        if(shotHit && !shipDestroyed) {
            m_state = 3;
            if(p.r < m_pt.r) {
                attackDir = 0;
            } else if(p.r > m_pt.r) {
                attackDir = 2;
            } else if(p.c > m_pt.c) {
                attackDir = 1;
            } else if(p.c < m_pt.c) {
                attackDir = 3;
            }
            attackCounter = 1;
        }

        if(shotHit && shipDestroyed) {
            m_state = 1;
        }

        if(validHits.empty()) {
            m_state = 1;
        }
    } else {
        if(!shotHit) {
            if(attackDir == 0) {
                attackDir = 2;
            } else if(attackDir == 1) {
                attackDir = 3;
            } else if(attackDir == 2) {
                attackDir = 0;
            } else if(attackDir == 3) {
                attackDir = 1;
            }
            m_state = 3;
        }

        if(shotHit && !shipDestroyed) {
            m_state = 3;
        }

        if(shotHit && shipDestroyed) {
            m_state = 2;
        }
    }
}

void GoodPlayer::recordAttackByOpponent(Point p) {
    
}

Point GoodPlayer::doState2() {
    Point recPt;
    bool isShot = true;
    int rand = -1;
    while(isShot) {
        if(!validHits.empty()) {
            rand = randInt(int(validHits.size()));
            recPt = validHits[rand];
            if(isAttacked(recPt, shotsTaken)) {
                isShot = true;
                validHits.erase(validHits.begin() + rand);
            } else {
                isShot = false;
            }
        } else {
            m_state = 1;
            int counter = 0;
            bool isShot = true;
            while (isShot && counter < 50) {
                bool onChecker = false;
                while(!onChecker) {
                    recPt = game().randomPoint();
                    if((recPt.r - recPt.c) % 2 == 0) {
                        onChecker = true;
                    }
                }
                if(isAttacked(recPt, shotsTaken)) {
                    counter++;
                    isShot = true;
                } else {
                    isShot = false;
                }
            }

            if(counter >= 50) {
                bool isShot = true;
                while(isShot) {
                    bool notChecker = false;
                    while(!notChecker) {
                        recPt = game().randomPoint();
                        if((recPt.r - recPt.c) % 2 != 0) {
                            notChecker = true;
                        }
                    }
                    if(isAttacked(recPt, shotsTaken)) {
                        isShot = true;
                    } else {
                        isShot = false;
                    }
                }
            }
            shotsTaken.push_back(recPt);
            return recPt;
        }
    }
    validHits.erase(validHits.begin() + rand);
    shotsTaken.push_back(recPt);
    return recPt;
}

void GoodPlayer::generateHits(Point p) {
    if(p.r - 1 >= 0 && !isAttacked(Point(p.r - 1, p.c), shotsTaken)) {
        //added it into the vector storing the points in the cross
        validHits.push_back(Point(p.r - 1, p.c));
    }
    //if the cross is able to go down and not get out of bounds and the point is not being shot already
    if (p.r + 1 <= game().rows() - 1 && !isAttacked(Point(p.r + 1, p.c), shotsTaken)) {
        validHits.push_back(Point(p.r + 1, p.c));
    }
    //if the cross is able to go left and not get out of bounds and the point is not being shot already
    if (p.c - 1 >= 0 && !isAttacked(Point(p.r, p.c - 1), shotsTaken)) {
        validHits.push_back(Point(p.r, p.c - 1));
    }
    
    //if the cross is able to go right and not get out of bounds and the point is not being shot already
    if (p.c + 1 <= game().cols() - 1 && !isAttacked(Point(p.r, p.c + 1), shotsTaken)) {
        validHits.push_back(Point(p.r, p.c + 1));
    }
    
}

bool GoodPlayer::isAttacked(Point p, vector<Point> &v) {
    for(int i = 0; i < v.size(); i ++) {
        //if the point passed in is equal to the points stored in the shotsTaken vector
        if(v[i].r == p.r && v[i].c == p.c) {
            //return the point is being shot previously
            return true;
        }
    }
    return false;
}


//typedef AwfulPlayer GoodPlayer;

//*********************************************************************
//  createPlayer
//*********************************************************************

Player* createPlayer(string type, string nm, const Game& g)
{
    static string types[] = {
        "human", "awful", "mediocre", "good"
    };
    
    int pos;
    for (pos = 0; pos != sizeof(types)/sizeof(types[0])  &&
                                                     type != types[pos]; pos++)
        ;
    switch (pos)
    {
      case 0:  return new HumanPlayer(nm, g);
      case 1:  return new AwfulPlayer(nm, g);
      case 2:  return new MediocrePlayer(nm, g);
      case 3:  return new GoodPlayer(nm, g);
      default: return nullptr;
    }
}


