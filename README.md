This project includes the cpp source code and various ASCII art images for the game Wump.2. Game play instructions follow. Find a Let's Play and code intro at https://youtu.be/hexYrDQqmLA.

Premise derived from Hunt the Wumpus written in BASIC by Gregory Yob, 1973.
 
 Welcome to 'Wump.2' a pseudo successor to 'Hunt the Wumpus'! 
 The enemies live in a cave of multiple levels, each with 20 rooms. 
 Each room has 3 tunnels leading to other rooms. (Google "dodecahedron")
 After beating the Wumpus on the last level, treasure awaits!

 Hazards
 Bottomless pits - two rooms have bottomless pits. Don't fall in and die!
 Super bats - two other rooms have super bats. If you enter one, a bat grabs you
 and takes you to some other room at random. Once the bat has moved you, 
 it flies to another random room.

 Enemies
 The enemies are not bothered by hazards as they are otherworldly. Two things
 will wake an enemy though: you attacking in a neighboring room or entering 
 their room. If an enemy wakes, it may move one room (probability varies per
 enemy). If it moves into the room you're in, it may kill you, though you have
 a chance to fend it off with a weapon. After killing an enemy, you are magically
 transported to the next level. 

 You 
 Each turn you may move, attack, or quit. Moving goes one room at a time.
 To be able to attack, you must find the weapon in the level.
 Attacking requires you to pick the room to direct your attack/shot.
 
 Warnings
 When you are one room away from an enemy or hazard, messages are displayed:
     Bat: 'Bats nearby'
     Pit: 'I feel a draft'
     Enemies each have a unique warning!
