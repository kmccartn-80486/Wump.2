/*
 * Wump.2 - Premise based on Hunt the Wumpus, Gregory Yob, 1973
 * Author - Kevin McCartney, 2024
 * Introduction to the code on YouTube; search for Wump.2
 *
 * For a level:
 * 1. Dodecahedron map so 20 rooms & 3 adjacent rooms each
 * 2. 1 enemy/boss
 * 3. 1 weapon
 * 4. 2 hazs and 2 relocs
 */

#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <cmath>
#include <fstream>
#include <cstdlib>
#include <exception>
#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif
using namespace std;

const bool DEBUG = false;
const bool INTRO = true;

// LevelResult passed from Level to Game
struct LevelResult {
    bool climbDown = false;
    bool climbUp   = false;
    bool wonGame   = false;
    bool died      = false;
    bool killedEnemy = false;
};

// Cross-platform wait
void CPWait(int seconds) {
    #ifdef _WIN32
        Sleep(seconds * 1000);
    #else
        sleep(seconds);
    #endif
}

// Output file that is ascii (text, ~graphics)
void CatFile(const std::string& filename, int waitSeconds) {
    // Clear the screen (platform specific)
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
    ifstream file(filename);
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            cout << line << endl;
        }
        file.close();
    } else {
        cout << "Error: Could not open file " << filename << endl;
    }
    if (waitSeconds > 0) {
        CPWait(waitSeconds);
    }
}

class Enemy {
private:
    int lev;
public:
    string warningMsg;
    string killedItMsg;
    string killedYouMsg;
    float pMove;

    Enemy(int);
    void StartledSequence();
    void KilledItSequence();
    void KilledYouSequence();
};

// conx
Enemy::Enemy(int thisLev) {
    lev = thisLev;    

    // e.g., use pMove = 1 to always move
    if (lev == 0) {
        // SPIDER
        warningMsg = "You hear a light tap-tap coming towards you...";
        killedItMsg = "SHLACHING.. Thump! \nGood shot! You killed the Spider!";
        killedYouMsg = "The Spider bit you and quickly wrapped you in a cocoon web!"; 
        pMove = 1;
    } else if (lev == 1) {
        // SNAKE
        warningMsg = "You hear something slither on the ground...";
        killedItMsg = "KA-POW.. Slice! \nNice! You killed the Snake!";
        killedYouMsg = "The Snake bit you! You've been injected with venom!";
        pMove = 1;
    } else if (lev == 2) {
        // FROST MAGE
        warningMsg = "You feel a sudden chill...";
        killedItMsg = "SS-TCCAAH.. Pow! \nVictory! You killed the Frost Mage!";
        killedYouMsg = "The Frost Mage cast a spell that instantaneously froze you to the core!";
        pMove = 1;
    } else if (lev == 3) {
        // WUMPUS
        warningMsg = "You smell a horrid stench...";
        killedItMsg = "ARGH.. Splat! \nCongratulations! You killed the Wumpus!";
        killedYouMsg = "The Wumpus ate you!";
        pMove = 0.75;
    }
}

void Enemy::StartledSequence() {
    if (lev == 0) {
        // SPIDER
        cout << "Miss! But you startled the Spider" << endl;
    } else if (lev == 1) {
        // SNAKE
        cout << "Miss! But you startled the Snake" << endl;
    } else if (lev == 2) {
        // FROST MAGE
        cout << "Miss! But you startled the Frost Mage" << endl;
    } else if (lev == 3) {
        // WUMPUS
        cout << "Miss! But you startled the Wumpus" << endl;
    }
}

void Enemy::KilledItSequence() {
    if (lev == 0) {
        // ARROW - SPIDER
        CatFile("ascii_img/arrow", 1);
        CatFile("ascii_img/spider.rotate", 1);
    } else if (lev == 1) {
        // SPADE - SNAKE
        CatFile("ascii_img/spade", 1);
        CatFile("ascii_img/snake.rotate", 1);
    } else if (lev == 2) {
        // CLUB - FROST MAGE
        CatFile("ascii_img/club", 1);
        CatFile("ascii_img/frost.mage", 1);
        CatFile("ascii_img/explosion", 1);
    } else if (lev == 3) {
        // SWORD - WUMPUS
        CatFile("ascii_img/sword", 1);
        CatFile("ascii_img/wump.rotate", 1);
    }
    cout << killedItMsg << endl;
}

