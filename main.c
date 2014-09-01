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




/** @brief  Last function executed by the program, used to release the screen */
void killTheApp(void)
{
        /*  Stop the program and leave the graphic mode ! Very important ! */
        endwin();
}



/**
  * @brief Extract all usefull function from command line, and configure the program to
  *        apply wanted parameters
  * @param *p_structCommon : this is the common structure of the program, to carry all important datas
  * @param argc : number of aguments received by the main
  * @param argv : 2D array to store all parameters gived to the main
  */
void extractConfigFromCommandLine(int argc, char** argv, structProgramInfo* p_structCommon)
{
	int l_iTmp;

        if(argc > 1)
        {
                for(l_iTmp = 1; l_iTmp < argc ; l_iTmp++)
                {
                        if(strlen(argv[l_iTmp]) > 1)
                        {
                                /* Change size of the grid */
                                p_structCommon->iSizeX = (!strcmp(argv[l_iTmp], "-s")) ? atoi(argv[l_iTmp + 1]) : p_structCommon->iSizeX;
                                p_structCommon->iSizeY = (!strcmp(argv[l_iTmp], "-s")) ? atoi(argv[l_iTmp + 2]) : p_structCommon->iSizeY;
                                /*if(!strcmp(argv[l_iTmp], "-s")) {LOG_WRITE_STRING_LONG("C.LINE: ", (long)p_structCommon->iSizeX)}*/

                                /* Display help */
                                if(!strcmp(argv[l_iTmp], "-h"))
                                {
                                        endwin();
                                        printf("Lhopi - Command line use : lhopi [-h{help}] [-s Width Height{new dimensions}]\n");
                                }
                        }
                        /* Else the parameter is ignored, use strcmp on it cause a segfault */
                }
	}
}





/** Main
  * @brief Main function of the program, this is the starting point
  * @param argc : number of parameters gived to the program
  * @param argv : 2D array to store all the parameters gived to the program
  */
int main(int argc, char** argv)
{
	int l_iTmp;
	char* l_cBuffer;
	char* l_cBuffer2;
	structProgramInfo* l_structCommon;


	/* Declare memory */
	l_structCommon = (structProgramInfo*)malloc(sizeof(structProgramInfo));

	/* Init values */
	l_structCommon->iRow = -1;
	l_structCommon->iCol = -1;
	l_structCommon->iSizeX = 19;
	l_structCommon->iSizeY = 19;
	l_iTmp = 0;

	extractConfigFromCommandLine(argc, argv, l_structCommon);

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
        nodelay(stdscr, 0);

	getch();
	playGame(l_structCommon);
	getch();

	/* Release memory */
	free(l_cBuffer);
	free(l_cBuffer2);
	free(l_structCommon);

	exit(EXIT_SUCCESS);
	return EXIT_SUCCESS;
}
