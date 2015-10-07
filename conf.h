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
#define SYNC_MATRIX                     3   /* The matrix 'layer' used by the synchronisation algo in server part during online game to know block to synch with clients */
#define USER_COMMAND_LENGHT             256 /* Size max of the command enter by the user during the game */
#define TCP_PORT                        5555
#define MAX_CONNECTED_CLIENTS           6  /* When  program is a server, max clients connected to him | 8 colors available, but there is black and the current user color */
#define TIME_BETWEEN_TWO_REQUEST        20000   /* FIXME why can i go over somthing like 20000 without lost the capability of sending messages */
#define PARAMETER_MAX_LENGHT            40  /* Max parameter size for a user command during execution. Limited by ipv6 max lenght */
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
    POINT_EXPLORED_NOT_FILLING,
    POINT_START_EXPLORED,
    POINT_START_EXPLORED_FILLING,
    POINT_TO_SYNC,
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
    enumBoardLine = 15,
    enumLetterRed    = 24,
    enumLetterGreen  = 25,
    enumLetterYellow = 26,
    enumLetterBlue   = 28,
    enumLetterPurple = 29,
    enumLetterCyan   = 30,
    enumLetterWhite  = 31
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
  * Member 'iOffsetX' Beginning of the grid on the axis X
  * @var structProgramInfo_::iOffsetX
  * Member 'iOffsetY' Beginning of the grid on the axis Y
  * @var structProgramInfo_::iOffsetY
  * Member 'iServerSocket' For a client, socket value of the server. Returned by open() on the server address
  * @var structProgramInfo_::iServerSocket
  * Member 'iClientsSockets' For a server, array of the soket value (returned by accept()) of all the remote clients
  * @var structProgramInfo_::iClientsSockets
  * Member 'bIpV4' Boolean. For a client, 1 means server address is provided in IPV4, 0 means IPV6.
  * @var structProgramInfo_::bIpV4
  * Member 'bMutexInitialized' For client and server, means that multithread is started and mutex was initialized. Thus, we have to release it at the end of the execution
  * @var structProgramInfo_::bMutexInitialized
  * Member 'bNetworkDisconnectionRequiered'  For client and server, means that the user have asked to terminate the network connection. Threads receive this message, close the active connection, close the thread. For a client, after that, put FALSE in this value. For a server, because there is many threads, we can't do that. So the caller have to pool the remote client socket list, and wait for all threads put 0 in their cases.
  * @var structProgramInfo_::bNetworkDisconnectionRequiered
  * Member 'sUserCommand' Store the command entred by user in order to share it with sending / receiving thread
  * @var structProgramInfo_::sUserCommand
  * Member 'sServerAddress' For a client, this is the parameter provided by user as an address for the remote server. IPV4 / IPV6 is automatically decided by the lenght of the address
  * @var structProgramInfo_::sServerAddress
  * Member 'iRow' contains number of row in the screen
  * @var structProgramInfo_::cGrid
  * Member 'cGrid' contains the grid. For each position in the matrix, grid store all informations (color etc...)
  * @var structProgramInfo_::iSizeX
  * Member 'iSizeX' contains height of the board game
  * @var structProgramInfo_::bAbleToRestartGame
  * Member 'bAbleToRestartGame' this flag is set to FALSE until cleaning function have finish and put TRUE. Thus the game restart
  * @var structProgramInfo_::iSizeY
  * Member 'iSizeY' contains height of the board game
  * @var structProgramInfo_::iCurrentUserNumber
  * Member 'iCurrentUserNumber' contains the number of the local user of the game
  * @var structProgramInfo_::iCurrentUserColor
  * Member 'iCurrentUserColor' contains the rock color of the current user
  * @var structProgramInfo_::cUserMove
  * Member 'cUserMove' Last user move, used also as a flag. When this value goes back to 0 means that the user move have been transmitted to the server
  * @var structProgramInfo_::iClientsColor
  * Member 'iClientsColor' Table containing all the clients color when you are on the server side of the app
  * @var structProgramInfo_::sUserName
  * Member 'sUserName' Name of the user, by default we put some funny stuff in it. User can redefine it. Usefull to know who sent a message on the network discussion
  * @var structProgramInfo_::bMyTurnToPlay
  * Member 'bMyTurnToPlay' In single user mode, it is already true. By when we are in online mode, it more complicated.
  * @var structProgramInfo_::bWhoHaveToPlay
  * Member 'bWhoHaveToPlay' Server's table to gives turns to each player. 2 means it is the current player, 1 is an active player awaiting for his turn, and 0 an uninitialized player
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
    unsigned int  iLastXUsed;
    unsigned int  iLastYUsed;
    int           iServerSocket;
    int*          iClientsSockets;
    unsigned int* iClientsColor;
    char    bMyTurnToPlay;
    char    bIpV4;
    char    bMutexInitialized;
    char    bNetworkDisconnectionRequiered;
    char    bAbleToRestartGame;
    char    cUserMove;
    char*   bWhoHaveToPlay;
    char*   sUserCommand;
    char*   sServerAddress;
    char*   sUserName;
    char*** cGrid;
    unsigned char padding[2];
}__attribute__((aligned(4),packed)) structProgramInfo;






#include "debug.h"
#include "game.h"
#include "drawing.h"
#include "networking.h"
#include "main.h"


#endif
