# CrawlerLike
This is a work-in-progress roguelike dungeon crawler built in C with SDL2.

## Compiling

Download and install the SDL2 library (https://www.libsdl.org). I recommend installing SDL2 with the "Unix Way" Instructions for MacOS. After installing SDL2, clone, compile, and run the program like so:

    git clone https://github.com/ethinethin/CrawlerLike
    cd CrawlerLike
    make
    ./crawler

## In-game Controls

- ESC: Main menu
- Up/W: Move forward one space
- Down/S: Move backward one space
- Left/Q: Turn 90-degrees left
- Right/E: Turn 90-degrees right
- A: move left one square without turning
- D: move right on square without turning
- C: open character sheet
- Space: go up or down stairs

## This is a work in progress

### The features completed so far are:

- Map generation
- Wall rendering
- Movement in the dungeon
- Automapping
- Title screen
- Saving/Loading
- Character creation
- Allocating stat points on character screen
- Money from trashing items
- Time passing
- Item random stat generation
- Treasure boxes with items in dungeon

### The features planned are:

- Better procedural item and skill generation, with icon building
- Consumable items
- Monsters
- Combat
- Merchants
- Usable skill leveling
- Achievements and Unlockables

### Assets TO-DO list:

- All music
- More walls and floors
- More item icons
- Monster sprites
- Merchant sprites
- Achievement icons
