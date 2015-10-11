/**
 * @file        drawing.c
 * @program     lhopi
 * @brief       All screen drawing functions
 * @description Here it is all of the drawing functions, including the splash and the help message.
 *              All ncurses stuff is here too.
 * @date        8/2014
 * @licence     Beerware (rev 42)
 * @author      Jerome GRARD
 */


#include "conf.h"

extern FILE* g_FILEOutputLogStream;


/** @brief Blues lines - Top and bottom of the screen */
void initBar(void)
{
        int l_iRow,l_iCol,i;                            /* to store the number of rows and */
        getmaxyx(stdscr,l_iRow,l_iCol);                 /* get the number of rows and columns */
        start_color();                                  /* start color mode */
        init_pair(1, COLOR_BLACK, COLOR_LINE_BG_BOTTOM);   /* référence des couleurs */
        attron(COLOR_PAIR(1));
        /* Bottom line */
        for(i = 0; i<l_iCol ; i++)
                mvprintw(l_iRow-1,i," ");
        /* Top line */
        attroff(COLOR_PAIR(1));
        init_pair(2, COLOR_BLACK, COLOR_LINE_BG_TOP);   /* référence des couleurs */
        attron(COLOR_PAIR(2));

        for(i = 0; i<l_iCol ; i++)
                mvprintw(0,i," ");

        attroff(COLOR_PAIR(2));
}



/** @brief Text of the top line */
void topText(const char* p_sString)
{
        start_color();                                  /* start color mode */
        attron(COLOR_PAIR(2));
        mvprintw(0,0,"%s", p_sString);
        attroff(COLOR_PAIR(2));
}



/** @brief Text of the bottom line
  * @param p_sString : string to print at the bottom of the screen
  */
void botText(const char* p_sString)
{
        int l_iRow,l_iCol;                              /* to store the number of rows and */
        getmaxyx(stdscr,l_iRow,l_iCol);                 /* get the number of rows and columns */
        start_color();                                  /* start color mode */
        attron(COLOR_PAIR(1));
        mvprintw(l_iRow-1,0,"%s", p_sString);

        attroff(COLOR_PAIR(1));
        l_iCol++;
}




/** @brief Disable the two lines */
void disableBar(void)
{
        start_color();                                  /* start color mode */
        init_pair(1, COLOR_BLACK, COLOR_BLACK);         /* référence des couleurs */

        init_pair(2, COLOR_BLACK, COLOR_BLACK);         /* référence des couleurs */
}




/** @brief Draw an element on the screen
  * @param p_iX : X coord
  * @param p_iY : Y coord
  * @param p_cChar : the character to print
  * @param p_iColor : color, Cf the enum
  * @return 0
  */
int drawElement(int p_iX, int p_iY, unsigned char p_cChar, int p_iColor)
{
        int l_iRow,l_iCol;                              /* to store the number of rows and */
        getmaxyx(stdscr,l_iRow,l_iCol);                 /* get the number of rows and columns */

        /* Graphical binding */
        if(p_iX>l_iCol) return -1;
        if(p_iY>l_iRow) return -1;

        start_color();                                  /* start color mode */
        attron(COLOR_PAIR(p_iColor));

        /* Draw the element */
        mvprintw(p_iY,p_iX,"%c", p_cChar);

        attroff(COLOR_PAIR(p_iColor));
        return 0;
}





/** @brief Initialisation of the colors of each kind of people
  */
