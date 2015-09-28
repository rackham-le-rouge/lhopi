/**
 * @program     lhopi
 * @file        networking.c
 * @brief       All network function to connect and receive/send
 * @description Functions are enclosed in differents threads in order to allow them to don't stop the main program
 * @date        9/2015
 * @licence     Beerware (rev 42)
 * @author      Jerome GRARD
 */


#include "conf.h"


extern FILE* g_FILEOutputLogStream;


/**
  * General help on the network part    -> means call | ==> means start a thread and continue its execution | o means thread is close normally | X means thread is killed
  *
  *
  * The main point of this doc is to show differences between server and client side in the way to handle receive / analyse / send part. The other main point is
  * to show the differences when leaving is requiered. We can leave when the other side ask to leave, so we have to close our side of the connection ; and we can
  * also leave when user want to quit the program. So, we have to signal it to the other side. When it is a client, it is simple (cause there is just one person
  * to warn) but for a server, we have all clients to warn. Moreover, for a server, when asking is asked by user, we have to kill all threads, and the mecanism to
  * wait until all threads are dead -properly please- is implemented in the caller -the caller have to know that it is a server, and pool the client socket table
  * until there is only 0 in it.
  *
  * A ping pong mechanism was setted in order to have permanent communication between the two sides. read() is a blocking function, and i dont want to have more
  * thread level, so, i do a permanent communication (stream is setted by the waiting time between the packets, this is a parameter of config.h). Client starts
  * by sending a ping, server reply a pong when it receive the ping and so on. Server send only pong, and client only ping.
  *
  *
  * Client
  * ------
  *
  * Call from outside -> tcpSocketClient() (the interface for client) ==> clientConnectionThread()
  *
  * clientConnectionThread
  * |
  * |client ask for init information with request r0000
  * |while(we_dont_ask_to_quit)
  * | * read part *
  * | read data from server (send back pong, execute commands etc...)
  * | read specific data from server, init values provided by request r0001, 2, 3. And send back ack0001, 2, 3.
  * | analyse data from server
  * | if server ask to close the connection, leave the loop
  * | * exit part *
  * | if the user in client ask to quit, send a quit message to the server, and ask to leave to loop
  * | * sending part *
  * | if there is something to send in the buffer, send it to the server
  * |end while
  * |
  * |if we reach this point, loop have been killed, thus something ask to quit network functions, thus clean socket, close it
  * |in order to say to the caller that evrything is OK, down the flag bNetworkDisconnectionRequiered. The caller is poolling this signal.
  * o
  *
  *
  *
  *
  *
  *
  *
  * Server
  * ------
  *
  * Call from outside -> tcpSocketServer() (interface for server) ==> waitingForNewConnectionsThread ==> tcpSocketServerConnectionHander()
  *                                                                                                  |==> tcpSocketServerConnectionHander()
  *                                                                                                  |==> etc...
  * tcpSocketServer
  * |
  * | waitingForNewConnectionsThread
  * o |
  *   | initialize main socket. This socket is not for a connection, just to wait incoming connections.
  *   | if there is any network error in this function, release the mutex, clean it, clean the soket, and close the server-waiting thread. Server function is disabled until user ask a new start.
  *   | wait until there is a connection asked by a client
  *   | |
  *   | | start a thread for this client
  *   | | |
  *   | | | If there is any error in this thread, I/O, connection, network, etc...  clean soket value in the p_structCommon->iClientsSockets table by putting 0 in it. Close socket. Close the thread.
  *   | | | while we dont ask to leave the loop
  *   | | | | * reception from client *
  *   | | | | receive data from socket, analyse it and execute it. pong data is sent directly in this part.
  *   | | | | send init info to the client, when ack0003 -the last one- is received send back a pong to init ping-pong system and keep the connection working
  *   | | | | if exit is requiered by client, leave the loop
  *   | | | | * exit asked * in this case, exiting is asked by the user on the server-sided program
  *   | | | | ask to leave the loop
  *   | | | | * sending part *
  *   | | | | if there is something in the buffer 'sosend' send it and fluch the buffer
  *   | | | end of the loop
  *   | | |
  *   | | | if we are here, something ask to kill this client, thus close the socket, but not close mutex (some other thread may need it) and not down the bNetworkDisconnectionRequiered flag
  *   | | | because we may are not the last thread. So, for the server side, just put 0 in the socket table p_structCommon->iClientsSockets[the_socket]. The caller is pooling this table
  *   | | | and wait for a full-0 table. And close the thread.
  *   | | o
  *   | | this thread is waiting for new connections, so when exit was asked, it gonna be killed. Maybee this system have to be improved...
  *   | /
  *   |/
  *   X
  *
  */






