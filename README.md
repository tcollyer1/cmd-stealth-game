# CMD Stealth
## Overview
CMD Stealth is a command line game written in C++, using Visual Studio 2019 (& 2022) and Windows 10 (64-bit). It's designed to be a top-down stealth game where you, the player, must navigate around the map and attempt to pickpocket enemies in search of the key to the treasure. Once the treasure has been opened and obtained, the player may seek the exit and leave the level. The player must navigate their way around the map and around enemies without alerting them too much - should the game's detection meter fill up, it's game over for the player.

Gold can optionally be collected along the way - and each game is timed to encourage the player to beat the level as quickly as possible. These two things in conjunction will increase the player's score, which they can attempt to beat each time.

[YouTube walkthrough]() | [Repo](https://github.com/tcollyer1/cmd-stealth-game)

---

## Gameplay
### Tiles
The walkable tiles on the map have a **light level** and **terrain type** property. Each tile will either be `BRIGHT`, `MEDIUM` or `DARK`, and will also be either `HARD` (`*`) or `SOFT` (`.`).

`BRIGHT` tiles are light grey in colour, where `MEDIUM` and `DARK` tiles are black. `DARK` tiles are completely black - so you won't be able to tell whether it's `HARD` or `SOFT`, to account for some realism.

`BRIGHT` tiles mean the enemy will be able to see you, should you be in their line of sight; `MEDIUM` means it will be more difficult for an enemy to spot you (you must be closer to them to raise suspicion), and `DARK` renders you completely invisible. Should an enemy bump into you, however - you'll be immediately spotted.

`HARD` (`*`) tiles will emit noise when walked on, alerting the enemies audibly when within their hearing range. `SOFT` (`.`) tiles are impossible to hear when walked on and will not alert enemies audibly.

### Enemies
In order to navigate the map, you'll need to avoid enemies. Enemies can **see** and **hear** you if you're too close.

Enemies have three stages of player awareness: `UNAWARE` (visually green), `SUSPICIOUS` (visually yellow) and `SPOTTED` (visually red). Entering an enemy's line of sight on `BRIGHT` tiles will raise suspicion - filling up the detection meter at the bottom - or even get you spotted right away, as will moving across `HARD` tiles too close to an enemy.

Make sure to stay outside of an enemy's sight and hearing range where you can.

Where you have to get close, make use of the environment around you. Sneak past unnoticed by moving through `DARK` or `SOFT` tiles.

Moving across `DARK` tiles mean the enemy can't **see** you; moving across `SOFT` tiles mean the enemy can't **hear** you. One of both ensures complete stealth.

Sneaking up from behind offers you the opportunity to **pickpocket** or **knock out** the enemy. Knocked out enemies will be unable to move or detect the player and are shown in blue.

You will need to pickpocket the **key** to the treasure (`X`) from one of the enemies - but only one, at random, will have the key. Unlock the treasure and take it to the exit (`E`) to complete the level.

## Controls
| Key | Action |
| ----------- | ----------- |
| `W/A/S/D` | Movement |
| `E` | Quit |
| `H` | Show help |
| `Space` | Pickpocket enemy / pick up gold piece |
| `F` | Knock out enemy (from behind) / unlock treasure |

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
