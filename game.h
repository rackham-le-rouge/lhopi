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
int loopCompletion(unsigned int p_iCursorX, unsigned int p_iCursorY, unsigned int p_iOffsetX, unsigned int p_iOffsetY, structProgramInfo* p_structCommon);
void cleanGridLayer(unsigned int p_iLayerOrig, int  p_iToReplace, unsigned int p_iLayerDest, int p_iFillingValue, unsigned int p_iOffsetX, unsigned int p_iOffsetY, structProgramInfo* p_structCommon);
int recursiveDiscovery(unsigned int p_iHop, unsigned int p_iY, unsigned int p_iX, structProgramInfo* p_structCommon);
int recursiveEmptyFilling(unsigned int p_iY, unsigned int p_iX, structProgramInfo* p_structCommon);
void playGame(structProgramInfo* p_structCommon);

#endif