/*******************************************
 *
 *           Thread Safe log part
 *   Because access to the graphic methods
 *   needs to be thread safe. Add mutexes.
 *
 *******************************************/

/**
  * @brief This function is used to display log in the logBar in a thread context because you have to call mutexes in order to protect displaying functions called by logBar
  * @param p_structCommon : all usefull datas for this program
  * @param p_enumBarWantedAction : action asked to the logBar() function
  * @param p_sNewLine : line to display, like in a logBar call
  * @return nothing
  */
void threadSafeLogBar(structProgramInfo* p_structCommon, g_enumLogBar p_enumBarWantedAction, const char* p_sNewLine)
{
    pthread_mutex_lock(p_structCommon->pthreadMutex);

    logBar(p_structCommon, p_enumBarWantedAction, p_sNewLine);
    logBar(p_structCommon, DISPLAY, "");

    pthread_mutex_unlock(p_structCommon->pthreadMutex);
}



/*******************************************
 *
 *             Server part
 *   Accept Client connections in separated
 *   threads.  And manage all incoming con-
 *   nections in a saparated thread.
 *
 *******************************************/



/** @brief PUBLIC INTERFACE
  * Start this function in order to have a TCP server, binding port TCP_PORT
  * This function starts a thread in charge of waiting for incoming connections.
  * @param p_structCommon : all usefull data of the program
  * @return -1 if error during starting the waiting thread, 0 if the thread starts correctly. Nothing matter with the TCP / Socket state
  */
int tcpSocketServer(structProgramInfo* p_structCommon)
{
    pthread_t l_structWaitingThreadID;

    pthread_mutex_init(p_structCommon->pthreadMutex, NULL);
    p_structCommon->bMutexInitialized = TRUE;

    if(pthread_create(&l_structWaitingThreadID,NULL, waitingForNewConnectionsThread, (void*)p_structCommon) < 0)
    {
        log_err("Could not create the WaitingForNewConnectionsThread %s", " ");
        pthread_mutex_destroy(p_structCommon->pthreadMutex);
        p_structCommon->bMutexInitialized = FALSE;
        return -1;
    }

    return 0;
}


/** @brief Waiting thread. This thread is on the server side, waiting for incoming client connections.
  * This thread starts the 'master' socket, bind the port, and wait for connections. It accept connections
  * and starts threads for each one in order to dialog with clients.
  * @param p_structCommon : all usefull data of the program
  * @return 0 cause this is a thread. Another system is used to have status sent back
  */