void Enemy::KilledYouSequence() {
    if (lev == 0) {
        // SPIDER
        CatFile("ascii_img/spider", 1);
    } else if (lev == 1) {
        // SNAKE
        CatFile("ascii_img/snake", 1);
    } else if (lev == 2) {
        // FROST MAGE
        CatFile("ascii_img/frost.mage", 1);
    } else if (lev == 3) {
        // WUMPUS
        CatFile("ascii_img/wump.partial", 1);
        CatFile("ascii_img/wump.blur", 1);
        CatFile("ascii_img/wump", 1);
    }
    cout << killedYouMsg << endl;
}

class Weapon {
private:
    int lev;
public:
    bool haveWeapon;
    bool isLimited;
    int  quantity;
    float pToHit;

    Weapon(int);
    void HaveWeaponSequence();
};

// conx
Weapon::Weapon(int thisLev) {
    lev = thisLev;
    // use 1 for always hit
    if (lev == 0) { 
        // BOW
        haveWeapon = false; // effectively not until you find an arrow
        isLimited = true;
        quantity = 0;  // starting number of arrows
        pToHit = 1;  
    } else if (lev == 1) {
        // SPADE
        haveWeapon = false;
        isLimited = false;
        quantity = 0;
        pToHit = 0.75;
    } else if (lev == 2) { 
        // CLUB
        haveWeapon = false;
        isLimited = false;
        quantity = 0;
        pToHit = 1;
    } else if (lev == 3) { 
        // SWORD
        haveWeapon = false;
        isLimited = false;
        quantity = 0;
        pToHit = 0.5;
    }
} 

void Weapon::HaveWeaponSequence() {
    if (!haveWeapon) {
        if (lev == 0) {
            // BOW
            CatFile("ascii_img/arrow", 1);
            CatFile("ascii_img/bow", 1);
            cout << "You found two arrows!" << endl;
            quantity = 2;
        } else if (lev == 1) {
            // SPADE
            CatFile("ascii_img/spade", 1);
            cout << "You found the spade!" << endl;
        } else if (lev == 2) {
            // CLUB
            CatFile("ascii_img/club", 1);
            cout << "You found the club!" << endl;
        } else if (lev == 3) {
            // SWORD
            CatFile("ascii_img/sword", 1);
            cout << "You found the sword!" << endl;
        }
    }
    haveWeapon = true;
}

class Hazard {
private:
    int lev;
public:
    string nearMsg;

    Hazard(int);
    void KillSequence();
};

Hazard::Hazard(int thisLev) {
    lev = thisLev;
    nearMsg = "You feel a draft...";

/* FUTURE
    if (lev == 0) {
        // PIT
        nearMsg = "You feel a draft...";
    } else if (lev == 1) {
        // 
        nearMsg = "...";
    }
*/
}

void Hazard::KillSequence() {
    CatFile("ascii_img/pit", 1);
    cout << "@#$*! You fell in a pit!" << endl;

/* FUTURE
    if (lev == 0) {
        // PIT
        CatFile("ascii_img/pit", 1);
        cout << "@#$*! You fell in a pit!" << endl;
    } else if (lev == 1) {
        // 
        CatFile("", 1);
        cout << "..." << endl;
    }
*/
}

class Relocator {
private:
    int lev;
public:
    string nearMsg;
    string savedMsg;
    string movedMsg;

    Relocator(int);
    void RelocateSequence();
};

Relocator::Relocator(int thisLev) {
    lev = thisLev;
    nearMsg = "Bats nearby...";
    savedMsg = "Luckily, the bats saved you from a hazard!";
    movedMsg = "The bats moved you to room ";

/* FUTURE
    if (lev == 0) {
        // BAT
        nearMsg = "Bats nearby...";
        savedMsg = "Luckily, the bats saved you from a hazard!";
        movedMsg = "The bats moved you to room ";
    } else if (lev == 1) {
        // 
        nearMsg = "...";
        savedMsg = "...!";
        movedMsg = "";
    }
*/
};

void Relocator::RelocateSequence() {
    CatFile("ascii_img/bat", 1);
    cout << "Snatched by superbats!" << endl;

/* FUTURE
    if (lev == 0) {
        // BAT
        CatFile("ascii_img/bat", 1);
        cout << "Snatched by superbats!" << endl;
    } else if (lev == 1) {
        // 
        CatFile("", 1);
        cout << "..." << endl;
    }
*/
}

