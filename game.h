/**
 * @file        game.h
 * @rogram      Lhopi
 * @brief       The game part - Informatic implementation of the rules
 * @description There is all the player stuff, how to the gamer moves on the grid, allowed and forbidden acts etc...
 * @date        8/2014
 * @licence     Beerware (rev 42)
 * @author      Jerome GRARD
 */

#ifndef	GAME_H
#define	GAME_H


void gameInit(structProgramInfo* p_structCommon);
int loopCompletion(unsigned int p_iCursorX, unsigned int p_iCursorY,structProgramInfo* p_structCommon);
void cleanGridLayer(unsigned int p_iLayer, unsigned char p_cFillingValue, structProgramInfo* p_structCommon);
int markNeighbour(unsigned int p_iCursorX, unsigned int p_iCursorY, structProgramInfo* p_structCommon);
int loopBrowsing(structProgramInfo* p_structCommon);
void playGame(structProgramInfo* p_structCommon);

#endif
