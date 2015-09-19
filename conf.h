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

#define _BSD_SOURCE                     /* Add that to have usleep in unistd.h*/
#include <string.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <arpa/inet.h>
#include <pthread.h>

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
#define CONSOLE_SPACE_ON_BOARD_BOTTOM   4   /* Means there is 4 lines at the bottom of the game board reserved to display */
                                            /* informations and, later, the communication box */
#define LOG_DIMENTION                   ((CONSOLE_SPACE_ON_BOARD_BOTTOM - 1))   /* Want three lines in the bottom log display */
#define DEFAULT_PLAYERS                 2   /* Default number of players per play */
#define DEFAULT_GRID_LENGHT             39
#define DEFAULT_GRID_HEIGHT             19
#define COLOR_MATRIX                    0   /* The matrix 'layer'dedicated to store color */
#define TEXT_MATRIX                     1   /* The matrix 'layer' with the character to display */
#define LOOPALGO_MATRIX                 2   /* The matric 'layer' used by the algo to find if a loop is done */
#define USER_COMMAND_LENGHT             256 /* Size max of the command enter by the user during the game */
#define TCP_PORT                        5555
#define MAX_CONNECTED_CLIENTS           16  /* When  program is a server, max clients connected to him */
#define TIME_BETWEEN_TWO_REQUEST        5000
/* Cursor parameters */
#define CURSOR_COLOR                    COLOR_WHITE
#define CURSOR_CHARACTER                219 /* The full matrix */


/* Different kind of point to used by the loop algo in its dedicated layer of the grid */
typedef enum
{
    POINT_EMPTY = 0,
    POINT_START,
    POINT_START_FILLING,
    POINT_TO_EXPLORE,
    POINT_TO_EXPLORE_FILLING,
    POINT_EXPLORED,
    POINT_EXPLORED_FILLING,
    POINT_START_EXPLORED,
    POINT_START_EXPLORED_FILLING,
    POINT_ALL
}g_enumKindOfPoints;

/* Colors of lines (Bottom and Top) */
#define COLOR_LINE_BG_BOTTOM            COLOR_RED
#define COLOR_LINE_BG_TOP               COLOR_BLUE
#define COLOR_LINE_FG_BOTTOM            COLOR_BLACK
#define COLOR_LINE_FG_TOP               COLOR_BLACK


/* Colors of element in a single matrix */
#define COLOR_ELEMENT_FG                COLOR_BLACK
#define GRAPHIC_MODE                    1   /* 1 background is colored 0 background id black and letter is colored */


/* Colors of part of the screen */
#define CONSOLE_LINE_COLOR              COLOR_MAGENTA
#define CONSOLE_LOGTEXT_COLOR           COLOR_YELLOW
#define CONSOLE_BOARDLINE_COLOR         COLOR_RED

/* Directions */
#define DIRECTION_UP                    0
#define DIRECTION_DOWN                  1
#define DIRECTION_LEFT                  2
#define DIRECTION_RIGHT                 3

#define h_addr h_addr_list[0]           /* for backward compatibility */


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
    enumBlanc =     11,
    enumConsole =   12,
    enumLine =      13,
    enumLogLine =   14,
    enumBoardLine = 15
}g_enumJeuxDeCouleursDispo;


typedef enum
{
   DISPLAY = 0,
   CLEAN_L0,
   CLEAN_L1,
   CLEAN_L2,
   CLEAN_BUF,
   ADD_LINE
}g_enumLogBar;


/**
  * @struct structProgramInfo_
  * @var structProgramInfo_::iCol
  * Member 'iCol' contains number of colonne in the screen
  * @var structProgramInfo_::iRow
  * Member 'iRow' contains number of row in the screen
  * @var structProgramInfo_::cGrid
  * Member 'cGrid' contains the grid. For each position in the matrix, grid store all informations (color etc...)
  * @var structProgramInfo_::iSizeX
  * Member 'iSizeX' contains height of the board game
  * @var structProgramInfo_::iSizeY
  * Member 'iSizeY' contains height of the board game
  * @var structProgramInfo_::iCurrentUserNumber
  * Member 'iCurrentUserNumber' contains the number of the local user of the game
  * @var structProgramInfo_::iCurrentUserColor
  * Member 'iCurrentUserNumber' contains the rock color of the current user
  * @var structProgramInfo_::padding
  * Member 'padding' contains only empty spaces in order to guarantee the memory alignement.
  */
typedef struct structProgramInfo_
{
    pthread_mutex_t*    pthreadMutex;
    unsigned int  iCol;
    unsigned int  iRow;
    unsigned int  iSizeX;
    unsigned int  iSizeY;
    unsigned int  iOffsetX;
    unsigned int  iOffsetY;
    unsigned int  iCurrentUserNumber;
    unsigned int  iCurrentUserColor;
    int           iServerSocket;
    int*          iClientsSockets;
    char    bIpV4;
    char    bMutexInitialized;
    char    bNetworkDisconnectionRequiered;
    char*   sUserCommand;
    char*   sServerAddress;
    char*** cGrid;
    unsigned char padding[3];
}__attribute__((aligned(4),packed)) structProgramInfo;






#include "debug.h"
#include "game.h"
#include "drawing.h"
#include "networking.h"
#include "main.h"


#endif
