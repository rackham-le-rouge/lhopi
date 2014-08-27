/**
 * @program     lhopi
 * @file        drawing.h
 * @brief       All screen drawing functions prototypes
 * @description All is in the brief
 * @date        1/2014
 * @licence     Beerware (rev 42)
 * @author      Jerome GRARD
 */



#ifndef DRAWING_H
#define DRAWING_H


/*

        Header of the graphical part of the program

*/


void initBar(void);
void disableBar(void);
void enableBar(void);
void topText(const char* p_sString);
void botText(const char* p_sString);
int drawElement(int, int, char, int);
void initColor(void);

#endif
