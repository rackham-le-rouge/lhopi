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
	unsigned char l_cKey;
	int l_iCursorX;
	int l_iCursorY;

	l_cKey = 0;
	l_iCursorX = 1;
	l_iCursorY = 1;

	/* Init the game, screen stuff etc... */
	gameInit(p_structCommon);

	/* Display cursor */
	displayCursor(l_iCursorX, l_iCursorY);

	do
	{
		l_cKey = getch();



	}while(l_cKey != 27);		/* until Esc pressed */
}