void* waitingForNewConnectionsThread(void* p_structCommonShared)
{
    structProgramInfo* p_structCommon = (structProgramInfo*)p_structCommonShared;

    int l_iSocket;
    int l_iSocketNewConnection;
    int l_iSocketCounter;
    socklen_t l_structClientLen;
    char l_cBufferTransmittedData[USER_COMMAND_LENGHT];
    struct sockaddr_in l_structServAddr;
    struct sockaddr_in l_structClientAddr;
    pthread_t l_structThreadID;

    bzero((char *) &l_structServAddr, sizeof(l_structServAddr));
    bzero(l_cBufferTransmittedData, USER_COMMAND_LENGHT);

    l_iSocketCounter = 0;
    l_structServAddr.sin_family = AF_INET;
    l_structServAddr.sin_addr.s_addr = INADDR_ANY;                                        
    l_structServAddr.sin_port = htons(TCP_PORT);

    l_iSocket = socket(AF_INET, SOCK_STREAM, 0); 

    log_msg("Socket-server: Waiting thread started");

    if (l_iSocket < 0)
    { 
        log_err("Socket-server: error opening socket. err %d", errno);
        log_msg("Socket-server: Waiting thread closed on error");
        pthread_mutex_destroy(p_structCommon->pthreadMutex);
        p_structCommon->bMutexInitialized = FALSE;
        return 0;
    }

    if (bind(l_iSocket, (struct sockaddr *) &l_structServAddr, sizeof(l_structServAddr)) < 0)
    {
        log_err("Socket-server: error binding port. errno %d", errno);
        log_msg("Socket-server: Waiting thread closed on error");
        pthread_mutex_destroy(p_structCommon->pthreadMutex);
        p_structCommon->bMutexInitialized = FALSE;
        close(l_iSocket);
        return 0;
    }

    if(listen(l_iSocket, MAX_CONNECTED_CLIENTS) < 0)
    {
        log_err("Socket-server: listen failed. errno %d", errno);
        log_msg("Socket-server: Waiting thread closed on error");
        pthread_mutex_destroy(p_structCommon->pthreadMutex);
        p_structCommon->bMutexInitialized = FALSE;
        close(l_iSocket);
        return 0;
    }

    l_structClientLen = sizeof(l_structClientAddr);

    threadSafeLogBar(p_structCommon, ADD_LINE, "Waiting for incomming connections !");
    threadSafeLogBar(p_structCommon, DISPLAY, "");


    /* Add all clients */
    while((l_iSocketNewConnection = accept(l_iSocket, (struct sockaddr *) &l_structClientAddr, &l_structClientLen)))
    {
        log_msg("Socket-server: Waiting thread received a client co request. Start a new thread...");
        log_info("Socket-server: Starting thread have the index %d/%d and the Socket is %d", l_iSocketCounter + 1, MAX_CONNECTED_CLIENTS, l_iSocketNewConnection);
        p_structCommon->iClientsSockets[l_iSocketCounter++] = l_iSocketNewConnection;
        if(pthread_create( &l_structThreadID , NULL ,  tcpSocketServerConnectionHander , (void*) p_structCommon) < 0)
        {
            log_err("Could not create the thread %s", " ");
            log_msg("Socket-server: Waiting thread closed on error");
            pthread_mutex_destroy(p_structCommon->pthreadMutex);
            p_structCommon->bMutexInitialized = FALSE;
            close(l_iSocket);
            return 0;
        }

        if(l_iSocketCounter >= MAX_CONNECTED_CLIENTS)
        {
            log_err("Socket-server: can't have more client connected %s", " ");

            threadSafeLogBar(p_structCommon, ADD_LINE, "Max network users reached. Refusing connections.");
            threadSafeLogBar(p_structCommon, DISPLAY, "");

            log_msg("Socket-server: Waiting thread closed on error");
            pthread_mutex_destroy(p_structCommon->pthreadMutex);
            p_structCommon->bMutexInitialized = FALSE;
            close(l_iSocket);
            return 0;
        }
    }

    /* If failed to accept a connection */
    if (l_iSocketNewConnection < 0)
    {
        log_err("Socket-server: Connection requested by peer, but failed to establish. Retrieved socket is empty. errno %d", errno);
        log_msg("Socket-server: Waiting thread closed on error");
        close(l_iSocket);
        pthread_mutex_destroy(p_structCommon->pthreadMutex);
        p_structCommon->bMutexInitialized = FALSE;
        return 0;
    }

    log_msg("Socket-server: Waiting thread closed normally");
    close(l_iSocket);
    pthread_mutex_destroy(p_structCommon->pthreadMutex);
    p_structCommon->bMutexInitialized = FALSE;
    return 0;
}


/** @brief Connection thread on the server side. This function is launched in a thread and have to manage connection with
  * _a_ client. This threads die when client close connection or when there is a writing or reading issue.
  * this function is cut in three parts. Receiving from client and analyse the sent data. Quit, only if the client sent a quit order
  * -so close the connection and the thread properly- and a sending part to communicate data from the part to the client.
  * @param p_structCommon : all usefull data of the program
  * @return 0 cause this is a thread. Another system is used to have status sent back
  */
