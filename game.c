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

   logBar(p_structCommon, ADD_LINE, "Party starting...");
   logBar(p_structCommon, DISPLAY, "");

	/* Init of the grid matrix. That mean we are reserving memory, and fill it with default data */

	/* --> The first number is to select the matrix. Matrix 0 (selected with p_structCommon->cGrid[COLOR_MATRIX][whateverY][whateverX]) stores
	   color for the given position X, Y. The Matrix 1 is for the text to display on the screen. The Third is for the loop algorithme. See loop
       function.
	   --> The second layer is the text layer, in order to put special character for each user
	*/
	p_structCommon->cGrid = (char***)malloc(3 * sizeof(char**));

	for(l_iIteratorLayer = 0; l_iIteratorLayer < 3 ; l_iIteratorLayer++)
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
					case LOOPALGO_MATRIX:
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

/** @brief	To clean a layer of the grid
  * @param p_iLayer : layer to clean
  * @param p_cFillingValue : value to put in the grid
  * @param p_structCommon : Struct with all program informations
  */
void cleanGridLayer(unsigned int p_iLayer, unsigned char p_cFillingValue, structProgramInfo* p_structCommon)
{
    unsigned int l_iX;
    unsigned int l_iY;


    for(l_iY = 0; l_iY < p_structCommon->iSizeY ; l_iY++)
    {
        for(l_iX = 0; l_iX < p_structCommon->iSizeX ; l_iX++)
        {
           p_structCommon->cGrid[p_iLayer][l_iY][l_iX] =  p_cFillingValue;
        }
    }
}







/** @brief	Recursive function to browse blocs from a starting point to the end. If the starting point is found again
  *         the recursive function will end and we know we have a loop. It takes at least 8 blocks to have a complete
  *         loop with an empty block in it.
  * @param p_iX : X position of the point to analyse
  * @param p_iY : Y position of the point to analyse
  * @param p_structCommon : Struct with all program informations
  * @return 0 if there is nothing to see in this point. >0 number equals to the number of Hops needed to reach the starting
  *           point with this path. This number have to be returned to the caller, and so on until the first calling function
  *           in order to let it know the number of hops needed to complete the loop.
  */
int recursiveDiscovery(unsigned int p_iHop, unsigned int p_iY, unsigned int p_iX, structProgramInfo* p_structCommon)
{
    unsigned int l_iReturned;

    l_iReturned = 0;
    /* Point is'nt in the game grid */
    if(p_iY > p_structCommon->iSizeY ||
       p_iX > p_structCommon->iSizeX)
    {
        return 0;
    }

    /* Point doen't belong to the right user */
    if(p_structCommon->cGrid[COLOR_MATRIX][p_iY][p_iX] != (signed)p_structCommon->iCurrentUserColor)
    {
        return 0;
    }

    /* We alreadu know this point */
    if(p_structCommon->cGrid[LOOPALGO_MATRIX][p_iY][p_iX] == POINT_EXPLORED)
    {
        return 0;
    }

    /* There is a new point. Mark visited before doing anything */
    if(p_structCommon->cGrid[LOOPALGO_MATRIX][p_iY][p_iX] != POINT_START)
    {
        p_structCommon->cGrid[LOOPALGO_MATRIX][p_iY][p_iX] = POINT_EXPLORED;
    }

    /* We have reached the starting point */
    if(p_structCommon->cGrid[LOOPALGO_MATRIX][p_iY][p_iX] == POINT_START && p_iHop > 1)
    {
        /* In this case, hop > 1 thus there is the result of a loop browsing */
        return p_iHop;
    }
    else if(p_structCommon->cGrid[LOOPALGO_MATRIX][p_iY][p_iX] == POINT_START && p_iHop == 0)
    {
        /* do nothing, we are on the STARTING_POINT */
    }
    else if(p_structCommon->cGrid[LOOPALGO_MATRIX][p_iY][p_iX] == POINT_START && p_iHop == 1)
    {
        /* In this case it is because we have jump on it from one of the four points next to the start point */
        return 0;
    }

    p_iHop++;

    /* recusive on the four other position next to this one */
    /* If one of them disvover the starting point it will return a number bigger than 6, so kill the discovery machine here */
    l_iReturned = recursiveDiscovery(p_iHop, p_iY - 1, p_iX, p_structCommon);
    if(l_iReturned > 6) return l_iReturned;
    l_iReturned = recursiveDiscovery(p_iHop, p_iY + 1, p_iX, p_structCommon);
    if(l_iReturned > 6) return l_iReturned;
    l_iReturned = recursiveDiscovery(p_iHop, p_iY, p_iX - 1, p_structCommon);
    if(l_iReturned > 6) return l_iReturned;
    l_iReturned = recursiveDiscovery(p_iHop, p_iY, p_iX + 1, p_structCommon);
    if(l_iReturned > 6) return l_iReturned;

    /* Nothing was discovered, return 0 */
    return 0;
}



/** @brief	 Function to handle loop formation
  *          From test (if a loop is created or not) to the filling of it.
  * @param p_iCursorX : X position (position in a text line in the screen) supposed to be the last
  *          block needed to make the loop
  * @param p_iCursorY : Y position (the line number). Y axis, vertical axis
  * @param p_structCommon : Struct with all program informations
  * @return EXIT_FAILURE if there is no loop completed. EXIT_SUCCESS in case of loop with at least one block filled
  */
int loopCompletion(unsigned int p_iCursorX, unsigned int p_iCursorY, structProgramInfo* p_structCommon)
{
    /* Clean the -computation- grid */
    cleanGridLayer(LOOPALGO_MATRIX, POINT_EMPTY, p_structCommon);

    /* Set the starting point of the forsaken loop */
    p_structCommon->cGrid[LOOPALGO_MATRIX][p_iCursorY][p_iCursorX] = POINT_START;

    if(recursiveDiscovery(0, p_iCursorY, p_iCursorX, p_structCommon) > 6)
    {
        /* found */
        logBar(p_structCommon, ADD_LINE, "Loop found");
        logBar(p_structCommon, DISPLAY, "");
        
        return EXIT_SUCCESS;
    }

    logBar(p_structCommon, ADD_LINE, "NO Loop found");
    logBar(p_structCommon, DISPLAY, "");
    

    return EXIT_FAILURE; 
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
