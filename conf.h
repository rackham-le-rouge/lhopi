/*******************************************************

		Program configuration file
		After modification you have
		to recompile the program.

*******************************************************/


/**
 * @file        conf.h
 * @rogram      Lhopi
 * @brief       Configuration values
 * @description There is all configurables values of the program
 * @date        8/2014
 * @licence     Beerware (rev 42)
 * @author      Jerome GRARD
 */


#include <string.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>


/*   Avaiable colors    *

        COLOR_BLACK   0
        COLOR_RED     1
        COLOR_GREEN   2
        COLOR_YELLOW  3
        COLOR_BLUE    4
        COLOR_MAGENTA 5
        COLOR_CYAN    6
        COLOR_WHITE   7
**                      */




#ifndef CONFIG_H
#define CONFIG_H

/* Parameters of the program */
#define CONSOLE_SPACE_ON_BOARD_BOTTOM	3	/* Means there is 4 lines at the bottom of the game board reserved to display */
						/* informations and, later, the communication box */



/* Colors of lines (Bottom and Top) */
#define COLOR_LINE_BG_BOTTOM    COLOR_RED
#define COLOR_LINE_BG_TOP       COLOR_BLUE
#define COLOR_LINE_FG_BOTTOM    COLOR_BLACK
#define COLOR_LINE_FG_TOP       COLOR_BLACK


/* Colors of element in a single matrix */
#define COLOR_ELEMENT_FG        COLOR_BLACK
/*#define GRAPHIC_MODE            1 */              /* 1 background is colored 0 background id black and letter is colored */


/* Colors of part of the screen */
#define CONSOLE_LINE_COLOR	enumMagenta



typedef enum
{
        enumNoir =      3,
        enumRouge =     4,
        enumVert =      5,
        enumJaune =     6,
        /* 7 is reserved don't use it ! */
        enumBleu =      8,
        enumMagenta =   9,
        enumCyan =      10,
        enumBlanc =     11
}g_enumJeuxDeCouleursDispo;



/**
  * @struct structProgramInfo_
  * @var structProgramInfo_::iCol
  * Member 'iCol' contains number of colonne in the screen
  * @var structProgramInfo_::iRow
  * Member 'iRow' contains number of row in the screen
  * @var structProgramInfo_::padding
  * Member 'padding' contains only empty spaces in order to guarantee the memory alignement.
  */
typedef struct structProgramInfo_
{
        int  iCol;
        int  iRow;
	int  iSizeX;
	int  iSizeY;
	unsigned char padding[8];
}__attribute__((aligned(4),packed)) structProgramInfo;





#include "game.h"
#include "drawing.h"
#include "main.h"


#endif