void* tcpSocketServerConnectionHander(void* p_structCommonShared)
{
    structProgramInfo* p_structCommon = (structProgramInfo*)p_structCommonShared;

    char l_cBufferTransmittedData[USER_COMMAND_LENGHT];
    char l_cBufferToSendData[USER_COMMAND_LENGHT];
    char l_bExit;
    int l_iReturnedReadWriteValue;
    int l_iCurrentSocketIndex;
    int l_iClientRequestInit;
    int l_iCursorX;
    int l_iCursorY;
    int l_iPotentialNewRockX;
    int l_iPotentialNewRockY;
    int l_iVerificationIterator;
    int l_iWatchdog;
    unsigned int l_iCursorBrowseringX;
    unsigned int l_iCursorBrowseringY;

    l_bExit = FALSE;
    l_iCurrentSocketIndex = MAX_CONNECTED_CLIENTS - 1;
    l_iClientRequestInit = 0;
    l_iCursorX = 1;
    l_iCursorY = 1;
    l_iCursorBrowseringX = 0;
    l_iCursorBrowseringY = 0;
    l_iPotentialNewRockX = -1;
    l_iPotentialNewRockY = -1;
    l_iVerificationIterator = 0;
    l_iWatchdog = 0;

    log_msg("Socket-server: Terminal thread started");
    while(p_structCommon->iClientsSockets[l_iCurrentSocketIndex] == 0)
    {
        l_iCurrentSocketIndex--;
        if(l_iCurrentSocketIndex < 0)
        {
            log_msg("Socket-server: Terminal thread closed on error");
            log_warn("Client asking queue is empty %s", " ");
            return 0;
        }
    }
    log_info("Socket-server: Terminal thread use the socket index %d/%d with the value %d", l_iCurrentSocketIndex, MAX_CONNECTED_CLIENTS, p_structCommon->iClientsSockets[l_iCurrentSocketIndex]);

    threadSafeLogBar(p_structCommon, ADD_LINE, "New user joined");
    threadSafeLogBar(p_structCommon, DISPLAY, "");

    while(l_bExit != TRUE)
    {
        l_iReturnedReadWriteValue = read(p_structCommon->iClientsSockets[l_iCurrentSocketIndex], l_cBufferTransmittedData, USER_COMMAND_LENGHT - 1);

        /************************
         *
         * Reception from client
         *
         ************************/
        if(l_iReturnedReadWriteValue > 0)
        {
            if(strstr(l_cBufferTransmittedData, "cli_srv close_con") != NULL)
            {
                threadSafeLogBar(p_structCommon, ADD_LINE, "Server have closed the game.");
                threadSafeLogBar(p_structCommon, DISPLAY, "");

                log_info("Closing socket. Received order :  %s", l_cBufferTransmittedData);
                l_bExit = TRUE;
            }
            else if(strstr(l_cBufferTransmittedData, "r0005") != NULL)
            {
                /* Receive position and action done by user */
                l_iCursorX = atoi(strstr(l_cBufferTransmittedData, "r0005") + strlen("r0005") + 1);
                l_iCursorY = atoi(strstr(l_cBufferTransmittedData, "r0005") + strlen("r0005") + 6);
                p_structCommon->cUserMove = *(strstr(l_cBufferTransmittedData, "r0005") + strlen("r0005") + 11);

                /* If user put a rock, do the same here */
                if(p_structCommon->cUserMove == 'r')
                {
                    /* We have to check two times in order to avoid network issues */
                    if(l_iVerificationIterator == 1)
                    {
                        if(l_iPotentialNewRockX == l_iCursorX && l_iPotentialNewRockY == l_iCursorY)
                        {
                            l_iVerificationIterator++;
                            p_structCommon->cGrid[COLOR_MATRIX][l_iCursorY][l_iCursorX] = p_structCommon->iClientsColor[l_iCurrentSocketIndex];
                            p_structCommon->cGrid[TEXT_MATRIX][l_iCursorY][l_iCursorX] = ' ';
                            drawElement(l_iCursorX + p_structCommon->iOffsetX, l_iCursorY + p_structCommon->iOffsetY,
                                        p_structCommon->cGrid[TEXT_MATRIX][l_iCursorY][l_iCursorX],
                                        p_structCommon->cGrid[COLOR_MATRIX][l_iCursorY][l_iCursorX]);
                            loopCompletion(l_iCursorX, l_iCursorY, p_structCommon->iClientsColor[l_iCurrentSocketIndex], p_structCommon);
                            l_iVerificationIterator++;
                            l_iPotentialNewRockX = -1;
                            l_iPotentialNewRockY = -1;
                        }
                        else
                        {
                            l_iVerificationIterator = 0;
                        }
                        l_iPotentialNewRockX = -1;
                        l_iPotentialNewRockY = -1;
                    }
                    else if(l_iVerificationIterator == 0)
                    {
                        l_iPotentialNewRockX = l_iCursorX;
                        l_iPotentialNewRockY = l_iCursorY;
                        l_iVerificationIterator++;
                    }

                }

                /* Prepare answer by continuing to tell the content of the grid */
                if(p_structCommon->cUserMove == 'Z' || l_iVerificationIterator < 2)
                {
                    snprintf(l_cBufferToSendData, USER_COMMAND_LENGHT, "cli_srv ack0005 %4d %4d %d %c",
                                l_iCursorBrowseringX,
                                l_iCursorBrowseringY,
                                p_structCommon->cGrid[COLOR_MATRIX][l_iCursorBrowseringY][l_iCursorBrowseringX],
                                p_structCommon->cGrid[TEXT_MATRIX][l_iCursorBrowseringY][l_iCursorBrowseringX]);
                }
                else if(p_structCommon->cUserMove == 'r')
                {
                    l_iVerificationIterator = 0;
                    snprintf(l_cBufferToSendData, USER_COMMAND_LENGHT, "cli_srv ack0005 %4d %4d %d %c",
                                l_iCursorX,
                                l_iCursorY,
                                p_structCommon->cGrid[COLOR_MATRIX][l_iCursorY][l_iCursorX],
                                'r');
                }

                write(p_structCommon->iClientsSockets[l_iCurrentSocketIndex],
                      l_cBufferToSendData,
                      strlen(l_cBufferToSendData));
                bzero(l_cBufferToSendData, USER_COMMAND_LENGHT);

                l_iWatchdog = 0;
                do
                {
                    if(++l_iCursorBrowseringX >= p_structCommon->iSizeX)
                    {
                        l_iCursorBrowseringX = 0;
                        if(++l_iCursorBrowseringY >= p_structCommon->iSizeY)
                        {
                            l_iCursorBrowseringY = 0;
                            l_iWatchdog++;
                        }
                    }
                }while(p_structCommon->cGrid[COLOR_MATRIX][l_iCursorBrowseringY][l_iCursorBrowseringX] == enumNoir && l_iWatchdog < 2);
            }
            else if(strstr(l_cBufferTransmittedData, "cli_srv r0000") != NULL)
            {
                l_iClientRequestInit = 1;
            }
            else if(strstr(l_cBufferTransmittedData, "cli_srv ack0001") != NULL)
            {
                l_iClientRequestInit = 2;
            }
            else if(strstr(l_cBufferTransmittedData, "cli_srv ack0002") != NULL)
            {
                l_iClientRequestInit = 3;
            }
            else if(strstr(l_cBufferTransmittedData, "cli_srv ack0003") != NULL)
            {
                l_iClientRequestInit = 4;
            }
            else if(strstr(l_cBufferTransmittedData, "srv_cli msg") != NULL)
            {
                threadSafeLogBar(p_structCommon, ADD_LINE, strstr(l_cBufferTransmittedData, "srv_cli msg ") + strlen("srv_cli msg "));
                threadSafeLogBar(p_structCommon, DISPLAY, "");
            }
            else
            {
                log_info("Socket [%d] Thread index [%d] : Received message from client [%s]", p_structCommon->iClientsSockets[l_iCurrentSocketIndex], l_iCurrentSocketIndex, l_cBufferTransmittedData);
            }
            bzero(l_cBufferTransmittedData, USER_COMMAND_LENGHT);
        }


        switch(l_iClientRequestInit)
        {
            case 0:
                /* Normal state of an intialized remote-host */
                break;
            case 1:
                p_structCommon->iClientsColor[l_iCurrentSocketIndex] = getNextAvailableUserColor(p_structCommon);
                snprintf(l_cBufferToSendData, USER_COMMAND_LENGHT, "cli_srv r0001 %d", p_structCommon->iClientsColor[l_iCurrentSocketIndex]);
                break;
            case 2:
                snprintf(l_cBufferToSendData, USER_COMMAND_LENGHT, "cli_srv r0002 %d", 0);
                break;
            case 3:
                snprintf(l_cBufferToSendData, USER_COMMAND_LENGHT, "cli_srv r0003");
                break;
            case 4:
                l_iClientRequestInit = 0;
                snprintf(l_cBufferToSendData, USER_COMMAND_LENGHT, "cli_srv ack0005 %4d %4d %d %c", 0, 0, 0, ' ');
                break;
            default:
                log_msg("Server: unexpected starting runlevel reached");
                break;
        }


        if(strstr(p_structCommon->sUserCommand, "sendmsg"))
        {
            strcpy(l_cBufferToSendData, "srv_cli msg ");
            strcat(l_cBufferToSendData, strstr(p_structCommon->sUserCommand, "sendmsg ") + strlen("sendmsg "));

            threadSafeLogBar(p_structCommon, ADD_LINE, strstr(l_cBufferToSendData, "srv_cli msg ") + strlen("srv_cli msg "));
            threadSafeLogBar(p_structCommon, DISPLAY, "");

            bzero(p_structCommon->sUserCommand, USER_COMMAND_LENGHT);
        }


        /************************
         *
         *     Exit asked
         *
         ************************/
        if(p_structCommon->bNetworkDisconnectionRequiered == TRUE)
        {
            strncpy(l_cBufferToSendData, "cli_srv close_con", strlen("cli_srv close_con"));
            l_bExit = TRUE;
        }

        /************************
         *
         *      Sending part
         *
         ************************/
        if(strlen(l_cBufferToSendData) > 0)
        {
            l_iReturnedReadWriteValue = write(p_structCommon->iClientsSockets[l_iCurrentSocketIndex],
                                              l_cBufferToSendData, strlen(l_cBufferToSendData));

            if(l_iReturnedReadWriteValue <= 0)
            {
                log_msg("Socket-server: Terminal thread closed on writing error");
                l_bExit = TRUE;
            }
            bzero(l_cBufferToSendData, USER_COMMAND_LENGHT);
        }

        usleep(TIME_BETWEEN_TWO_REQUEST);
    }

    threadSafeLogBar(p_structCommon, ADD_LINE, "User had leaved the game.");
    threadSafeLogBar(p_structCommon, DISPLAY, "");

    log_msg("Socket-server: Terminal thread close normally");
    close(p_structCommon->iClientsSockets[l_iCurrentSocketIndex]);
    p_structCommon->iClientsSockets[l_iCurrentSocketIndex] = 0;
    return 0;
}