class Level {
private:
    int numRooms;
    int currentRoom, startingPosition; 
    int enemyRoom, relocRoom1, relocRoom2, hazRoom1, hazRoom2, weaponRoom; // Stores the room numbers of the respective
    int enemyStart, reloc1Start, reloc2Start;
    int myLevelNum;
    LevelResult result; // climb down, up, win, die, kill enemy
    Weapon* myWeapon;
    Enemy*  myEnemy;
    Hazard* myHaz;
    Relocator* myReloc;

    // constant value 2d array to represent the dodecahedron room structure
    // rooms are thus 0 - 19
    constexpr static int adjacentRooms[20][3] = {
        {1, 4, 7},    {0, 2, 9},   {1, 3, 11},   {2, 4, 13},   {0, 3, 5},
        {4, 6, 14},   {5, 7, 16},  {0, 6, 8},    {7, 9, 17},   {1, 8, 10},
        {9, 11, 18},  {2, 10, 12}, {11, 13, 19}, {3, 12, 14},  {5, 13, 15},
        {14, 16, 19}, {6, 15, 17}, {8, 16, 18},  {10, 17, 19}, {12, 15, 18}
    };

    void PlaceHazs();
    void PlaceRelocs();
    void PlaceEnemy();
    void PlacePlayer();
    void PlaceWeapon();
    bool IsValidMove(int);
    bool IsRoomAdjacent(int, int);
    int  Move(int);
    void InspectCurrentRoom();
    void PerformAction(int);
    void MoveStartledEnemy(int);
    void PlayAgain();
    void CurrentRoomEnemyRoom();

public:
    Level();
    Level(int);
    void SetLevelNum(int);
    LevelResult PlayLevel();

    ~Level() {
        if (myEnemy != nullptr) {
            delete myEnemy;
            myEnemy = nullptr;
        }
        if (myWeapon != nullptr) {
            delete myWeapon;
            myWeapon = nullptr;
        }
        if (myHaz != nullptr) {
            delete myHaz;
            myHaz = nullptr;
        }
        if (myReloc != nullptr) {
            delete myReloc;
            myReloc = nullptr;
        }
    }
};

// conx
Level::Level() {
}

// conx
Level::Level(int myLev) {
    numRooms = 20;
    myLevelNum = myLev;

    // construct these every time / delete these every time
    myEnemy  = new Enemy(myLevelNum);
    myWeapon = new Weapon(myLevelNum);
    myHaz    = new Hazard(myLevelNum);
    myReloc  = new Relocator(myLevelNum);
}

// This function will place two relocs throughout the map
// this ensures that the relocs will not be place in the same
// room as another reloc or the enemy
void Level::PlaceRelocs() {
    bool validRoom = false;
    while (!validRoom) {
        relocRoom1 = rand() % 19 + 1;
        if (relocRoom1 != enemyRoom)
            validRoom = true;
    }

    validRoom = false;
    while (!validRoom) {
        relocRoom2 = rand() % 19 + 1;
        if (relocRoom2 != enemyRoom && relocRoom2 != relocRoom1)
            validRoom = true;
    }

    reloc1Start = relocRoom1;
    reloc2Start = relocRoom2;
if (DEBUG) cout << "RELOC ROOM 1: " << to_string(relocRoom1) << endl;
if (DEBUG) cout << "RELOC ROOM 2: " << to_string(relocRoom2) << endl;
}

// this function randomly places the hazs
// throughout the map excluding room 0
void Level::PlaceHazs() {
    hazRoom1 = rand() % 19 + 1;
    hazRoom2 = rand() % 19 + 1;
if (DEBUG) cout << "HAZ ROOM 1: " << to_string(hazRoom1) << endl;
if (DEBUG) cout << "HAZ ROOM 2: " << to_string(hazRoom2) << endl;
}

// this function randomly places the enemy in a room
// without being in room number 0
void Level::PlaceEnemy() {
    enemyRoom = rand() % 19 + 1;
    enemyStart = enemyRoom;
if (DEBUG) cout << "ENEMY ROOM: " << to_string(enemyRoom) << endl;
}

// place the player in room 0
void Level::PlacePlayer() {
    startingPosition = 0;
    currentRoom = Move(0);
}

// 
void Level::PlaceWeapon() {
    bool validRoom = false;
    while (!validRoom) {
        weaponRoom = rand() % 19 + 1; 
        if (weaponRoom != enemyRoom && weaponRoom != hazRoom1 && weaponRoom != hazRoom2)
            validRoom = true;
    }
if (DEBUG) cout << "WEAPON ROOM: " << to_string(weaponRoom) << endl;
}

