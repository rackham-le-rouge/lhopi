/**
 * @file        main.c
 * @rogram      Lhopi
 * @brief       Main part of the program
 * @description There is the main function (start the screen and display the splash) and after run the program
 * @date        8/2014
 * @licence     Beerware (rev 42)
 * @author      Jerome GRARD
 */


/**
 *   ERRATUM
 *
 *i.e : l_cCurrentCharacter   == l is for local (g : global and p : parameter) // c is for char (i : integer, f : float etc...)
 */


#include "conf.h"



/** Revision and version number */
char rev[] = "0.0";
/** Revision and version number */
char ver[] = "1.0";

FILE* g_FILEOutputLogStream;



/** @brief  Last function executed by the program, used to release the screen */
void killTheApp(void)
{
        /* Show the cursor when the program going down */
        curs_set(1);

        /*  Stop the program and leave the graphic mode ! Very important ! */
        endwin();
}



/**
  * @brief Extract all usefull function from command line, and configure the program to
  *        apply wanted parameters
  * @param *p_structCommon : this is the common structure of the program, to carry all important datas
  * @param argc : number of aguments received by the main
  * @param argv : 2D array to store all parameters gived to the main
  * @return If there i a non NULL value, that means you have to end the program now
  */
int extractConfigFromCommandLine(int argc, char** argv, structProgramInfo* p_structCommon)
{
        int l_iTmp;

        if(argc > 1)
        {
                for(l_iTmp = 1; l_iTmp < argc ; l_iTmp++)
                {
                        if(strlen(argv[l_iTmp]) > 1)
                        {
                                /* Change size of the grid */
                                p_structCommon->iSizeX = (!strcmp(argv[l_iTmp], "-s")) ? (unsigned int)atoi(argv[l_iTmp + 1]) : p_structCommon->iSizeX;
                                p_structCommon->iSizeY = (!strcmp(argv[l_iTmp], "-s")) ? (unsigned int)atoi(argv[l_iTmp + 2]) : p_structCommon->iSizeY;
                                /*if(!strcmp(argv[l_iTmp], "-s")) {LOG_WRITE_STRING_LONG("C.LINE: ", (long)p_structCommon->iSizeX)}*/
                                p_structCommon->iTcpPort = (!strcmp(argv[l_iTmp], "-p")) ? (unsigned int)atoi(argv[l_iTmp + 1]) : p_structCommon->iTcpPort;
                                if(!strcmp(argv[l_iTmp], "-n"))
                                {
                                    strncpy(p_structCommon->sUserName, argv[l_iTmp + 1], PARAMETER_MAX_LENGHT);
                                }

                                /* Display help */
                                if(!strcmp(argv[l_iTmp], "-h"))
                                {
                                        endwin();
                                        printf("Lhopi - Command line use : lhopi [-h{help}] [-s Width Height{new dimensions}] [-n nick] [p port]\n");
                                        /* Quit function now, then kill the app in order to display help message */
                                        return 1;
                                }
                        }
                        /* Else the parameter is ignored, use strcmp on it cause a segfault */
                }
        }
        return 0;
}

/**
  * @brief Count points of each users -dropped rock on the grid-
  * @param p_cGrid : the user board
  * @param p_iPoints : the table to store points of each user. Size is of MAX_CONNECTED_CLIENTS
  * @param p_iSizeX : size of the grid
  * @param p_iSizeY : size of the grid
  */
void pointCounting(char*** p_cGrid, int* p_iPoints, unsigned int p_iSizeX, unsigned int p_iSizeY)
{
    unsigned int l_iX;
    unsigned int l_iY;

    memset(p_iPoints, 0, MAX_CONNECTED_CLIENTS);

    for(l_iY = 0; l_iY < p_iSizeY; l_iY++)
    {
        for(l_iX = 0; l_iX < p_iSizeX; l_iX++)
        {
            if(p_cGrid[COLOR_MATRIX][l_iY][l_iX] - enumRouge >= 0)
            {
                p_iPoints[p_cGrid[COLOR_MATRIX][l_iY][l_iX] - enumRouge]++;
            }
        }
    }
}



