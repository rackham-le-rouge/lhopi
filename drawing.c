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
int drawElement(int p_iX, int p_iY, char p_cChar, int p_iColor)
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
#else
        init_pair(enumNoir, COLOR_BLACK, COLOR_BLACK);
        init_pair(enumRouge, COLOR_RED, COLOR_BLACK);
        init_pair(enumVert, COLOR_GREEN, COLOR_BLACK);
        init_pair(enumJaune, COLOR_YELLOW, COLOR_BLACK);
        init_pair(enumBleu, COLOR_BLUE, COLOR_BLACK);
        init_pair(enumMagenta, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(enumCyan, COLOR_CYAN, COLOR_BLACK);
        init_pair(enumBlanc, COLOR_WHITE, COLOR_BLACK);
#endif
}



