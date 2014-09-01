/**
 * @file        game.c
 * @rogram      Lhopi
 * @brief       The game part - Informatic implementation of the rules
 * @description There is all the player stuff, how to the gamer moves on the grid, allowed and forbidden acts etc...
 * @date        8/2014
 * @licence     Beerware (rev 42)
 * @author      Jerome GRARD
 */


#include "conf.h"





/** @brief	Init of the game parameters
  * @param	p_structCommon : Struct with all program informations
  */
void gameInit(structProgramInfo* p_structCommon)
{
	/* Preparation of the graphic part of the game */
	drawTheBoardGame(p_structCommon);


}


/** @brief	The game loop function
  * @param      p_structCommon : Struct with all program informations
  */
void playGame(structProgramInfo* p_structCommon)
{
	gameInit(p_structCommon);
}
