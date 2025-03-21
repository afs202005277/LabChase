# Lab-Chase - The TRON Light Cycle Clone

## Description
Lab-Chase is a top-down, two-player game inspired by the iconic TRON Light Cycle concept. The objective of the game is to survive longer than the opposing player by avoiding collisions with other entities such as the enemy player, the edges of the map, and the trails left behind by each player. Each player’s trail is equipped with a boost feature, allowing for brief periods of increased speed. The game supports both local and multi-computer (serial port) modes, featuring a day/night mode based on the system's real-time clock, all using low-level C programming.

## Features
- **Two-player Gameplay:** Compete against another player in local or multi-computer mode.
- **Boost Mechanic:** Temporary speed boost available to both players.
- **Day/Night Mode:** Background changes based on the system's time (Day Mode: 07:00 - 19:00, Night Mode: 19:00 - 07:00).
- **Pause Functionality:** Pause the game during local mode using the 'P' key.
- **Custom Cursor:** Specially designed cursor for game menu navigation.
- **Collision Detection:** Detection of collisions with trails, map edges, and other players.
- **Serial Port Communication:** Allows two computers to connect and play the game simultaneously using UART communication.

## How to Play
- **Player One (Blue):**
  - Movement: `WASD`
  - Boost: `X`
- **Player Two (Orange):**
  - Movement: `Arrow Keys`
  - Boost: `Right Shift`
- **Common Controls:**
  - `P`: Pauses/Resumes the game (Local Mode only).
  - `Esc`: Returns to the main menu during gameplay.

## Main Menu
The main menu offers three options:
- **Local Mode:** Starts a game session where both players use the same keyboard.
- **Serial Mode:** Waits for a connection with another computer through the serial port to start the game.
- **Quit:** Exits the game.

## Project Structure
The project consists of several modules handling different aspects of the game:
- `keyboard`: Manages keyboard inputs and interrupts.
- `main`: The core module managing game states, menus, and IO device handling.
- `mouse`: Handles mouse input for menu navigation.
- `RTC`: Reads system time to determine day or night mode.
- `serial port`: Manages UART communication for multi-computer mode.
- `timer`: Handles timer interrupts for player movement.
- `utils`: General-purpose utility functions.
- `video_gr`: Manages graphical rendering and collision detection.
- `auxiliary_data_structures`: Defines various data structures used throughout the project.

## Dependencies
The project relies on functionalities provided by standard IO devices:
- **Timer (60Hz):** Handles game framerate and continuous player movement.
- **Keyboard & Mouse:** For gameplay control and menu navigation.
- **Video Card (Mode 0x115):** Renders menus, game graphics, and custom cursor.
- **RTC:** Determines whether the game runs in day or night mode.
- **Serial Port:** Enables multi-computer gameplay.

## Future Improvements
- Implementing a scoreboard to track the longest survival times.
