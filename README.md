# CMD Stealth
## Overview
CMD Stealth is a command line game written in C++, using Visual Studio 2019 (& 2022) and Windows 10 (64-bit). It's designed to be a top-down stealth game where you, the player, must navigate around the map and attempt to pickpocket enemies in search of the key to the treasure. The treasure will be hidden to you at first, and marked as soon as you've found it. Once the treasure has been located, opened and obtained with the key, the player may seek the exit and leave the level. The player must navigate their way around the map and around enemies without alerting them too much - should the game's detection meter fill up, it's game over for the player.

Gold can optionally be collected along the way - and each game is timed to encourage the player to beat the level as quickly as possible. These two things in conjunction will increase the player's score, which they can attempt to beat each time.

![](https://github.com/tcollyer1/cmd-stealth-game/blob/dev/media/game.png)
![](https://github.com/tcollyer1/cmd-stealth-game/blob/dev/media/startup.png)

[YouTube walkthrough]() | [Repo](https://github.com/tcollyer1/cmd-stealth-game)

## Resources
- [ASCII logo generator](https://patorjk.com/software/taag/)
- [Sound effects](https://www.zapsplat.com) (ZapSplat)

---

## Gameplay
### Key Mechanics
- enemy detection system - acquiring the player's last known location and moving to it, eventually resetting detection for that enemy when their awareness timer expires
- ability to lure enemies into other locations using the above mechanic, which can aid with gold collection or looking for the treasure
- marking the treasure for the player once they have actually found it 
- full detection means game over
- tile loudness/brightness system
- wall & entity-to-entity collision
- walls blocking enemy line of sight
- enemy KOs
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

Sneaking up from behind offers you the opportunity to **pickpocket** or **knock out** the enemy. Knocked out enemies will be unable to move or detect the player and are shown in blue.

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
*Flow chart/class diagram here*

## Exception Handling
The main points of exception handling included:
- **Files not existing**: each time a `wifstream` object is created, function `GetIfFileExists()` is called which creates a temporary `wifstream` object and checks the `.good()` property.
- **Invalid input**: on the start menu, submitting an invalid menu option will repeatedly ask the user for an input until they submit something valid, using `while` loops.
- **Character movement**: it's vital that the movable `Character` types on the map, `Player` and `Enemy`, do not collide with `Wall` types, which have a property to indicate that nothing can move over this `Entity` type. This prevents enemies or the player from walking off the map beyond the border walls into coordinates not stored on the `GameMap`.
- **Player and enemy collision**: similarly, `Player` and `Enemy` types cannot collide with one another, or another of their type using the above property.
- **Deleting pointers**: this is particularly relevant to when gold is picked up or a game has ended. In the instance of gold, when the player has picked it up, it needs to be removed from the `GameMap`. To do this, its address is acquired and the pointer is freed, *after* removing all references to it in the map's `entities` and `gold` vectors to prevent null pointer references.
- **Spawning**: when entities are randomly spawned on a new game, two entities cannot be spawned in the same location (with the exception of entities spawning on the floor `Tile` objects - these can be passed over freely by any other entity type by design).
