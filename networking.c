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
  * | read data from server (send back ack0005, execute commands etc...)
  * | ack0005 replace pong - two kind of usages. If this client put a rock, this is a ack with the 'r' code inside and the X, Y received by server -for confirmation-
  * | and on the other cases, it is a point of the server grid to put in the client grid -synchro function-
  * | for the first case, the message is like ack0005 XXXX YYYY   r  // r is the ASCII character 'r'
  * | for the second one, the message is like ack0005 XXXX YYYY C T  // C an integer, the color. T a character, tu put inside the text matrix. If this is a new info, put it on screen
  * | in the first case, we send back cli_srv   r0005 XXXX YYYY A    // A is 'r' if the last action of the user was to put a rock -and if this action was not confirmed yet-
  * | or a 'Z' for all the other cases. 'Z' means for the server "send me another synchro data in order to have the same screen as yours". If we receive 'r' with coordinates of
  * | the last rock put on grid and the last action of the user is to put a rock, we can confirm that the server have received a good information. So reset the 'last action registered'.
  * |
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
  *   | | | | receive data from socket, analyse it and execute it.
  *   | | | | 
  *   | | | | synchro mechanism - r0005 / ack0005
  *   | | | | r0005 received : data is like r0005 XXXX YYYY A
  *   | | | | if A == 'r' means that the client have put a rock. We store the position and we put the verification iterator to 1. If the verification iterator equals 1,
  *   | | | |  that means we have already receivedi this position with the same request. So, put the rock in the grid, and put the iterator to 2 in order to say to
  *   | | | |  the sender part "give the client an ack0005"
  *   | | | | if A == 'Z' or if the iterator isn't great enought to say "we have to acknowledge the position received" we send back to the client a rock of the server's grid 
  *   | | | | in order to make it synchronized with us. We send only colored block, in order to send only usefull informations. This have to change in order to be faster and smarter
  *   | | | | -And after, find the next colored block, store coordinates and continue the analyse of the receive data
  *   | | | | This section have an independant write() call it is inherited from the old ping-pong system, and makes the connection always active to send data
  *   | | | | 
  *   | | | | send init info to the client, when ack0003 -the last one- is received send back an empty ack0005 in order to make the new ping-pong system works and keep the connection working
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

    /* Introducing functions have to start the mutex system. It is the same for the client side */
    pthread_mutex_init(p_structCommon->pthreadMutex, NULL);
    p_structCommon->bMutexInitialized = TRUE;

    /* start the thread and leave it alone. This system make us able to get back the hand with a listening server just behind */
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


    /* Wait for new clients, when there is one new connection, this function accept it and a new thread is started to handle this client 
     * All clients are synchronized with the common structure. And in order to display log, theses functions uses mutex  */
    while((l_iSocketNewConnection = accept(l_iSocket, (struct sockaddr *) &l_structClientAddr, &l_structClientLen)))
    {
        log_msg("Socket-server: Waiting thread received a client co request. Start a new thread...");
        log_info("Socket-server: Starting thread have the index %d/%d and the Socket is %d", l_iSocketCounter + 1, MAX_CONNECTED_CLIENTS, l_iSocketNewConnection);

        /* New connection is asked. Try to start a new thread - If this is the first client, put 4.This is a special code,
            because i've done bad work on this feature, to say it is still the server's turn to play. When the server have drop its rock,
            we put 2 in this value and inform the client that it is its turn to play */
        p_structCommon->bWhoHaveToPlay[l_iSocketCounter] = (l_iSocketCounter == 0) ? 4 : 1;
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

        /* The thread was started, but we check if we can have a another new connection or if it was the last one.
         * In this case, this listening thread will end now and let all the client-handler threads do their jobs */
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

    /* end of the listening thread, server is not able to add new clients */
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
    char l_cClientAction;
    char l_bDuckServiceMessageAnswerWaiting;
    int l_iReturnedReadWriteValue;
    int l_iCurrentSocketIndex;
    int l_iClientRequestInit;
    int l_iCursorX;
    int l_iCursorY;
    int l_iPotentialNewRockX;
    int l_iPotentialNewRockY;
    int l_iVerificationIterator;
    int l_iWatchdog;
    unsigned int l_iLastUserRequestID;
    unsigned int l_iCursorBrowseringX;
    unsigned int l_iCursorBrowseringY;
    double l_dWhenWeHaveToSendAnswer;
    struct timeval l_structTimeNow;

    l_bExit = FALSE;
    l_bDuckServiceMessageAnswerWaiting = FALSE;
    l_dWhenWeHaveToSendAnswer= 0;
    l_cClientAction = 0;
    l_iLastUserRequestID = 0;
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

    /* Find our index in the socket table. The last socket entered have the greatest index in the table */
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

    /* Main loop for handling the connection */
    while(l_bExit != TRUE)
    {
        /* We read, we analyse, then we write through the socket - read() is a blocking function, so the ack0005 / r0005 commands are here to stimulate the connection */
        l_iReturnedReadWriteValue = read(p_structCommon->iClientsSockets[l_iCurrentSocketIndex], l_cBufferTransmittedData, USER_COMMAND_LENGHT - 1);

        /************************
         *
         * Reception from client
         *
         ************************/
        if(l_iReturnedReadWriteValue > 0)
        {
            /* User on the client side ask to quit, we have to terminate this thread */
            if(strstr(l_cBufferTransmittedData, "cli_srv close_con") != NULL)
            {
                threadSafeLogBar(p_structCommon, ADD_LINE, "Server have closed the game.");
                threadSafeLogBar(p_structCommon, DISPLAY, "");

                log_info("Closing socket. Received order :  %s", l_cBufferTransmittedData);
                l_bExit = TRUE;
            }
            /* the new ping / pong system. Described in the intro help of this document */
            else if(strstr(l_cBufferTransmittedData, "r0005") != NULL)
            {
                /* Receive position and action done by user */
                l_iCursorX = atoi(strstr(l_cBufferTransmittedData, "r0005") + strlen("r0005") + 1);
                l_iCursorY = atoi(strstr(l_cBufferTransmittedData, "r0005") + strlen("r0005") + 6);
                l_cClientAction = *(strstr(l_cBufferTransmittedData, "r0005") + strlen("r0005") + 11);

                /* If user put a rock, do the same here */
                if(l_cClientAction == 'r')
                {
                    /* We have to check two times in order to avoid network issues */
                    if(l_iVerificationIterator == 1)
                    {
                        /* Put the second check in first to avoid incrementation issues. Checks position is the same than the previous one, and the action too */
                        if(l_iPotentialNewRockX == l_iCursorX && l_iPotentialNewRockY == l_iCursorY)
                        {
                            /* Put l_iVerificationIterator to 2 in order to say to the writing part to send an ack of this rock order. It uses a specific pattern */
                            l_iVerificationIterator++;

                            /* Put the rock on the server's grid - now this information is going to be scanned by other threads and sent to all clients */
                            p_structCommon->cGrid[COLOR_MATRIX][l_iCursorY][l_iCursorX] = p_structCommon->iClientsColor[l_iCurrentSocketIndex];
                            p_structCommon->cGrid[TEXT_MATRIX][l_iCursorY][l_iCursorX] = ' ';
                            drawElement(l_iCursorX + p_structCommon->iOffsetX, l_iCursorY + p_structCommon->iOffsetY,
                                        p_structCommon->cGrid[TEXT_MATRIX][l_iCursorY][l_iCursorX],
                                        p_structCommon->cGrid[COLOR_MATRIX][l_iCursorY][l_iCursorX]);

                            /* Registration of the new point. Have to be synchcronized quicly with clients */
                            p_structCommon->cGrid[SYNC_MATRIX][l_iCursorY][l_iCursorX] = POINT_TO_SYNC;

                            /* Check if the client have not completed a loop. We compute loop here and now. After, we fill the area and send data to clients */
                            loopCompletion(l_iCursorX, l_iCursorY, p_structCommon->iClientsColor[l_iCurrentSocketIndex], p_structCommon);

                            /* This player have done its turn. Change it. Give new players's turn */
                            p_structCommon->bWhoHaveToPlay[l_iCurrentSocketIndex] = 1;
                            p_structCommon->bWhoHaveToPlay[ (p_structCommon->bWhoHaveToPlay[l_iCurrentSocketIndex + 1] == 1) ? l_iCurrentSocketIndex + 1 : 0 ] = 2;

                            if(p_structCommon->bWhoHaveToPlay[0] == 2)
                            {
                                p_structCommon->bMyTurnToPlay = TRUE;
                                p_structCommon->bWhoHaveToPlay[0] = 4;
                            }
                            else if(p_structCommon->bWhoHaveToPlay[0] == 4 && p_structCommon->bMyTurnToPlay == FALSE)
                            {
                                p_structCommon->bWhoHaveToPlay[0] = 2;
                            }
                        }
                        /* Second verification failed */
                        else
                        {
                            l_iVerificationIterator = 0;
                        }

                        /* Whatever, reset these values at the end of this code */
                        l_iPotentialNewRockX = -1;
                        l_iPotentialNewRockY = -1;
                    }
                    /* If the client puts a rock, record the position. It is the first call. If the client re-send just after this message the same position,
                     * we are going to put it on the grid. If this is another message, we delete this information */
                    else if(l_iVerificationIterator == 0)
                    {
                        l_iPotentialNewRockX = l_iCursorX;
                        l_iPotentialNewRockY = l_iCursorY;
                        l_iVerificationIterator++;
                    }

                }

                /* Prepare answer by continuing to tell the content of the grid */
                if(l_cClientAction == 'Z' || l_iVerificationIterator < 2)
                {
                    snprintf(l_cBufferToSendData, USER_COMMAND_LENGHT, "cli_srv ack0005 %4d %4d %d %c",
                                l_iCursorBrowseringX,
                                l_iCursorBrowseringY,
                                p_structCommon->cGrid[COLOR_MATRIX][l_iCursorBrowseringY][l_iCursorBrowseringX],
                                p_structCommon->cGrid[TEXT_MATRIX][l_iCursorBrowseringY][l_iCursorBrowseringX]);
                    if(p_structCommon->cGrid[SYNC_MATRIX][l_iCursorBrowseringY][l_iCursorBrowseringX] >= POINT_TO_SYNC)
                    {
                        p_structCommon->cGrid[SYNC_MATRIX][l_iCursorBrowseringY][l_iCursorBrowseringX]++;
                    }
                    if(p_structCommon->cGrid[SYNC_MATRIX][l_iCursorBrowseringY][l_iCursorBrowseringX] > POINT_TO_SYNC + 2 * MAX_CONNECTED_CLIENTS)
                    {
                        p_structCommon->cGrid[SYNC_MATRIX][l_iCursorBrowseringY][l_iCursorBrowseringX] = POINT_EMPTY;
                    }
                }
                /* If we are here it is because the distant move of client was 'r' means "drop a rock", and we have passed the verifications
                 * (two identical request to the server). So we can send an aknowlegement to the client to say "stop sending me this order, i know it" */
                else if(l_cClientAction == 'r')
                {
                    l_iVerificationIterator = 0;
                    snprintf(l_cBufferToSendData, USER_COMMAND_LENGHT, "cli_srv ack0005 %4d %4d %d %c",
                                l_iCursorX,
                                l_iCursorY,
                                p_structCommon->cGrid[COLOR_MATRIX][l_iCursorY][l_iCursorX],
                                'r');
                }

                /* Dedicated write function inherited from the old ping-pong system in order to make this connection always awake */
                write(p_structCommon->iClientsSockets[l_iCurrentSocketIndex],
                      l_cBufferToSendData,
                      strlen(l_cBufferToSendData));
                bzero(l_cBufferToSendData, USER_COMMAND_LENGHT);

                /* Find the next block -case with color- on the screen to do an optimized system to send to client only interesting data. Used only when we receive a 'Z' */
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
                }while((p_structCommon->cGrid[SYNC_MATRIX][l_iCursorBrowseringY][l_iCursorBrowseringX] < POINT_TO_SYNC && l_iWatchdog < 2) ||
                       (p_structCommon->cGrid[COLOR_MATRIX][l_iCursorBrowseringY][l_iCursorBrowseringX] == enumNoir && l_iWatchdog == 2));
            }
            /* Initialization commands, in order to send to client all informations it needs */
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
            /* Messaging function */
            else if(strstr(l_cBufferTransmittedData, "srv_cli msg") != NULL)// && strlen(l_cBufferToSendData) != 0)
            {
                threadSafeLogBar(p_structCommon, ADD_LINE, strstr(l_cBufferTransmittedData, "srv_cli msg ") + strlen("srv_cli msg "));
                threadSafeLogBar(p_structCommon, DISPLAY, "");

                /* Prepare server to resend message to all other users, and give it a new user command ID  */
                snprintf(p_structCommon->sUserCommand, USER_COMMAND_LENGHT, "resendtoall %s", strstr(l_cBufferTransmittedData, "srv_cli msg ") + strlen("srv_cli msg "));
                p_structCommon->iLastUserRequestID++;

                /* Empty answer to avoid stopping ping-pong and then block read() of client */
                snprintf(l_cBufferToSendData, USER_COMMAND_LENGHT, "cli_srv ack0005 %4d %4d %d %c", 0, 0, enumNoir, ' ');

                if(strstr(l_cBufferTransmittedData, "\\_o<") != NULL)
                {
                    gettimeofday(&l_structTimeNow, NULL);
                    if(l_bDuckServiceMessageAnswerWaiting == FALSE)
                    {
                        l_bDuckServiceMessageAnswerWaiting = TRUE;
                        l_dWhenWeHaveToSendAnswer = l_structTimeNow.tv_sec * 1000000 +
                                                    l_structTimeNow.tv_usec +
                                                    (p_structCommon->iCurrentUserColor + rand() % 200) * TIME_BETWEEN_TWO_REQUEST;
                    }
                }
            }
            else if(strstr(l_cBufferTransmittedData, "cli_srv ack0006") != NULL)
            {
                p_structCommon->bWhoHaveToPlay[l_iCurrentSocketIndex] = 3;
                p_structCommon->cUserMove = 0;

                /* Empty answer to avoid stopping ping-pong and then block read() of client */
                snprintf(l_cBufferToSendData, USER_COMMAND_LENGHT, "cli_srv ack0005 %4d %4d %d %c", 0, 0, enumNoir, ' ');
            }
            /* Unknown messages */
            else
            {
                log_info("Socket [%d] Thread index [%d] : Received message from client [%s]", p_structCommon->iClientsSockets[l_iCurrentSocketIndex], l_iCurrentSocketIndex, l_cBufferTransmittedData);
            }
            bzero(l_cBufferTransmittedData, USER_COMMAND_LENGHT);
        }


        /* When we have to initialize the client. All the request code are here. At the end, put 0
           We don't have to protect this part against l_cBufferToSendData overwriting because it is the initialisation part */
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
            /* At the end of the starting, initialize new ping-pong system to keep connection alive. send an empty ack0005 request to have a r0005 replied etc... */
            case 4:
                l_iClientRequestInit = 0;
                snprintf(l_cBufferToSendData, USER_COMMAND_LENGHT, "cli_srv ack0005 %4d %4d %d %c", 0, 0, enumNoir, ' ');
                break;
            default:
                log_msg("Server: unexpected starting runlevel reached");
                break;
        }
        /* If server drops a rocks, end its turn */
        if(p_structCommon->cUserMove == 'r')
        {
            p_structCommon->bWhoHaveToPlay[0] = 2;
            p_structCommon->bMyTurnToPlay = FALSE;
        }

        if(l_iClientRequestInit == 0 && 
           strlen(l_cBufferToSendData) < 1 &&
           p_structCommon->bWhoHaveToPlay[l_iCurrentSocketIndex] == 2)
        {
            /* Send message to the user it is its turn to play */
            strcpy(l_cBufferToSendData, "cli_srv r0006");
        }


        if(l_bDuckServiceMessageAnswerWaiting == TRUE && strstr(p_structCommon->sUserCommand, "sendmsg") == NULL && strstr(p_structCommon->sUserCommand, "resendtoall") == NULL)
        {
            gettimeofday(&l_structTimeNow, NULL); 
            if(l_structTimeNow.tv_sec * 1000000 + l_structTimeNow.tv_usec > l_dWhenWeHaveToSendAnswer)
            {
                l_dWhenWeHaveToSendAnswer = 0;
                l_bDuckServiceMessageAnswerWaiting = FALSE;

                /* Send a dead duck */
                snprintf(p_structCommon->sUserCommand, USER_COMMAND_LENGHT, "resendtoall PAN ! \\_x<");
                p_structCommon->iLastUserRequestID++;
            }
        }


        /* If user wants to send a message, prepare the request */
        if(strstr(p_structCommon->sUserCommand, "sendmsg") && strlen(l_cBufferToSendData) == 0 && l_iLastUserRequestID < p_structCommon->iLastUserRequestID)
        {
            snprintf(l_cBufferToSendData,
                            USER_COMMAND_LENGHT,
                            "srv_cli msg ##%d%s##%d %s",
                            p_structCommon->iCurrentUserColor + 20,
                            p_structCommon->sUserName,
                            20,
                            strstr(p_structCommon->sUserCommand, "sendmsg ") + strlen("sendmsg "));

            /* local l_iLastUserRequestID update */
            l_iLastUserRequestID = p_structCommon->iLastUserRequestID;
        }

        if(strstr(p_structCommon->sUserCommand, "resendtoall ") && strlen(l_cBufferToSendData) == 0 && l_iLastUserRequestID < p_structCommon->iLastUserRequestID)
        {
            snprintf(l_cBufferToSendData,
                            USER_COMMAND_LENGHT,
                            "srv_cli msg %s",
                            strstr(p_structCommon->sUserCommand, "resendtoall ") + strlen("resendtoall "));

            /* local l_iLastUserRequestID update */
            l_iLastUserRequestID = p_structCommon->iLastUserRequestID;
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
                log_msg("Socket-server: Terminal thread writing error");
            }
            bzero(l_cBufferToSendData, USER_COMMAND_LENGHT);
        }

        usleep(TIME_BETWEEN_TWO_REQUEST);
    }

    /* For any reason, the client-handling thread is out, so close the connection, send a little message and leave it */
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

    /* Introducing functions have to start the mutex system. It is the same for the server side */
    pthread_mutex_init(p_structCommon->pthreadMutex, NULL);
    p_structCommon->bMutexInitialized = TRUE;

    /* start the thread and leave it alone. This system make us able to get back the hand with a listening client just behind */
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
    char l_bDuckServiceMessageAnswerWaiting;
    char l_cBufferTransmittedData[USER_COMMAND_LENGHT];
    char l_cBufferToSendData[USER_COMMAND_LENGHT];
    char l_bQuit;
    char l_bHaveToGetStartingInformationsFromServer;
    struct in_addr l_structIpV4Addr;
    struct in6_addr l_structIpV6Addr;
    unsigned int l_iX;
    unsigned int l_iY;
    int l_iOldColor;
    double l_dWhenWeHaveToSendAnswer;
    struct timeval l_structTimeNow;

    bzero((char *) &l_structServAddr, sizeof(l_structServAddr));
    bzero(l_cBufferTransmittedData, USER_COMMAND_LENGHT);
    l_bDuckServiceMessageAnswerWaiting = FALSE;
    l_dWhenWeHaveToSendAnswer= 0;
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

    /* Connection part, real one ! If it success, it is ok, on the other hand, leave this thread and return a fail message */
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

        /* We read, we analyse, then we write through the socket - read() is a blocking function, so the ack0005 / r0005 commands are here to stimulate the connection */
        l_iReturnedReadWriteValue = read(l_iSocketClient, l_cBufferTransmittedData, USER_COMMAND_LENGHT);
        if(l_iReturnedReadWriteValue > 0)
        {
            /* User on the server side ask to quit, we have to terminate this thread */
            if(strstr(l_cBufferTransmittedData, "cli_srv close_con") != NULL)
            {
                log_info("Closing socket. Received order :  %s", l_cBufferTransmittedData);
                l_bQuit = TRUE;
            }
            /* all the init request, to grab usefull data like color, number etc... */
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
            /* the new ping-pong system to have an connection alive all the time */
            else if(strstr(l_cBufferTransmittedData, "r0006") != NULL)
            {
                strcpy(l_cBufferToSendData, "cli_srv ack0006");
                p_structCommon->bMyTurnToPlay = TRUE;
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
                            /* If this messages is the expected ack, down the flag in order to say that we expect nothing more from server */
                            p_structCommon->cUserMove = 0;
                        }
                    }
                }
                /* In this case, we just update the grid with server's information provided by 0005 cmd - and update the displayed grid */
                else if(l_iX < p_structCommon->iSizeX && l_iY < p_structCommon->iSizeY)
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

                /* Send a request to ask 1/ 'r' case : an ack to confirm that the server have understood we have put a rock at (iLastXUsed, iLastYUsed) coord.
                                         2/ 'Z' case : an ack to send back another informations from its grid because we have no new-rock to confirm */
                snprintf(l_cBufferToSendData, USER_COMMAND_LENGHT, "cli_srv r0005 %4d %4d %c", p_structCommon->iLastXUsed, p_structCommon->iLastYUsed,
                    (p_structCommon->cUserMove == 'r') ? p_structCommon->cUserMove : 'Z');
            }
            /* Messaging function, we display message we just received */
            else if(strstr(l_cBufferTransmittedData, "srv_cli msg") != NULL)
            {
                threadSafeLogBar(p_structCommon, ADD_LINE, strstr(l_cBufferTransmittedData, "srv_cli msg ") + strlen("srv_cli msg "));
                threadSafeLogBar(p_structCommon, DISPLAY, "");

                /* Special case of a Discovery message*/
                if(strstr(l_cBufferTransmittedData, "\\_o<") != NULL || l_bDuckServiceMessageAnswerWaiting == TRUE)
                {
                    gettimeofday(&l_structTimeNow, NULL);

                    if(l_bDuckServiceMessageAnswerWaiting == FALSE)
                    {
                        l_bDuckServiceMessageAnswerWaiting = TRUE;
                        l_dWhenWeHaveToSendAnswer = l_structTimeNow.tv_sec * 1000000 +
                                                    l_structTimeNow.tv_usec +
                                                    (p_structCommon->iCurrentUserColor + rand() % 200) * TIME_BETWEEN_TWO_REQUEST;
                    }
                }

                /* Empty answer to avoid stoping ping-pong and then block read() of the server */
                snprintf(l_cBufferToSendData, USER_COMMAND_LENGHT, "cli_srv r0005 %4d %4d %c", 0, enumNoir, 'Z');
            }
            else             /* some UFO */
            {
                log_info("Received message from server [%s]", l_cBufferTransmittedData);

                /* Empty answer to avoid stoping ping-pong and then block read() of the server */
                snprintf(l_cBufferToSendData, USER_COMMAND_LENGHT, "cli_srv r0005 %4d %4d %c", 0, enumNoir, 'Z');
            }
            bzero(l_cBufferTransmittedData, USER_COMMAND_LENGHT);
        }


        if(l_bDuckServiceMessageAnswerWaiting == TRUE && strstr(p_structCommon->sUserCommand, "sendmsg") == NULL)
        {
            gettimeofday(&l_structTimeNow, NULL);
            if(l_structTimeNow.tv_sec * 1000000 + l_structTimeNow.tv_usec > l_dWhenWeHaveToSendAnswer)
            {
                l_dWhenWeHaveToSendAnswer = 0;
                l_bDuckServiceMessageAnswerWaiting = FALSE;

                /* Send a dead duck */
                snprintf(p_structCommon->sUserCommand, USER_COMMAND_LENGHT, "sendmsg PAN ! \\_x<");
                p_structCommon->iLastUserRequestID++;
            }
        }



        /* If we want to send a message to the server (an so to evrybody */
        if(strstr(p_structCommon->sUserCommand, "sendmsg") != NULL)
        {
            snprintf(l_cBufferToSendData,
                            USER_COMMAND_LENGHT,
                            "srv_cli msg ##%d%s##%d %s",
                            p_structCommon->iCurrentUserColor + 20,
                            p_structCommon->sUserName,
                            20,
                            strstr(p_structCommon->sUserCommand, "sendmsg ") + strlen("sendmsg "));

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
