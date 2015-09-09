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

	l_iIterateur = 0;

	/* Clean bars */
	initBar();

    start_color();                                  /* start color mode */
    attron(COLOR_PAIR(enumBoardLine));

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
 
    attroff(COLOR_PAIR(enumBoardLine));



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
void displayCursor(unsigned int p_iCursorX, unsigned int p_iCursorY, unsigned int p_iOffsetX, unsigned int p_iOffsetY, char*** p_cGrid)
{
	/* Init of static values, in order to erase former cursor position */
	static unsigned int l_iPreviousCursorXPosition = 0;
	static unsigned int l_iPreviousCursorYPosition = 1;

	/* Avoid redraw because it will erase the cursor and not redraw it */
	if(!(l_iPreviousCursorXPosition == p_iCursorX && l_iPreviousCursorYPosition == p_iCursorY))
	{
		/* draw the cursor */
		drawElement(p_iCursorX + p_iOffsetX, p_iCursorY + p_iOffsetY, ' ' , enumConsole);

		/* re-draw the grid on the former position of the cursor */
		drawElement(l_iPreviousCursorXPosition + p_iOffsetX,
			l_iPreviousCursorYPosition + p_iOffsetY,
			p_cGrid[TEXT_MATRIX][l_iPreviousCursorYPosition][l_iPreviousCursorXPosition],
			p_cGrid[COLOR_MATRIX][l_iPreviousCursorYPosition][l_iPreviousCursorXPosition]);
		/* refresh of the 'previous' values */
		l_iPreviousCursorXPosition = p_iCursorX;
		l_iPreviousCursorYPosition = p_iCursorY;

		refresh();
	}
}


/** @brief This function have to draw the board game taking care of the size of the screen
  * @param p_structCommon : Struct with all the program information
  */
void drawLogLine(structProgramInfo* p_structCommon, unsigned int p_iLineNumber, char* p_sLineContent)
{
	unsigned int l_iIterateur;

	l_iIterateur = 0;

	for (l_iIterateur=0; l_iIterateur < p_structCommon->iCol ; l_iIterateur++)
	{
		drawElement(l_iIterateur, p_structCommon->iRow - (CONSOLE_SPACE_ON_BOARD_BOTTOM ) + p_iLineNumber, p_sLineContent[l_iIterateur] , enumLogLine);
		/* We put -1 in order to take care of the scpace taken by this line */
	}
}

