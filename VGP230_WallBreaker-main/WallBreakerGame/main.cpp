#include "raylib.h"
#include "WallBreaker.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

//This is our first serious attempt at creating a game using Raylib + C++.
//
//To do that, we will take the game created in class on Session 4 (Wall Breaker) and expand it with the following :
//
//1 - Include deltaTime in all speed, movement calculations to make sure
//consistent speed is applied across devices-----------------------------------------------------------Done
// the speed is constant if I change the fps
//
//
//2 - Develop a CircleToRoundRectangleCollision to detect the collision that happens
//with the corners of the bricks-----------------------------------------------------------------------DONE
//
//
//3 - Depending on where the collision happens with the bricks, come up with a direction
//that will affect the x and y of the ball's speed-----------------------------------------------------DONE
//
//
//4 - Include an unlimited number of levels that are created procedurally.
//Each time you spawn the game, a new layout should be displayed_______________________________________DONE
//
//
//5 - Have pickups such as life that fall from certain bricks upon impact.
//Collecting them by the player, will activate the reward._____________________________________________DONE
//
//
//6 - Include a cheat.Such as a pickup that temporarily(for instance for the next 5 seconds)
//[with a time feedback for the user] eliminates the reversal of speed upon impact with bricks.________DONE
// I made a super ball cheat, player get the item ball, and will be unstopable for 5s.
//
//
//7 - Bonus->Include sound effects---------------------------------------------------------------------I add a bgm, but if I want more sound files, 
//I needs to put them into a vector. I find the way to do it:https://github.com/JeffM2501/raylib/blob/b9a10b5db758eae8110357c8ad7b243f95ca1aaf/examples/audio/audio_sound_multi.c
//maybe gave me bonus 1 for this..?

//8 - Bonus->Find a way to put the game on your phone
//go it on the web:https://github.com/raysan5/raylib/wiki/Working-for-Web-%28HTML5%29

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

int main()
{
	WallBreaker game;
	game.Main();

	return 0;
}