// This is a  method that checks if the user inputted a valid room to move to or not.
// The room number has to be between 0 and 19, but also must be adjacent to the current room.
bool Level::IsValidMove(int roomID) {
    if ((roomID < 0) || \
        (roomID > numRooms-1) || \
        (!IsRoomAdjacent(currentRoom, roomID))) {
        return false;
    }
    return true;
}

// This method returns true if roomB is adjacent to roomA, otherwise returns false.
// It is a helper method that loops through the adjacentRooms array to check.
// It will be used throughout the app to check if we are next to the enemy, relocs, or hazs
// as well as check if we can make a valid move.
bool Level::IsRoomAdjacent(int roomA, int roomB)
{
    for (int j = 0; j < 3; j++) {
        if (adjacentRooms[roomA][j] == roomB) {
          return true;
        }
    }
    return false;
}

// This method moves the player to a new room and returns the new room. 
int Level::Move(int newRoom)
{
    // currently performs no checks on its own
    return newRoom;
}

// Inspects the current room.
// This method check for Hazards such as being in the same room as the enemy, relocs, or hazs
// It also checks if you are adjacent to a hazard and handle those cases
// Finally it will just print out the room description
void Level::InspectCurrentRoom() {
    CurrentRoomEnemyRoom();
    if (result.killedEnemy || result.died)
        return;
    
    if (currentRoom == weaponRoom) {     // can find weapon then be picked up by relocs, in that order
        myWeapon->HaveWeaponSequence();
    } 
    if (currentRoom == relocRoom1 || currentRoom == relocRoom2) {
        int roomRelocsLeft = currentRoom;
        bool validNewRelocRoom = false;
        bool isRelocRoom = false;

        myReloc->RelocateSequence();
        if (currentRoom == hazRoom1 || currentRoom == hazRoom2)
            cout << myReloc->savedMsg << endl;
        while (!isRelocRoom) {
            currentRoom = Move(rand() % 19 + 1);
            if (currentRoom != relocRoom1 && currentRoom != relocRoom2)
                isRelocRoom = true;
        }
        myReloc->RelocateSequence();
        cout << myReloc->movedMsg << endl;
        cout << currentRoom << endl;

        InspectCurrentRoom();

        if (roomRelocsLeft == relocRoom1) {
            while (!validNewRelocRoom) {
                relocRoom1 = rand() % 19 + 1;
                if (relocRoom1 != enemyRoom && relocRoom1 != currentRoom)
                    validNewRelocRoom = true;
            }
        } else {
            while (!validNewRelocRoom) {
                relocRoom2 = rand() % 19 + 1;
                if (relocRoom2 != enemyRoom && relocRoom2 != currentRoom)
                    validNewRelocRoom = true;
            }
        }
    } else if (currentRoom == hazRoom1 || currentRoom == hazRoom2) {
        myHaz->KillSequence();
        PlayAgain();
    } else {
        cout << "You are in room ";
        cout << currentRoom << endl;
        if (IsRoomAdjacent(currentRoom, enemyRoom)) {
            cout << myEnemy->warningMsg << endl;
        }
        if (IsRoomAdjacent(currentRoom, relocRoom1) || IsRoomAdjacent(currentRoom, relocRoom2)) {
            cout << myReloc->nearMsg << endl;
        }
        if (IsRoomAdjacent(currentRoom, hazRoom1) || IsRoomAdjacent(currentRoom, hazRoom2)) {
            cout << myHaz->nearMsg << endl;
        }
        cout << "Tunnels lead to rooms " << endl;
        for (int j = 0; j < 3; j++)
        {
            cout << adjacentRooms[currentRoom][j];
            cout << " ";
        }
        cout << endl;
    }
}

