# Fnake
__Fnake - Snake with Friends__

By Reo Kimura  - Period 10

and Karol Regula - Period 5

## Introduction

### Purpose
  Fnake is a lobby-integrated snake game that is played by several players at the same time over a network. Players may create their own accounts, create publicly visible rooms and join a room with their friends and play together.

### Instructions for Running
  To run Fnake, simply `make` using the makefile. Then use your terminal to run the server (`./server`) and client (`./client`) files.

### Tutorial - Instructions for Usage
  + After you have run the game, you will be prompted to create an account.
  + To register, type in a username and your new password twice. To login, simply type in your username and Fnake will recognize that you are an existing user and will prompt you for your password.
  + **Note: The `!help` command is your friend, it will show you a list of available commands and their purpose at any time.**
  + While in the lobby, you can view a list of existing game rooms using `!refresh`.
  + Use the `!join <roomName>` command to join a room.
  + You can create your own room using the `!create <roomName>` command, you will be automatically placed in the room.
  + The `!refresh` command refreshes the list of available rooms.
  + Use the `!exit` command to close Fnake.

#### Upon Entering a Game Room
  +  you join a room, use the `!refresh` command to view the players in the room and see how many players have indicated that they are ready to start the game.
  + When you are ready, use the `!ready` command to indicate so.
  + `!leave` will cause you to leave the room.
  + The game will automatically start once all players indicate that they are ready.

#### In Game
  + Use your WASD keys to move the snake in the correct direction once it is your turn to make a move.

## Bugs and Concerns
  + A not-reproducible segmentation fault rarely appears.
  + All networking and game code is in place, we have a bug that we have not been able to resolve with the functioning of the game itself. The communication between the server and the client seems to get caught somewhere and never reach the destination function.
  + Duplicate room names are allowed, causes problems.