/*******************************************
 *
 *             Client part
 *   Connect to the server with one thread
 *
 *******************************************/



/** @brief PUBLIC INTERFACE
  * Start this function in order to have a TCP client, using port TCP_PORT
  * This function starts a thread in charge of ask connection to a server.
  * @param p_structCommon : all usefull data of the program
  * @return -1 if error during starting the waiting thread, 0 if the thread starts correctly. Nothing matter with the TCP / Socket state
  */
int tcpSocketClient(structProgramInfo* p_structCommon)
{
    pthread_t l_structWaitingThreadID;

    pthread_mutex_init(p_structCommon->pthreadMutex, NULL);
    p_structCommon->bMutexInitialized = TRUE;

    if(pthread_create(&l_structWaitingThreadID,NULL, clientConnectionThread, (void*)p_structCommon) < 0)
    {
        pthread_mutex_destroy(p_structCommon->pthreadMutex);
        p_structCommon->bMutexInitialized = FALSE;
        log_err("Could not create the clientConnectionThread %s", " ");
        return -1;
    }

    return 0;
}



/** @brief Connection thread. This thread is on the client side, asking to a server a connection.
  * If there is no server, or a network failure, this thread is closed and soket is cleaned.
  * Normally, this thread send data of the client to the server and receive data from all other users
  * from the server in order to display it.
  * @param p_structCommon : all usefull data of the program
  * @return 0 cause this is a thread. Another system is used to have status sent back
  */