// PerformAction
void Level::PerformAction(int cmd) {
    int newRoom;
    switch (cmd) {
        case 1:     // move
            cout << "Which room? " << endl;
            try {
                cin >> newRoom;
                // Check if the user input a valid room id, then simply tell the player to move there.
                if (IsValidMove(newRoom)) {
                    currentRoom = Move(newRoom);
                    InspectCurrentRoom();
                } else {
                    cout << "You cannot move there." << endl;
                }
            }
            catch (...) { // Try...Catch block will catch if the user inputs text instead of a number.
                cout << "You cannot move there." << endl;
            }
            break;
        case 2:     // attack
            if (myWeapon->haveWeapon && (!myWeapon->isLimited || (myWeapon->isLimited && myWeapon->quantity > 0))) {
                cout << "Which room? " << endl;
                try {
                    cin >> newRoom;
                    // Check if the user input a valid room id, then attack into that room.
                    if (IsValidMove(newRoom)) {
                        myWeapon->quantity--;
                        if (newRoom == enemyRoom) {
                            myEnemy->KilledItSequence();
                            cout << "Press 0 to return to the game menu." << endl;
                            result.killedEnemy = true;
                            cin >> newRoom;
                            cin.clear(); cin.ignore(10000, '\n');
                        } else {
                            MoveStartledEnemy(enemyRoom);
                            if (myWeapon->isLimited) {
                                cout << "Ammunition Left: ";
                                cout << myWeapon->quantity << endl;
                            }
                            CurrentRoomEnemyRoom();
                        }
                    } 
                }
                catch (...) { // Try...Catch block will catch if the user inputs text instead of a number.
                    cout << "You cannot attack there." << endl;
                }
            } else {
                    cout << "You cannot attack - no or limited weapon, or out of ammo." << endl;
            }
            break;
        case 3:     // quit
            cout << "Quiting level." << endl;
            break;
        default:
            cout << "You cannot do that. You can move, attack or quit." << endl;
            break;
    }
}

// this function moves the enemy randomly to a room that is adjacent to
// the enemy's current position
void Level::MoveStartledEnemy(int roomNum) {
if (DEBUG) cout << "STARTLED ENEMY IN ROOM " << to_string(roomNum) << endl;
    myEnemy->StartledSequence();
    if ((rand() % 100) < int(round(myEnemy->pMove * 100))) {    // move if (0-99) turns out less than prob * 100
        int rando = rand() % 3;
        enemyRoom = adjacentRooms[roomNum][rando];
        cout << "And the enemy moved!" << endl;
if (DEBUG) cout << "MOVED ENEMY TO ROOM " << to_string(enemyRoom) << endl;
    }
}

// When in the same room with the enemy:
// If have the weapon and hit, it dies. 
// If don't have the weapon or have it and miss, you die.
void Level::CurrentRoomEnemyRoom() {
    if (currentRoom == enemyRoom)
    {
        if (myWeapon->haveWeapon && (!myWeapon->isLimited || (myWeapon->isLimited && myWeapon->quantity > 0))) {
            // do attack if (0-99) turns out less than prob * 100
            if ((rand() % 100) < int(round(myWeapon->pToHit * 100))) { 
                myEnemy->KilledItSequence();
                result.killedEnemy = true;
            } else {
                myEnemy->KilledYouSequence();
                PlayAgain();
            }
        } else { // no weapon/no ammo
            myEnemy->KilledYouSequence();
            PlayAgain();
        }
    }
}

// This restarts the level from the beginning
void Level::PlayAgain() {
    int reply;
    cout << "Would you like to replay the same level? Enter 0 to play again." << endl;
    cin >> reply;
    if (reply == 0) {
        myWeapon->quantity = 0; // lose all ammo
        myWeapon->haveWeapon = false;
        currentRoom = startingPosition;
        enemyRoom = enemyStart;
        relocRoom1 = reloc1Start;
        relocRoom2 = reloc2Start;
        cout << "Try not to die this time. \n" << endl;
        InspectCurrentRoom();
    } else {
        result.died = true;
    }
}

// PlayLevel() method starts the level.
// It houses the level loop and when PlayLevel() quits the level has ended.
LevelResult Level::PlayLevel()
{
    cout << "Loading level " << to_string(myLevelNum) << " ..." << endl;

    // Initialize the level
    int choice;
    enemyStart = reloc1Start = reloc2Start = -1;
    PlaceEnemy();
    PlaceRelocs();
    PlaceHazs();
    PlacePlayer();
    PlaceWeapon();

    result.climbDown = false;
    result.climbUp   = false;
    result.wonGame   = false;
    result.died      = false;
    result.killedEnemy = false;

    // Inspects the initial room
    InspectCurrentRoom();

    // level loop
    while ((!result.died && !result.killedEnemy)) { // while you & wump are alive
        cout << "Enter an action choice." << endl;
        cout << "1) Move" << endl;
        cout << "2) Attack" << endl;
        cout << "3) Quit" << endl;
        cout << ">>> ";
        cout << "Please make a selection: ";
        try {
            cin >> choice;
            switch (choice) {
                case 1: 
                case 2:
                    PerformAction(choice);
                    break;
                case 3:
                    result.died = true; // quit really
                    break;
                default:
                    cout << "Invalid choice. Please try again." << endl;
                    cin.clear(); cin.ignore(10000, '\n');
                    break;
            }
        }
        catch (...) {
            cout << "Invalid choice. Please try again." << endl;
            cin.clear(); cin.ignore(10000, '\n');
        }
    } // while(!result.died && !result.killedEnemy)
    return result;
}

