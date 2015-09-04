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

extern FILE* g_FILEOutputLogStream;




/** @brief	Init of the game parameters
  * @param	p_structCommon : Struct with all program informations
  */
void gameInit(structProgramInfo* p_structCommon)
{
	/* variables init */
	unsigned int l_iIterator;
	unsigned int l_iIterator2;
	unsigned int l_iIteratorLayer;
	unsigned char l_iTmp;

	l_iIterator = 0;
	l_iIterator2 = 0;
	l_iIteratorLayer = 0;
	l_iTmp = 0;

	/* Preparation of the graphic part of the game */
	drawTheBoardGame(p_structCommon);

	/* Init of the grid matrix. That mean we are reserving memory, and fill it with default data */

	/* --> The first number is to select the matrix. Matrix 0 (selected with p_structCommon->cGrid[COLOR_MATRIX][whateverY][whateverX]) stores
	   color for the given position X, Y
	   --> The second layer is the text layer, in order to put special character for each user
	*/
	p_structCommon->cGrid = (char***)malloc(2 * sizeof(char**));

	for(l_iIteratorLayer = 0; l_iIteratorLayer < 2 ; l_iIteratorLayer++)
	{
		p_structCommon->cGrid[l_iIteratorLayer] = (char**)malloc(p_structCommon->iSizeY * sizeof(char*));
		for(l_iIterator = 0 ; l_iIterator < p_structCommon->iSizeY ; l_iIterator++)
		{
			p_structCommon->cGrid[l_iIteratorLayer][l_iIterator] = (char*)malloc(p_structCommon->iSizeX * sizeof(char));
			for(l_iIterator2 = 0 ; l_iIterator2 < p_structCommon->iSizeX ; l_iIterator2++)
			{
				switch(l_iIteratorLayer)
				{
					case COLOR_MATRIX:
					l_iTmp = enumNoir;
					break;
					case TEXT_MATRIX:
					l_iTmp = ' ';
					break;
					default:
					l_iTmp = 0;
					break;
				}
				p_structCommon->cGrid[l_iIteratorLayer][l_iIterator][l_iIterator2] = l_iTmp;
			}
		}
	}
}



/** @brief	Function to handle loop formation
            From test (if a loop is created or not) to the filling of it.
  * @param l_iCursorX, X position (position in a text line in the screen) supposed to be the last
                    block needed to make the loop
  * @param l_iCursorY, Y position (the line number). Y axis, vertical axis
  * @param p_structCommon : Struct with all program informations
  */
int loopCompletion(unsigned int p_iCursorX, unsigned int p_iCursorY, structProgramInfo* p_structCommon)
{
    /* Find if there is two neighboor - if not it is dead */
    UNUSED(p_iCursorX);
    UNUSED(p_iCursorY);
    UNUSED(p_structCommon);
    return 0; 
}







/** @brief	The game loop function
  * @param  p_structCommon : Struct with all program informations
  */
void playGame(structProgramInfo* p_structCommon)
{
	unsigned char l_cKey;
	unsigned int l_iCursorX;
	unsigned int l_iCursorY;
	unsigned int l_iOffsetX;
	unsigned int l_iOffsetY;
	unsigned int l_iMovement;	/* store the wanted move, if impossible this variable
					  allow the program to go back */

	l_cKey = 0;
	l_iMovement = 0;
	l_iCursorX = 1;
	l_iCursorY = 1;
	l_iOffsetX = (p_structCommon->iCol / 2) - (p_structCommon->iSizeX / 2);
	l_iOffsetY = (p_structCommon->iRow / 2) - (p_structCommon->iSizeY / 2);


	p_structCommon->iCurrentUserColor = 1; /* FIXME multiplayer mode incoming */

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
				l_iMovement = DIRECTION_LEFT;
				break;
			}
			case 'C':
			{
				/* RIGHT */
				l_iCursorX = (l_iCursorX > p_structCommon->iSizeX - 2) ? p_structCommon->iSizeX - 1 : l_iCursorX + 1;
				l_iMovement = DIRECTION_RIGHT;
				break;
			}
			case 'A':
			{
				/* UP */
				l_iCursorY = (l_iCursorY < 1) ? 0 : l_iCursorY - 1;
				l_iMovement = DIRECTION_UP;
				break;
			}
			case 'B':
			{
				/* DOWN */
				l_iCursorY = (l_iCursorY > p_structCommon->iSizeY - 2) ?
					p_structCommon->iSizeY - 1 : l_iCursorY + 1;
				l_iMovement = DIRECTION_DOWN;
				break;
			}
			case ' ':
			{
				/* When the user drop a rock */

				/* Put the color information in the matrix */
				p_structCommon->cGrid[COLOR_MATRIX][l_iCursorY][l_iCursorX] =
					p_structCommon->iCurrentUserColor;

				/* Put the text information in the matrix */
				p_structCommon->cGrid[TEXT_MATRIX][l_iCursorY][l_iCursorX] =
					' ';

				/* Draw the block of the current user (the other blocks are draw by
				   another function) */
				drawElement(l_iCursorX + l_iOffsetX, l_iCursorY + l_iOffsetY,
					p_structCommon->cGrid[TEXT_MATRIX][l_iCursorY][l_iCursorX],
					p_structCommon->iCurrentUserColor);

                /* Check neighborhood - If there is two contigous blocks of the player's
                   color that means there is maybee a loop */
                loopCompletion(l_iCursorX, l_iCursorY, p_structCommon);
			}

			default:
			{
				/* Else, do nothing */
				break;
			}
		}

		/* Check code, in order to forbid access to already reserved boxes */
		if((unsigned int)p_structCommon->cGrid[COLOR_MATRIX][l_iCursorY][l_iCursorX] != enumNoir)
		{
			/* So there is something here and it is not me */
			switch(l_iMovement)
			{
				case DIRECTION_UP:
				l_iCursorY++;
				break;
				case DIRECTION_DOWN:
				l_iCursorY--;
				break;
				case DIRECTION_LEFT:
				l_iCursorX++;
				break;
				case DIRECTION_RIGHT:
				l_iCursorX--;
				break;
				default:
				/* Who, error ! */
				perror("Unknown code");
				break;
			}

			/* If we go out of the screen by the upper or wester side put it in 0:0 and
			let the next block check if this block is empty or not */
			if(l_iCursorY >= p_structCommon->iSizeY || l_iCursorX >= p_structCommon->iSizeX)
			{
				l_iCursorX = 0;
				l_iCursorY = 0;
			}

			/* If with bad luck you go over the grid */
			while((unsigned int)p_structCommon->cGrid[COLOR_MATRIX][l_iCursorY][l_iCursorX] != enumNoir)
			{
				/* Find random coordinate with the simplest way */
				l_iCursorX = rand() % p_structCommon->iSizeX;
				l_iCursorY = rand() % p_structCommon->iSizeY;
			}
		}


	}while((l_cKey != 'q') && (l_cKey != 'Q'));		/* until q/Q pressed */


}