void* clientConnectionThread(void* p_structCommonShared)
{
    structProgramInfo* p_structCommon = (structProgramInfo*)p_structCommonShared;

    int l_iSocketClient;
    int l_iReturnedReadWriteValue;
    struct sockaddr_in l_structServAddr;
    struct hostent* l_structRemoteServer;
    char l_cBufferTransmittedData[USER_COMMAND_LENGHT];
    char l_cBufferToSendData[USER_COMMAND_LENGHT];
    char l_bQuit;
    char l_bHaveToGetStartingInformationsFromServer;
    struct in_addr l_structIpV4Addr;
    struct in6_addr l_structIpV6Addr;
    unsigned int l_iX;
    unsigned int l_iY;
    int l_iOldColor;

    bzero((char *) &l_structServAddr, sizeof(l_structServAddr));
    bzero(l_cBufferTransmittedData, USER_COMMAND_LENGHT);
    l_structServAddr.sin_family = AF_INET;
    l_structServAddr.sin_port = htons(TCP_PORT);
    l_iReturnedReadWriteValue = 0;
    l_bQuit = FALSE;
    l_bHaveToGetStartingInformationsFromServer = TRUE;
    l_iX = 0;
    l_iY = 0;
    l_iOldColor = 0;

    log_msg("Socket-client: Communication thread started");

    l_iSocketClient = socket(AF_INET, SOCK_STREAM, 0);
    if (l_iSocketClient < 0) 
    {
        log_err("Socket-client: socket declaration failed. errno %d", errno);
        log_msg("Socket-client: Communication thread closed on error");
        p_structCommon->bMutexInitialized = FALSE;
        return 0;
    }

    /* have to be replaced by getaddrinfo FIXME */
    if(p_structCommon->bIpV4 == TRUE)
    {
        inet_pton(AF_INET, p_structCommon->sServerAddress, &l_structIpV4Addr);
        l_structRemoteServer = gethostbyaddr(&l_structIpV4Addr, sizeof l_structIpV4Addr, AF_INET);
    }
    else
    {
        inet_pton(AF_INET6, p_structCommon->sServerAddress, &l_structIpV6Addr);
        l_structRemoteServer = gethostbyaddr(&l_structIpV6Addr, sizeof l_structIpV6Addr, AF_INET6);
    }


    if (l_structRemoteServer == NULL)
    {
        log_err("Socket-client: Host doen't exist. errno %d", errno);
        log_msg("Socket-client: Communication thread closed on error");
        p_structCommon->bMutexInitialized = FALSE;
        close(l_iSocketClient);
        return 0;
    }

    bcopy((char *)l_structRemoteServer->h_addr, (char *)&l_structServAddr.sin_addr.s_addr, l_structRemoteServer->h_length);
    if (connect(l_iSocketClient,(struct sockaddr *) &l_structServAddr,sizeof(l_structServAddr)) < 0)
    {
        log_err("Socket-client: connection to the server failed. errno %d", errno);
        log_msg("Socket-client: Communication thread closed on error");
        p_structCommon->bMutexInitialized = FALSE;
        close(l_iSocketClient);
        return 0;
    }

    threadSafeLogBar(p_structCommon, ADD_LINE, "Online game joined !");
    threadSafeLogBar(p_structCommon, DISPLAY, "");

    /* First start init client informations */
    if(l_bHaveToGetStartingInformationsFromServer == TRUE)
    {
        strcpy(l_cBufferToSendData, "cli_srv r0000");
        if(write(l_iSocketClient, l_cBufferToSendData, strlen(l_cBufferToSendData)) < 0)
        {
            /* Fail at writing, leave the thread */
            l_bQuit = TRUE;
        }
        bzero(l_cBufferToSendData, USER_COMMAND_LENGHT);
    }


    while(l_bQuit != TRUE)
    {
        /************************
         *
         *     Receiving part
         *
         ************************/
        l_iReturnedReadWriteValue = read(l_iSocketClient, l_cBufferTransmittedData, USER_COMMAND_LENGHT);
        if(l_iReturnedReadWriteValue > 0)
        {
            if(strstr(l_cBufferTransmittedData, "cli_srv close_con") != NULL)
            {
                log_info("Closing socket. Received order :  %s", l_cBufferTransmittedData);
                l_bQuit = TRUE;
            }
            else if(strstr(l_cBufferTransmittedData, "r0001") != NULL)
            {
                p_structCommon->iCurrentUserColor = atoi(strstr(l_cBufferTransmittedData, "r0001") + strlen("r0001"));
                log_info("New Color %d", p_structCommon->iCurrentUserColor);
                strcpy(l_cBufferToSendData, "cli_srv ack0001");
            }
            else if(strstr(l_cBufferTransmittedData, "r0002") != NULL)
            {
                p_structCommon->iCurrentUserNumber = atoi(strstr(l_cBufferTransmittedData, "r0002") + strlen("r0002"));
                log_info("New ID %d", p_structCommon->iCurrentUserNumber);
                strcpy(l_cBufferToSendData, "cli_srv ack0002");
            }
            else if(strstr(l_cBufferTransmittedData, "r0003") != NULL)
            {
                p_structCommon->bAbleToRestartGame = TRUE;
                strcpy(l_cBufferToSendData, "cli_srv ack0003");
            }
            else if(strstr(l_cBufferTransmittedData, "ack0005") != NULL)
            {
                l_iX = atoi(strstr(l_cBufferTransmittedData, "ack0005") + strlen("ack0005") + 1);
                l_iY = atoi(strstr(l_cBufferTransmittedData, "ack0005") + strlen("ack0005") + 6);


                /* If we _have_ to receive an ack for a move we have sent - verify it is OK and the good value is returned */
                if(p_structCommon->cUserMove == 'r')
                {
                    if(l_iX == p_structCommon->iLastXUsed && l_iY == p_structCommon->iLastYUsed)
                    {
                        if(*(strstr(l_cBufferTransmittedData, "ack0005") + strlen("ack0005") + 13) == 'r')
                        {
                            p_structCommon->cUserMove = 0;
                        }
                    }
                    /* In order to avoid execution of the next block of code */
                    l_iX = p_structCommon->iSizeX;
                }

                if(l_iX < p_structCommon->iSizeX && l_iY < p_structCommon->iSizeY)
                {
                    l_iOldColor = p_structCommon->cGrid[COLOR_MATRIX][l_iY][l_iX];
                    p_structCommon->cGrid[COLOR_MATRIX][l_iY][l_iX] = atoi(strstr(l_cBufferTransmittedData, "ack0005") + strlen("ack0005") + 11);
                    p_structCommon->cGrid[TEXT_MATRIX][l_iY][l_iX] = *(strstr(l_cBufferTransmittedData, "ack0005") + strlen("ack0005") + 13);
                    if(l_iOldColor != p_structCommon->cGrid[COLOR_MATRIX][l_iY][l_iX])
                    {
                        drawElement(l_iX + p_structCommon->iOffsetX, l_iY + p_structCommon->iOffsetY,
                                    p_structCommon->cGrid[TEXT_MATRIX][l_iY][l_iX],
                                    p_structCommon->cGrid[COLOR_MATRIX][l_iY][l_iX]);
                    }
                }

                snprintf(l_cBufferToSendData, USER_COMMAND_LENGHT, "cli_srv r0005 %4d %4d %c", p_structCommon->iLastXUsed, p_structCommon->iLastYUsed,
                    (p_structCommon->cUserMove == 'r') ? p_structCommon->cUserMove : 'Z');
            }
            else if(strstr(l_cBufferTransmittedData, "srv_cli msg") != NULL)
            {
                threadSafeLogBar(p_structCommon, ADD_LINE, strstr(l_cBufferTransmittedData, "srv_cli msg ") + strlen("srv_cli msg "));
                threadSafeLogBar(p_structCommon, DISPLAY, "");
            }
            else
            {
                log_info("Received message from server [%s]", l_cBufferTransmittedData);
            }
            bzero(l_cBufferTransmittedData, USER_COMMAND_LENGHT);
        }




        if(strstr(p_structCommon->sUserCommand, "sendmsg") != NULL)
        {
            strcpy(l_cBufferToSendData, "srv_cli msg ");
            strcat(l_cBufferToSendData, strstr(p_structCommon->sUserCommand, "sendmsg ") + strlen("sendmsg "));

            threadSafeLogBar(p_structCommon, ADD_LINE, strstr(l_cBufferToSendData, "srv_cli msg ") + strlen("srv_cli msg "));
            threadSafeLogBar(p_structCommon, DISPLAY, "");

            bzero(p_structCommon->sUserCommand, USER_COMMAND_LENGHT);
        }

         /************************
         *
         *       Exit part
         *
         ************************/
        if(p_structCommon->bNetworkDisconnectionRequiered == TRUE)
        {
            log_msg("Socket-client: Program ask to close connection");
            strncpy(l_cBufferToSendData, "cli_srv close_con", strlen("cli_srv close_con"));
            write(l_iSocketClient,l_cBufferToSendData,strlen(l_cBufferToSendData));
            l_bQuit = TRUE;
        }
         /************************
         *
         *      Sending part
         *
         ************************/
        else if(strlen(l_cBufferToSendData) > 0)
        {
            l_iReturnedReadWriteValue = write(l_iSocketClient, l_cBufferToSendData, strlen(l_cBufferToSendData));

            if(l_iReturnedReadWriteValue == 0 )
            {
                log_err("Soket-client reading function failed %s", " ");
                log_msg("Socket-client: Communication thread closed on error");
                p_structCommon->bMutexInitialized = FALSE;
                close(l_iSocketClient);
                return 0;
            }

            /* Clean the request to avoid loop-sending */
            bzero(p_structCommon->sUserCommand, USER_COMMAND_LENGHT);
            bzero(l_cBufferToSendData, USER_COMMAND_LENGHT);
        }

        usleep(TIME_BETWEEN_TWO_REQUEST + 10);
    }

    threadSafeLogBar(p_structCommon, ADD_LINE, "Game leaved");
    threadSafeLogBar(p_structCommon, DISPLAY, "");

    log_msg("Socket-client: Communication thread closed normally");
    close(l_iSocketClient);

    /* Down the flag to say to game.c that this is OK, socket is closed */
    p_structCommon->bNetworkDisconnectionRequiered = FALSE;
    return 0;
}