void initColor(void)
{
        start_color();

#ifdef GRAPHIC_MODE
        init_pair(enumNoir, COLOR_BLACK, COLOR_BLACK);
        init_pair(enumRouge, COLOR_BLACK, COLOR_RED);
        init_pair(enumVert, COLOR_BLACK, COLOR_GREEN);
        init_pair(enumJaune, COLOR_BLACK, COLOR_YELLOW);
        init_pair(enumBleu, COLOR_BLACK, COLOR_BLUE);
        init_pair(enumMagenta, COLOR_BLACK, COLOR_MAGENTA);
        init_pair(enumCyan, COLOR_BLACK, COLOR_CYAN);
        init_pair(enumBlanc, COLOR_BLACK, COLOR_WHITE);
        init_pair(enumConsole, COLOR_BLACK, COLOR_WHITE);
        init_pair(enumLine, CONSOLE_LINE_COLOR, COLOR_BLACK);
        init_pair(enumLogLine, CONSOLE_LOGTEXT_COLOR, COLOR_BLACK);
        init_pair(enumBoardLine, CONSOLE_BOARDLINE_COLOR, COLOR_BLACK);

        init_pair(enumLetterRed, COLOR_RED, COLOR_BLACK);
        init_pair(enumLetterGreen, COLOR_GREEN, COLOR_BLACK);
        init_pair(enumLetterYellow, COLOR_YELLOW, COLOR_BLACK);
        init_pair(enumLetterBlue, COLOR_BLUE, COLOR_BLACK);
        init_pair(enumLetterPurple, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(enumLetterCyan, COLOR_CYAN, COLOR_BLACK);
        init_pair(enumLetterWhite, COLOR_WHITE, COLOR_BLACK);
 #else
        init_pair(enumNoir, COLOR_BLACK, COLOR_BLACK);
        init_pair(enumRouge, COLOR_RED, COLOR_BLACK);
        init_pair(enumVert, COLOR_GREEN, COLOR_BLACK);
        init_pair(enumJaune, COLOR_YELLOW, COLOR_BLACK);
        init_pair(enumBleu, COLOR_BLUE, COLOR_BLACK);
        init_pair(enumMagenta, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(enumCyan, COLOR_CYAN, COLOR_BLACK);
        init_pair(enumBlanc, COLOR_WHITE, COLOR_BLACK);
        init_pair(enumConsole, COLOR_BLACK, CONSOLE_WHITE);
        init_pair(enumLine, CONSOLE_LINE_COLOR, COLOR_BLACK);
        init_pair(enumLogLine, CONSOLE_LOGTEXT_COLOR, COLOR_BLACK);
        init_pair(enumBoardLine, CONSOLE_BOARDLINE_COLOR, COLOR_BLACK);
#endif
}



/** @brief This function have to draw the board game taking care of the size of the screen
  * @param p_structCommon : Struct with all the program information
  */
void drawTheBoardGame(structProgramInfo* p_structCommon)
{
	unsigned int l_iIterateur;
    unsigned int l_iCursorY;
    unsigned int l_iCursorX;

	l_iIterateur = 0;

	/* Clean bars */
	initBar();





    start_color();                                  /* start color mode */
    attron(COLOR_PAIR(enumBoardLine));


    /**** BOARD LIMIT ****/
    /* Upper Left */
    mvaddch(p_structCommon->iOffsetY - 1, p_structCommon->iOffsetX - 1, ACS_ULCORNER );
    for(l_iIterateur = 0; l_iIterateur < p_structCommon->iSizeX; l_iIterateur++)
    {
        /* Draw the horizontal lines */
        mvaddch(p_structCommon->iOffsetY - 1, l_iIterateur + p_structCommon->iOffsetX, ACS_HLINE);
        mvaddch(p_structCommon->iOffsetY + p_structCommon->iSizeY, l_iIterateur + p_structCommon->iOffsetX, ACS_HLINE);
    }
    /* Upper right */
    mvaddch(p_structCommon->iOffsetY - 1, p_structCommon->iOffsetX + p_structCommon->iSizeX , ACS_URCORNER );

    for(l_iIterateur = 0; l_iIterateur < p_structCommon->iSizeY; l_iIterateur++)
    {
        /* Draw the vertical lines */
        mvaddch(p_structCommon->iOffsetY + l_iIterateur, p_structCommon->iOffsetX - 1, ACS_VLINE);
        mvaddch(p_structCommon->iOffsetY + l_iIterateur, p_structCommon->iOffsetX + p_structCommon->iSizeX, ACS_VLINE);
    }
    mvaddch(p_structCommon->iOffsetY + p_structCommon->iSizeY, p_structCommon->iOffsetX - 1, ACS_LLCORNER );
    mvaddch(p_structCommon->iOffsetY + p_structCommon->iSizeY, p_structCommon->iOffsetX + p_structCommon->iSizeX, ACS_LRCORNER );




    /**** PLAYER LIST ****/
    /* Upper Left */
    mvaddch((p_structCommon->iRow - MAX_CONNECTED_CLIENTS + 2) / 2, p_structCommon->iCol - 22, ACS_ULCORNER );
    for(l_iIterateur = 0; l_iIterateur < 20; l_iIterateur++)
    {
        /* Draw the horizontal lines */
        mvaddch((p_structCommon->iRow - MAX_CONNECTED_CLIENTS + 2) / 2, l_iIterateur + p_structCommon->iCol - 21, ACS_HLINE);
        mvaddch((p_structCommon->iRow - MAX_CONNECTED_CLIENTS + 2) / 2 + MAX_CONNECTED_CLIENTS + 1, l_iIterateur + p_structCommon->iCol - 21, ACS_HLINE);
    }
    /* Upper right */
    mvaddch((p_structCommon->iRow - MAX_CONNECTED_CLIENTS + 2) / 2, p_structCommon->iCol - 1, ACS_URCORNER );

    for(l_iIterateur = 0; l_iIterateur < MAX_CONNECTED_CLIENTS; l_iIterateur++)
    {
        /* Draw the vertical lines */
        mvaddch((p_structCommon->iRow - MAX_CONNECTED_CLIENTS + 2) / 2 + 1 + l_iIterateur, p_structCommon->iCol - 22, ACS_VLINE);
        mvaddch((p_structCommon->iRow - MAX_CONNECTED_CLIENTS + 2) / 2 + 1 + l_iIterateur, p_structCommon->iCol - 1, ACS_VLINE);
    }
    mvaddch((p_structCommon->iRow - MAX_CONNECTED_CLIENTS + 2) / 2 + MAX_CONNECTED_CLIENTS + 1, p_structCommon->iCol - 22, ACS_LLCORNER );
    mvaddch((p_structCommon->iRow - MAX_CONNECTED_CLIENTS + 2) / 2 + MAX_CONNECTED_CLIENTS + 1, p_structCommon->iCol - 1, ACS_LRCORNER );


    drawElement(p_structCommon->iCol - 19, (p_structCommon->iRow - MAX_CONNECTED_CLIENTS + 2) / 2, 'P' , enumLine);
    drawElement(p_structCommon->iCol - 18, (p_structCommon->iRow - MAX_CONNECTED_CLIENTS + 2) / 2, 'l' , enumLine);
    drawElement(p_structCommon->iCol - 17, (p_structCommon->iRow - MAX_CONNECTED_CLIENTS + 2) / 2, 'a' , enumLine);
    drawElement(p_structCommon->iCol - 16, (p_structCommon->iRow - MAX_CONNECTED_CLIENTS + 2) / 2, 'y' , enumLine);
    drawElement(p_structCommon->iCol - 15, (p_structCommon->iRow - MAX_CONNECTED_CLIENTS + 2) / 2, 'e' , enumLine);
    drawElement(p_structCommon->iCol - 14, (p_structCommon->iRow - MAX_CONNECTED_CLIENTS + 2) / 2, 'r' , enumLine);
    drawElement(p_structCommon->iCol - 13, (p_structCommon->iRow - MAX_CONNECTED_CLIENTS + 2) / 2, 's' , enumLine);
 
    attroff(COLOR_PAIR(enumBoardLine));







    /* Draw the whole board game */
    for(l_iCursorY = 0; l_iCursorY < p_structCommon->iSizeY; l_iCursorY++)
    {
        for(l_iCursorX = 0; l_iCursorX < p_structCommon->iSizeX; l_iCursorX++)
        {
            drawElement(p_structCommon->iOffsetX + l_iCursorX,
                        p_structCommon->iOffsetY + l_iCursorY,
					    p_structCommon->cGrid[TEXT_MATRIX][l_iCursorY][l_iCursorX],
					    p_structCommon->cGrid[COLOR_MATRIX][l_iCursorY][l_iCursorX]);
        }
    }


	for (l_iIterateur = 0; l_iIterateur < p_structCommon->iCol ; l_iIterateur++)
	{
		drawElement(l_iIterateur, p_structCommon->iRow - (CONSOLE_SPACE_ON_BOARD_BOTTOM + 1), '_' , enumLine);
		/* We put -1 in order to take care of the scpace taken by this line */
	}
}



/** @brief Function to display the cursor, at the cursor position
  * @param p_iCursorX : Cursor column
  * @param p_iCursorY : Cursor row
  * @param p_cGrid : Three dimentional tabular storing all values of the game board
  */
void displayCursor(unsigned int p_iCursorX, unsigned int p_iCursorY, unsigned int p_iOffsetX, unsigned int p_iOffsetY, char p_bForceRedraw, char*** p_cGrid)
{
	/* Init of static values, in order to erase former cursor position */
	static unsigned int l_iPreviousCursorXPosition = 0;
	static unsigned int l_iPreviousCursorYPosition = 1;

	/* Avoid redraw because it will erase the cursor and not redraw it */
	if(!(l_iPreviousCursorXPosition == p_iCursorX && l_iPreviousCursorYPosition == p_iCursorY) || p_bForceRedraw == TRUE)
	{
		/* draw the cursor */
		drawElement(p_iCursorX + p_iOffsetX, p_iCursorY + p_iOffsetY, ' ' , enumConsole);

		/* re-draw the grid on the former position of the cursor */
        if(p_bForceRedraw != TRUE)
        {
            drawElement(l_iPreviousCursorXPosition + p_iOffsetX,
                l_iPreviousCursorYPosition + p_iOffsetY,
                p_cGrid[TEXT_MATRIX][l_iPreviousCursorYPosition][l_iPreviousCursorXPosition],
                p_cGrid[COLOR_MATRIX][l_iPreviousCursorYPosition][l_iPreviousCursorXPosition]);
        }

		/* refresh of the 'previous' values */
		l_iPreviousCursorXPosition = p_iCursorX;
		l_iPreviousCursorYPosition = p_iCursorY;

		refresh();
	}
}


/** @brief This function have to draw the board game taking care of the size of the screen
  * Colors available. Usage : "there is a ##5string for test##7 to show colors"
  * Here, "there is a " is printed with the default color for the log lines, "string for test" is printed with
  * the color 5, Cf g_enumJeuxDeCouleursDispo to see all available colors. ##7 reset color to the default one,
  * and then the last part of the string is printed with the default log line colors.
  * @param p_structCommon : Struct with all the program information
  */
void drawLogLine(structProgramInfo* p_structCommon, unsigned int p_iLineNumber, char* p_sLineContent)
{
	unsigned int l_iIterateur;
    unsigned int l_iX;
    unsigned int l_iUserNameIndex;
    unsigned int l_iLastUserColor;
    int     l_iColor = enumLogLine;
    char    l_sBuffer[USER_COMMAND_LENGHT + 3];
    char    l_sUserName[22];                        /* Because displaying space have 20 characters */

    l_iX = 0;
	l_iIterateur = 0;
    l_iUserNameIndex = 0;
    l_iLastUserColor = 0;
    bzero(l_sBuffer, USER_COMMAND_LENGHT + 3);
    bzero(l_sUserName, 22);
    strcpy(l_sBuffer, p_sLineContent);
	for (l_iIterateur=0; l_iIterateur < p_structCommon->iCol ; l_iIterateur++)
	{
        usleep(2);
        /* New color detected */
        if(l_sBuffer[l_iIterateur] == '#' && l_sBuffer[l_iIterateur + 1] == '#')
        {
            l_iColor = atoi(l_sBuffer + l_iIterateur + 2);
            /* one or two figures possible / have to get a rid of them */
            if(l_iColor < 10)
            {
                l_sBuffer[l_iIterateur + 1] = '^';
            }
            else
            {
                l_sBuffer[l_iIterateur + 1] = '^';
                l_sBuffer[l_iIterateur + 2] = '^';
            }
            if(l_iColor != 7)
            {
                l_iLastUserColor = l_iColor;
            }

            /* Reserved color for the rest of the program. Here, used to reset the log color */
            if(l_iColor == 7)
            {
                if(l_iLastUserColor > 26) l_iLastUserColor--;   /* Because 27 doesn't exist, because color enum 7 is taken for service reasons */
                printUserName(l_sUserName, l_iLastUserColor, p_structCommon);

                if(l_iLastUserColor - 20 == p_structCommon->iCurrentUserColor)
                {
                    drawElement(p_structCommon->iCol - 20, (p_structCommon->iRow - MAX_CONNECTED_CLIENTS + 2) / 2 + 1 + l_iLastUserColor - enumLetterRed, '>', l_iLastUserColor);
                }

                l_iUserNameIndex = 0;
                bzero(l_sUserName, 22);
                l_iColor = enumLogLine;
            }
        }
        /* Useless character. Used to stuff the line */
        else if(l_sBuffer[l_iIterateur] == '^')
        {
            /* do nothing */
        }
        /* When we are on the second # of a color redifinition */
        else if(l_sBuffer[l_iIterateur] == '#' && l_sBuffer[l_iIterateur + 1] != '#')
        {
            /* do nothing */
        }
        /* When we are on a normal case */
        else if((l_iIterateur > 0 && l_sBuffer[l_iIterateur - 1] != '#' && l_sBuffer[l_iIterateur - 1] != '^') ||       /* On any char of the string */
                (l_iIterateur == 0 && l_sBuffer[l_iIterateur] != '#' && l_sBuffer[l_iIterateur] != '^'))                /* On the first char */
        {
            if(l_iColor != enumLogLine && l_iUserNameIndex < 21)
            {
                l_sUserName[l_iUserNameIndex++] = p_sLineContent[l_iIterateur];
            }
		    drawElement(l_iX++, p_structCommon->iRow - (CONSOLE_SPACE_ON_BOARD_BOTTOM ) + p_iLineNumber, p_sLineContent[l_iIterateur] , l_iColor);
        }
		/* We put -1 in order to take care of the scpace taken by this line */
	}
}

/** @brief This function send back the next available color for new user
    @param p_structCommon : Struct with all the program information
    @return The free color, if not, -1 */
int getNextAvailableUserColor(structProgramInfo* p_structCommon)
{
    unsigned int l_iIterator;
    unsigned int l_iPossibleColor;
    char l_bColorFound = FALSE;

    l_iIterator = 0;
    l_iPossibleColor = 0;

    for(l_iPossibleColor = enumVert; l_iPossibleColor < enumVert + MAX_CONNECTED_CLIENTS; l_iPossibleColor++)
    {
        /* Really dirty - Cf conf.h color 7 is reserved, we have to jump over it */
        if(l_iPossibleColor == 7)
        {
            l_iPossibleColor = enumBlanc;
        }

        for(l_iIterator = 0; l_iIterator < MAX_CONNECTED_CLIENTS; l_iIterator++)
        {
            if(p_structCommon->iClientsColor[l_iIterator] == l_iPossibleColor)
            {
                l_bColorFound = TRUE;
            }
        }

        if (l_bColorFound == FALSE)
        {
            return l_iPossibleColor;
        }
        l_bColorFound = FALSE;

        /* Really dirty - Cf conf.h color 7 is reserved, we have to jump over it */
        if(l_iPossibleColor == enumBlanc)
        {
            l_iPossibleColor = 7;
        }
    }

    return -1;
}


/**
  * @brief Function to print user name with its color at the right place on the screen
  * @param p_sUserName : the name of the user
  * @param p_iUserColor : color (please refer to the enum). Based on thjis enum, we compute the Y coordinate where to print the name
  * @param p_structCommon : Struct with all the program information
  */
void printUserName(char* p_sUserName, unsigned int p_iUserColor, structProgramInfo* p_structCommon)
{
    unsigned int l_iIterator;

    /* Use 20 because it is the max len of nick displayed */
    for(l_iIterator = 0; l_iIterator < 20; l_iIterator++)
    {
        drawElement(p_structCommon->iCol - 19 + l_iIterator, (p_structCommon->iRow - MAX_CONNECTED_CLIENTS + 2) / 2 + 1 + p_iUserColor - enumLetterRed,  *(p_sUserName + l_iIterator), p_iUserColor);
    }
}

