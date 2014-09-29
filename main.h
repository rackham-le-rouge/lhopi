/**
 * @file        main.h
 * @program     lhopi
 * @brief       Call all functions of the rest of the program
 * @description All main functions of the program, prototypes of functions and declaration of other functions.
 * @date        8/2014
 * @licence     Beerware (rev 42)
 * @author      Jerome GRARD
 */



#ifndef MAIN_H
#define MAIN_H


/*

        Header of the main program

*/


void killTheApp(void);
int extractConfigFromCommandLine(int argc, char** argv, structProgramInfo* p_structCommon);

#endif