class Game {
private:
    Level* levelsArray;
    int numLevels;
    int currentLev;

public:
    Game(int);
    ~Game();
    void PrintInstructions();
    void StartGame();
};

// conx
Game::Game(int numLevs) {
    numLevels = numLevs;
    levelsArray = new Level[numLevels];

    for (int i=0; i<numLevels; i++) {
        //levelsArray[i].SetLevelNum(i);
        new (&levelsArray[i]) Level(i);
    }
    currentLev = 0;
}

Game::~Game() {
    if (levelsArray != nullptr) {
        delete[] levelsArray;
        levelsArray = nullptr;
    }
}

void Game::PrintInstructions() {
    char wait;
    CatFile("ascii_img/instructions", 0);
    cin >> wait;
}

// game loop
void Game::StartGame() {
    int choice;
    bool keepPlaying = true;
    bool won = false;

    cout << "Welcome to Wump.2" << endl;
    while (keepPlaying and !won) {
        LevelResult returnedResult;

        cout << "1> Play Game Level " << to_string(currentLev) << endl;
        cout << "2> Print Instructions" << endl;
        cout << "3> Quit Game" << endl;
        cout << "Please make a selection> ";
        try {
            cin >> choice;
            switch (choice) {
                case 1:
                    cout << "Playing level " << currentLev << endl;
                    returnedResult = levelsArray[currentLev].PlayLevel();
                    if (returnedResult.climbDown) { // not currently used
                        cout << "climbDown......" << endl;
                        currentLev++;
                    }
                    else if (returnedResult.climbUp) { // not currently used
                        cout << "climbUp......" << endl;
                        currentLev--;
                    }
                    /*else if (returnedResult.wonGame) {    // not currently used
                        cout << "wonGame......" << endl;
                    }
                    */
                    else if (returnedResult.died) { // died or quit
                        cout << "Exiting the current level." << endl;
                    }
                    else if (returnedResult.killedEnemy) {
                        cout << "You beat the level and advanced to the next one!" << endl;
                        currentLev++;
                        if (currentLev == numLevels) won = true;    // i.e., advanced beyond the last level
                    }
                    break;
                case 2:
                    PrintInstructions();
                    break;
                case 3:
                    cout << "Quitting game." << endl;
                    keepPlaying = false;
                    break;
                case 4:
                    cout << "SELECT LEVEL" << endl;
                    cin >> currentLev;
                    break;
                default:
                    cout << "Invalid choice. Please try again." << endl;
                    cin.clear(); cin.ignore(10000, '\n');
                    break;
            }
        }
        catch (...) {
            cout << "Invalid choice. Please try again." << endl;
            cin.clear(); cin.ignore(10000, '\n');
        }
    } // while (keepPlaying and !won)

    if (won) {
        CatFile("ascii_img/gold", 1);
        cout << "You won the game!" << endl;
    }
}

int main() {
//    try {
        srand (time(NULL));     // only necessary to do this once

        if (INTRO) {
            CatFile("ascii_img/title", 1);
            CatFile("ascii_img/bow", 1);
            CatFile("ascii_img/arrow", 1);
            CatFile("ascii_img/spade", 1);
            CatFile("ascii_img/club", 1);
            CatFile("ascii_img/sword", 1);
            CatFile("ascii_img/map", 1);
            CatFile("ascii_img/levels", 1);
            CatFile("ascii_img/passage", 1);
            CatFile("ascii_img/bat", 1);
            CatFile("ascii_img/pit", 1);
            CatFile("ascii_img/spider", 1);
            CatFile("ascii_img/snake", 1);
            CatFile("ascii_img/frost.mage", 1);
            CatFile("ascii_img/wump.partial", 1);
            CatFile("ascii_img/wump.blur", 1);
            CatFile("ascii_img/wump", 1);
            CatFile("ascii_img/gold", 1);
            CatFile("ascii_img/title", 1);
            CatFile("ascii_img/bump.word", 1);
            CatFile("ascii_img/the.word", 1);
            CatFile("ascii_img/wump.word", 1);
        }

        // create game object, pass number of levels
        Game game(4);
        game.StartGame();
//    }
//    catch (const exception& e) {
//        cerr << "Exception caught: " << e.what() << endl;
//    }
}
