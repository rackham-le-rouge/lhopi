/**
 * @program     lhopi
 * @file        networking.h
 * @brief       All network function to connect and receive/send
 * @description Functions are enclosed in differents threads in order to allow them to don't stop the main program
 * @date        9/2015
 * @licence     Beerware (rev 42)
 * @author      Jerome GRARD
 */



#ifndef NETWORKING_H
#define NETWORKING_H


/*

        Header of the networking part of the program

*/

void error(const char *msg);
int tcpSocketServer(structProgramInfo* p_structCommon);
int tcpSocketClient(structProgramInfo* p_structCommon);



#endif
