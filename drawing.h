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
int drawElement(int, int, unsigned char, int);
void initColor(void);
void drawTheBoardGame(structProgramInfo* p_structCommon);
void displayCursor(unsigned int p_iCursorX, unsigned int p_iCursorY, unsigned int p_iOffsetX, unsigned int p_iOffsetY, char p_bForceRedraw, char*** p_cGrid);
void drawLogLine(structProgramInfo* p_structCommon, unsigned int p_iLineNumber, char* p_sLineContent);
int getNextAvailableUserColor(structProgramInfo* p_structCommon);


#endif
