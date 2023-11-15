# CMD Stealth
## Overview
CMD Stealth is a command line game written in C++, using Visual Studio 2019 (& 2022) and Windows 10 (64-bit). It's designed to be a top-down stealth game where you, the player, must navigate around the map and attempt to pickpocket enemies in search of the key to the treasure. The treasure will be hidden to you at first, and marked as soon as you've found it. Once the treasure has been located, opened and obtained with the key, the player may seek the exit and leave the level. The player must navigate their way around the map and around enemies without alerting them too much - should the game's detection meter fill up, it's game over for the player.

Gold can optionally be collected along the way - and each game is timed to encourage the player to beat the level as quickly as possible. These two things in conjunction will increase the player's score, which they can attempt to beat each time.

![](https://github.com/tcollyer1/cmd-stealth-game/blob/dev/media/game.png)
![](https://github.com/tcollyer1/cmd-stealth-game/blob/dev/media/startup.png)

[YouTube walkthrough](https://www.youtube.com/watch?v=ezBTuYP-o9o) | [Repo](https://github.com/tcollyer1/cmd-stealth-game)

## Resources & Libraries
- [ASCII logo generator](https://patorjk.com/software/taag/)
- [Sound effects](https://www.zapsplat.com) (ZapSplat)

No external or special libraries were used, other than the following (standard) additional libraries:
- **Winmm.lib** (PlaySound function)
- **conio.h** (console I/O)
- **fstream/iostream** (file handling)
- **vector** (vectors)
- **io.h/fcntl.h** (configuring file mode for Unicode characters)
- **string**

---

## Gameplay
### Key Mechanics
- enemy detection system - acquiring the player's last known location and moving to it, eventually resetting detection for that enemy when their awareness timer expires
- ability to lure enemies into other locations using the above mechanic, which can aid with gold collection or looking for the treasure
- marking the treasure for the player once they have actually found it 
- full detection means game over
- tile loudness/brightness system
- a new, randomly generated map each game - player, treasure, exit, enemy, wall and tile spawns are randomised with each new iteration
- wall & entity-to-entity collision
- walls blocking enemy line of sight
- enemy KOs (for a limited time)
- optional gold collection
- core game progression - obtaining the key, finding the treasure, unlocking the treasure & activating the exit
- differing sound effects when player steps on a tile of a certain type for audio feedback
- game timer/scoring system - collecting gold and completing the level faster is encouraged
- enemy navigation - random movement VS specific movement once they are suspicious of the player either visually or audibly, navigating around other entities and walls


### Tiles
The walkable tiles on the map have a **light level** and **terrain type** property. Each tile will either be `BRIGHT`, `MEDIUM` or `DARK`, and will also be either `HARD` (`*`) or `SOFT` (`.`).

`BRIGHT` tiles are light grey in colour, where `MEDIUM` and `DARK` tiles are black. `DARK` tiles are completely black - so you won't be able to tell whether it's `HARD` or `SOFT`, to account for some realism, until you step on it, providing a sound cue.

`BRIGHT` tiles mean the enemy will be able to see you, should you be in their line of sight; `MEDIUM` means it will be more difficult for an enemy to spot you (you must be closer to them to raise suspicion), and `DARK` renders you completely invisible. Should an enemy bump into you, however - you'll be immediately spotted.

`HARD` (`*`) tiles will emit noise when walked on, alerting the enemies audibly when within their hearing range. `SOFT` (`.`) tiles are impossible to hear when walked on and will not alert enemies audibly.

Enemies will approach your last known location each time you raise their suspicion, which will also increase the **detection meter**.

### Enemies
In order to navigate the map, you'll need to avoid enemies. Enemies can **see** and **hear** you if you're too close.

Enemies have three stages of player awareness: `UNAWARE` (visually green), `SUSPICIOUS` (visually yellow) and `SPOTTED` (visually red). Entering an enemy's line of sight on `BRIGHT` tiles will raise suspicion - filling up the detection meter at the bottom - or even get you spotted right away, as will moving across `HARD` tiles too close to an enemy.

Make sure to stay outside of an enemy's sight and hearing range where you can.

Where you have to get close, make use of the environment around you. Sneak past unnoticed by moving through `DARK` or `SOFT` tiles.

Moving across `DARK` tiles mean the enemy can't **see** you; moving across `SOFT` tiles mean the enemy can't **hear** you. One of both ensures complete stealth.

Sneaking up from behind offers you the opportunity to **pickpocket** or **knock out** the enemy, for a short duration. Knocked out enemies will be unable to move or detect the player and are shown in blue.

You will need to pickpocket the **key** to the treasure (`X`) from one of the enemies - but only one, at random, will have the key. Find and unlock the treasure and take it to the exit (`E`) to complete the level.

## Controls
| Key | Action |
| ----------- | ----------- |
| `W/A/S/D` | Movement |
| `E` | Quit |
| `H` | Show help |
| `Space` | Pickpocket enemy / pick up gold piece / unlock treasure |
| `F` | Knock out enemy (from behind) |

## Map Icons
| Symbol | Meaning |
| ----------- | ----------- |
| `P` | Player |
| `X` | Treasure |
| `E` | Exit |
| `Arrows` | Enemies |
| `.` | Soft tile |
| `*` | Hard tile |
| `o` | Gold piece |

---

## UML
### Class Design Diagram
![](https://github.com/tcollyer1/cmd-stealth-game/blob/main/media/Class%20Diagram.png)

### Game Activity Diagram/Flow Chart
![](https://github.com/tcollyer1/cmd-stealth-game/blob/main/media/Flow%20Chart.png)

## Exception Handling
The main points of exception handling included:
- **Files not existing**: each time a `wifstream` object is created, function `GetIfFileExists()` is called which creates a temporary `wifstream` object and checks the `.good()` property.
- **Invalid input**: on the start menu, submitting an invalid menu option will repeatedly ask the user for an input until they submit something valid, using `while` loops.
- **Character movement**: it's vital that the movable `Character` types on the map, `Player` and `Enemy`, do not collide with `Wall` types, which have a property to indicate that nothing can move over this `Entity` type. This prevents enemies or the player from walking off the map beyond the border walls into coordinates not stored on the `GameMap`.
- **Player and enemy collision**: similarly, `Player` and `Enemy` types cannot collide with one another, or another of their type using the above property.
- **Deleting pointers**: this is particularly relevant to when gold is picked up or a game has ended. In the instance of gold, when the player has picked it up, it needs to be removed from the `GameMap`. To do this, its address is acquired and the pointer is freed, *after* removing all references to it in the map's `entities` and `gold` vectors to prevent null pointer references.
- **Spawning**: when entities are randomly spawned on a new game, two entities cannot be spawned in the same location (with the exception of entities spawning on the floor `Tile` objects - these can be passed over freely by any other entity type by design).

## Test Cases - Previous Issues & Workarounds
These test cases showcase the biggest issues with the program during testing and their workarounds.
| Test | Result | Solution |
| ----------- | ----------- | ----------- |
| Player/enemy should not be able to move through walls, or other impassible entities (other enemies/player). Position player at each boundary of the map and verify they cannot move through them. Repeat for in-game wall blocks wherever they spawn. | Player/enemies at the bottom of the map were able to move through walls at the border on the y axis by moving downward. | Walls were drawn to the screen correctly, but their positions were stored differently as drawing the basic game walls/tiles object creation were carried out in different loops with slightly different loop bounds. Players/the enemy can no longer move through walls. Other walls continue to perform as expected. |
| New game should be created successfully after another one ends, with no leftover pointers. End the game and enter 'y' to restart, followed by '1' for a new game. | Null pointer exceptions on termination of a game where memory was incorrectly freed. | There was a for loop that tried to loop through the size of a vector while simultaneously removing items from it, decreasing the upper loop parameter with each iteration and therefore not removing all entities. Fixed by establishing the size before the loop and using this as the upper loop bound. |
| When gold is picked up, this should be removed from the map and not redrawn in the instance the help menu is opened. Pick up a piece of gold, press 'H' to open help, followed by 'Enter' to close, and verify the program does not crash. | Null pointer exception if gold is picked up and help menu is opened and closed | Gold entity was not removed from gold or entities vector after having its memory freed, so a null pointer was trying to be accessed and redrawn. |
| Game should operate normally without crashing if the media folder is not present. Remove the media folder, play through the game normally, verifying no odd behaviour. | Failure to read file caused nothing to be displayed, Windows error sound would play where media files were trying to be played using `PlaySound` | Added a function that checks first if a file exists, and if this returns false, then does not call `PlaySound` and displays an error/alternative display in the instance the banner or help.txt can't be loaded.  |
| When an enemy is alerted and player is in hearing range and on a loud tile, and the player does not move by the time the enemy has prepared their next move, detection should not increase. Verify by entering an enemy's hearing range and stepping on a loud tile, then not moving. Detection should remain the same (unless also in line of sight). | Previously, triggering an enemy's alertness by stepping on a loud tile in their hearing range and not moving by the time the next enemy movement turn came around would treat it as a different move and keep increasing detection | Added a `static Entity::Position playerPos` that checks against the current player position that ensures the player position is not the same as it was upon calling the enemy movement function last |
| Player is on a BRIGHT or MEDIUM tile on a wall opposite the enemy, which is facing the wall. The player should NOT trigger detection via line of sight to the enemy. Wait for an enemy to be close to a wall, and step on a loud tile the opposite side. They should approach and be facing the wall at first. Step on a BRIGHT tile and verify the enemy does not see the player. | Previously, the opposite of this occurred - enemies were able to see through walls, increasing detection/causing instant max detection on a bright tile | Added a function that ensures a wall is not blocking an enemy's line of sight, `IsPlayerBehindWall()`, before checking enemy line of sight |
| Walls of the same orientation should NOT spawn next to one another. Initiate multiple new games to verify same-orientation walls do not spawn directly next to each other. | This did not work reliably, and occasionally same-orientation walls would spawn right next to each other | The game used a `while (i < numWalls)` loop to iterate through individual walls in a wall block to check for parallel wall positions, but did not increment the value of i each iteration, causing it to not check the position of every wall in the wall block. This is now fixed and walls do not generate next to each other. |
| Enemy should correctly set an intermediate position for a simple wall not crossed with another wall and navigates around, when in an alerted state and the player position is the other side of the wall. Recreate this and verify the enemy navigates to the end of the wall, and then approaches the player's last position. | Player stepping on a HARD tile the other side of a single wall with no intersections - enemy struggles to move to the player's position as the wall obstructs moving via one of the axes. | An intermediate position system is now in place that detects whether an obstruction is due to a wall block - and if so, sets an intermediate position at the end of the obstructing wall between the player's last known position and the enemy, rerouting them to this intermediate position first, allowing for better navigation. |
| Enemy should not move if knocked out between preparing a move and actioning a move. Position the player behind an enemy and wait a small period of time before pressing `F` to knock the enemy, and ensure the enemy does not proceed to move when the other enemies move. | Previously, if an enemy is knocked out in the time window between setting up a move and actioning a move, this move is still actioned. | Check the `isActive` property before actioning a move on the relevant game cycle (60th) so that the enemy cancels the planned move. This now functions as intended. |