void logBar(structProgramInfo* p_structCommon, g_enumLogBar p_enumBarWantedAction, const char* p_sNewLine)
{
    int l_iIterator;
    char* l_cBuffer;
    static char**  l_sLogStrings = NULL;

    if(l_sLogStrings == NULL)
    {
      /* Means that it's the first time - do malloc */
      l_sLogStrings = (char**)malloc(LOG_DIMENTION*sizeof(char*));
      if(l_sLogStrings == NULL)
      {
         log_err("No memory available. Must close now.%s", "\n");
         exit(ENOMEM);
      }

      for( l_iIterator = 0; l_iIterator < LOG_DIMENTION; l_iIterator++)
      {
         l_sLogStrings[l_iIterator] = (char*)malloc((p_structCommon->iCol + 1)*sizeof(char));

         if(l_sLogStrings[l_iIterator] == NULL)
         {
             log_err("No memory available. Can't create %d string. Must close now.\n", l_iIterator);
             exit(ENOMEM);
         }
      }
    }

    switch(p_enumBarWantedAction)
    {
      case DISPLAY:
            for(l_iIterator = LOG_DIMENTION - 1; l_iIterator >= 0; l_iIterator--)
            {
                drawLogLine(p_structCommon, l_iIterator, l_sLogStrings[l_iIterator]);
            }
            break;
      case CLEAN_BUF:
            for(l_iIterator = 0; l_iIterator < LOG_DIMENTION; l_iIterator++)
            {
                memset(l_sLogStrings[l_iIterator], ' ', p_structCommon->iCol);
                l_sLogStrings[l_iIterator][p_structCommon->iCol - 1] = '\0';
            }
            break;
      case ADD_LINE:
            l_cBuffer = (char*)malloc((p_structCommon->iCol + 1)*sizeof(char));
            if(l_cBuffer == NULL)
            {
                exit(ENOMEM);
            }

            memset(l_cBuffer, ' ', p_structCommon->iCol + 1);
            strcpy(l_cBuffer, p_sNewLine);
            l_cBuffer[strlen(p_sNewLine)] = ' ';
            l_cBuffer[p_structCommon->iCol] = '\0';

            strcpy(l_sLogStrings[2], l_sLogStrings[1]);
            strcpy(l_sLogStrings[1], l_sLogStrings[0]);
            strcpy(l_sLogStrings[0], l_cBuffer);
            free(l_cBuffer);
            break;
        case CLEAN_L0:
            memset(l_sLogStrings[0], ' ', p_structCommon->iCol);
            l_sLogStrings[0][p_structCommon->iCol] = '\0';
            break;
      case CLEAN_L1:
            memset(l_sLogStrings[1], ' ', p_structCommon->iCol);
            l_sLogStrings[1][p_structCommon->iCol] = '\0';
            break;
      case CLEAN_L2:
            memset(l_sLogStrings[2], ' ', p_structCommon->iCol);
            l_sLogStrings[2][p_structCommon->iCol] = '\0';
            break;
      default:
         break;
    }
}



/** Main
  * @brief Main function of the program, this is the starting point
  * @param argc : number of parameters gived to the program
  * @param argv : 2D array to store all the parameters gived to the program
  */
