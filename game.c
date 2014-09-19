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
	/* variables init */
	unsigned int l_iIterator;
	unsigned int l_iIterator2;

	l_iIterator = 0;
	l_iIterator2 = 0;

	/* Preparation of the graphic part of the game */
	drawTheBoardGame(p_structCommon);

	/* Init of the grid matrix. That mean we are reserving memory, and fill it with default data */

	/* --> The first number is to select the matrix. Matrix 0 (selected with p_structCommon->cGrid[COLOR_MATRIX][whateverY][whateverX]) stores
	   color for the given position X, Y
	   --> There is not second number yet
	*/
	p_structCommon->cGrid = (char***)malloc(1 * sizeof(char**));

	p_structCommon->cGrid[COLOR_MATRIX] = (char**)malloc(p_structCommon->iSizeY * sizeof(char*));
	for(l_iIterator = 0 ; l_iIterator < p_structCommon->iSizeY ; l_iIterator++)
	{
		p_structCommon->cGrid[COLOR_MATRIX][l_iIterator] = (char*)malloc(p_structCommon->iSizeX * sizeof(char));
		for(l_iIterator2 = 0 ; l_iIterator2 < p_structCommon->iSizeX ; l_iIterator2++)
		{
			p_structCommon->cGrid[COLOR_MATRIX][l_iIterator][l_iIterator2] = enumNoir;
		}
	}
}


/** @brief	The game loop function
  * @param      p_structCommon : Struct with all program informations
  */
void playGame(structProgramInfo* p_structCommon)
{
	unsigned char l_cKey;
	unsigned int l_iCursorX;
	unsigned int l_iCursorY;
	unsigned int l_iOffsetX;
	unsigned int l_iOffsetY;

	l_cKey = 0;
	l_iCursorX = 1;
	l_iCursorY = 1;
	l_iOffsetX = (p_structCommon->iCol / 2) - (p_structCommon->iSizeX / 2);
	l_iOffsetY = (p_structCommon->iRow / 2) - (p_structCommon->iSizeY / 2);

	/* Init the game, screen stuff etc... */
	gameInit(p_structCommon);

	do
	{
		/* Display wursor each time */
		displayCursor(l_iCursorX, l_iCursorY, l_iOffsetX, l_iOffsetY, p_structCommon->cGrid);
		refresh();

		l_cKey = getch();

		switch(l_cKey)
		{

			case 'D':
			{
				/* LEFT */
				l_iCursorX = (l_iCursorX < 1) ? 0 : l_iCursorX - 1;
				break;
			}
			case 'C':
			{
				/* RIGHT */
				l_iCursorX = (l_iCursorX > p_structCommon->iSizeX - 2) ? p_structCommon->iSizeX - 1 : l_iCursorX + 1;
				break;
			}
			case 'A':
			{
				/* UP */
				l_iCursorY = (l_iCursorY < 1) ? 0 : l_iCursorY - 1;
				break;
			}
			case 'B':
			{
				/* DOWN */
				l_iCursorY = (l_iCursorY > p_structCommon->iSizeY - 2) ?
					p_structCommon->iSizeY - 1 : l_iCursorY + 1;
				break;
			}

			default:
			{
				/* Else, do nothing */
				break;
			}
		}
	}while((l_cKey != 'q') && (l_cKey != 'Q'));		/* until q/Q pressed */


}