int main(int argc, char** argv)
{
	int l_iTmp, l_iIteratorLayer;
	unsigned int l_iIterator;
	char* l_cBuffer;
	char* l_cBuffer2;
	structProgramInfo* l_structCommon;

    /* Output of the log informations - put stdin for nothing -*/
    g_FILEOutputLogStream = stderr;

	/* Start the random machine */
	srand(time(NULL));

	/* Declare memory */
	l_structCommon = (structProgramInfo*)malloc(sizeof(structProgramInfo));

	/* Init values */
	l_structCommon->iRow = -1;
	l_structCommon->iCol = -1;
	l_structCommon->iOffsetX = -1;
	l_structCommon->iOffsetY = -1;
	l_structCommon->bIpV4 = TRUE;
    l_structCommon->iSizeX = DEFAULT_GRID_LENGHT;
	l_structCommon->iSizeY = DEFAULT_GRID_HEIGHT;
    l_structCommon->sUserCommand = NULL;
    l_structCommon->sServerAddress = NULL;
    l_structCommon->cUserMove = 0;
    l_structCommon->iServerSocket = 0;
    l_structCommon->bMyTurnToPlay = TRUE;
    l_structCommon->bAbleToRestartGame = FALSE;
    l_structCommon->pthreadMutex = NULL;
    l_structCommon->iLastUserRequestID = 0;
    l_structCommon->iTcpPort = TCP_PORT;
    l_structCommon->bMutexInitialized = FALSE;
    l_structCommon->bNetworkDisconnectionRequiered = FALSE;
    l_structCommon->sUserName = (char*)malloc(PARAMETER_MAX_LENGHT * sizeof(char));
    l_structCommon->bWhoHaveToPlay = (char*)malloc(MAX_CONNECTED_CLIENTS * sizeof(char));
    l_structCommon->iClientsSockets = (int*)malloc(MAX_CONNECTED_CLIENTS * sizeof(int));
    l_structCommon->iClientsColor = (unsigned int*)malloc(MAX_CONNECTED_CLIENTS * sizeof(unsigned int));
	l_iTmp = 0;

    memset(l_structCommon->iClientsColor, 0, MAX_CONNECTED_CLIENTS);
    memset(l_structCommon->iPoints, 0, MAX_CONNECTED_CLIENTS);

    l_structCommon->pthreadMutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    if(l_structCommon->pthreadMutex == NULL)
    {
        exit(ENOMEM);
    }

    l_structCommon->sServerAddress = (char*)malloc(40 * sizeof(char));  /* max ipv6 lenght */
    if(l_structCommon->sServerAddress == NULL)
    {
        exit(ENOMEM);
    }

    if(l_structCommon->sUserName == NULL)
    {
        exit(ENOMEM);
    }
    else
    {
        bzero(l_structCommon->sUserName, PARAMETER_MAX_LENGHT);
        strcpy(l_structCommon->sUserName, "n00b");
    }

    if(l_structCommon->iClientsSockets == NULL)
    {
        exit(ENOMEM);
    }

    if(l_structCommon->bWhoHaveToPlay == NULL)
    {
        exit(ENOMEM);
    }

	if(extractConfigFromCommandLine(argc, argv, l_structCommon) != 0)
	{
		/* Kill app before ncurse init, because user want to display the help message */
		exit(EXIT_FAILURE);
	}

    /*  Start the graphic mode */
    initscr();

    /*  Registeration of the final function (because since the previous line, screen is handled by ncurses) */
    atexit(killTheApp);

    /*  Hide the cursor */
    curs_set(0);

    noecho();
    cbreak();

    /*  Initialisation of some graphical elements */
    initColor();
    getmaxyx(stdscr, l_structCommon->iRow, l_structCommon->iCol);

	/* Save enought memory to display a line - Add 2 to be safe */
	l_cBuffer = (char*)malloc((l_structCommon->iCol + 2) * sizeof(char));
	l_cBuffer2 = (char*)malloc((l_structCommon->iCol + 2) * sizeof(char));
	memset(l_cBuffer, ' ', l_structCommon->iCol + 2);
	memset(l_cBuffer2, ' ', l_structCommon->iCol + 2);

    sprintf(l_cBuffer, "Lhopi - Ver %s - Rev %s", ver, rev);
    initBar();

    /*  Right message on the bottom bar - put l_iTmp as an unsigned value because, in this case, l_iTmp can't (musn't) be negative */
    for(l_iTmp=0; (unsigned)l_iTmp < l_structCommon->iCol - strlen(l_cBuffer) ; l_iTmp++)
    {
           l_cBuffer2[l_iTmp] = ' ';
           l_cBuffer2[l_iTmp+1] = '\0';
    }
    strcat(l_cBuffer2, l_cBuffer);
    botText(l_cBuffer2);


    /*  Don't ask Enter key in order to complete a getch() */
    nodelay(stdscr, 1);

	playGame(l_structCommon);

	/* Release memory */
    for(l_iIteratorLayer = 0; l_iIteratorLayer < 2 ; l_iIteratorLayer++)
    {
        for(l_iIterator = 0 ; l_iIterator < l_structCommon->iSizeY ; l_iIterator++)
        {
            free(l_structCommon->cGrid[l_iIteratorLayer][l_iIterator]);
		}
        free(l_structCommon->cGrid[l_iIteratorLayer]);
	}

    if(l_structCommon->bMutexInitialized == TRUE)
    {
        pthread_mutex_destroy(l_structCommon->pthreadMutex);
    }
    free(l_structCommon->bWhoHaveToPlay);
    free(l_structCommon->sUserName);
    free(l_structCommon->pthreadMutex);
    free(l_structCommon->iClientsSockets);
    free(l_structCommon->iClientsColor);
    free(l_structCommon->sUserCommand);
    free(l_structCommon->sServerAddress);
	free(l_structCommon->cGrid);
	free(l_cBuffer);
	free(l_cBuffer2);
	free(l_structCommon);

	exit(EXIT_SUCCESS);
	return EXIT_SUCCESS;
}
