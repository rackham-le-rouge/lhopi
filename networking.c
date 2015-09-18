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





/*******************************************
 *
 *           Thread Safe log part
 *   Because access to the graphic methods
 *   needs to be thread safe. Add mutexes.
 *
 *******************************************/

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
  *
  */
int tcpSocketServer(structProgramInfo* p_structCommon)
{
    pthread_t l_structWaitingThreadID;

    pthread_mutex_init(p_structCommon->pthreadMutex, NULL);
    p_structCommon->bMutexInitialized = TRUE;

    if(pthread_create(&l_structWaitingThreadID,NULL, waitingForNewConnectionsThread, (void*)p_structCommon) < 0)
    {
        log_err("Could not create the WaitingForNewConnectionsThread %s", " ");
        return -1;
    }

    return 0;
}


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
        return 0;
    }

    if (bind(l_iSocket, (struct sockaddr *) &l_structServAddr, sizeof(l_structServAddr)) < 0)
    {
        log_err("Socket-server: error binding port. errno %d", errno);
        log_msg("Socket-server: Waiting thread closed on error");
        close(l_iSocket);
        return 0;
    }

    if(listen(l_iSocket, MAX_CONNECTED_CLIENTS) < 0)
    {
        log_err("Socket-server: listen failed. errno %d", errno);
        log_msg("Socket-server: Waiting thread closed on error");
        close(l_iSocket);
        return 0;
    }

    l_structClientLen = sizeof(l_structClientAddr);

    logBar(p_structCommon, ADD_LINE, "Waiting for incomming connections !");
    logBar(p_structCommon, DISPLAY, "");


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
            close(l_iSocket);
            return 0;
        }

        if(l_iSocketCounter >= MAX_CONNECTED_CLIENTS)
        {
            log_err("Socket-server: can't have more client connected %s", " ");

            logBar(p_structCommon, ADD_LINE, "Max network users reached. Refusing connections.");
            logBar(p_structCommon, DISPLAY, "");

            log_msg("Socket-server: Waiting thread closed on error");
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
        return 0;
    }

    log_msg("Socket-server: Waiting thread closed normally");
    close(l_iSocket);
    return 0;
}


void* tcpSocketServerConnectionHander(void* p_structCommonShared)
{
    structProgramInfo* p_structCommon = (structProgramInfo*)p_structCommonShared;

    char l_cBufferTransmittedData[USER_COMMAND_LENGHT];
    char l_cBufferoToSendData[USER_COMMAND_LENGHT];
    char l_bExit;
    int l_iReturnedReadWriteValue;
    int l_iCurrentSocketIndex;

    l_bExit = FALSE;
    l_iCurrentSocketIndex = MAX_CONNECTED_CLIENTS - 1;

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

    logBar(p_structCommon, ADD_LINE, "New user joined");
    logBar(p_structCommon, DISPLAY, "");

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
                log_info("Closing socket. Received order :  %s", l_cBufferTransmittedData);
                l_bExit = TRUE;
            }
            else
            {
                log_info("Received message from client [%s]", l_cBufferTransmittedData);
            }
            bzero(l_cBufferTransmittedData, USER_COMMAND_LENGHT);
        }



        /************************
         *
         *     Exit asked
         *
         ************************/
        if(p_structCommon->bNetworkDisconnectionRequiered == TRUE)
        {
            strncpy(l_cBufferoToSendData, "cli_srv close_con", strlen("cli_srv close_con"));
            l_bExit = TRUE;
        }

         /************************
         *
         *      Sending part
         *
         ************************/
        if(strlen(l_cBufferoToSendData) > 0)
        {
            l_iReturnedReadWriteValue = write(p_structCommon->iClientsSockets[l_iCurrentSocketIndex],
                                              l_cBufferoToSendData, strlen(l_cBufferoToSendData));

            if(l_iReturnedReadWriteValue <= 0)
            {
                log_msg("Socket-server: Terminal thread closed on writing error");
                l_bExit = TRUE;
            }
            bzero(l_cBufferoToSendData, USER_COMMAND_LENGHT);
        }

        write(p_structCommon->iClientsSockets[l_iCurrentSocketIndex], "ping", strlen("ping"));
        usleep(TIME_BETWEEN_TWO_REQUEST);
    }

    log_msg("Socket-server: Terminal thread close normally");
    close(p_structCommon->iClientsSockets[l_iCurrentSocketIndex]);
    p_structCommon->iClientsSockets[l_iCurrentSocketIndex] = -1;
    return 0;
}








/*******************************************
 *
 *             Client part
 *   Connect to the server with one thread
 *
 *******************************************/



/** @brief PUBLIC INTERFACE
  *
  */
int tcpSocketClient(structProgramInfo* p_structCommon)
{
    pthread_t l_structWaitingThreadID;

    pthread_mutex_init(p_structCommon->pthreadMutex, NULL);
    p_structCommon->bMutexInitialized = TRUE;

    if(pthread_create(&l_structWaitingThreadID,NULL, clientConnectionThread, (void*)p_structCommon) < 0)
    {
        log_err("Could not create the clientConnectionThread %s", " ");
        return -1;
    }

    return 0;
}



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
    struct in_addr l_structIpV4Addr;
    struct in6_addr l_structIpV6Addr;

    bzero((char *) &l_structServAddr, sizeof(l_structServAddr));
    bzero(l_cBufferTransmittedData, USER_COMMAND_LENGHT);
    l_structServAddr.sin_family = AF_INET;
    l_structServAddr.sin_port = htons(TCP_PORT);
    l_iReturnedReadWriteValue = 0;
    l_bQuit = FALSE;

    log_msg("Socket-client: Communication thread started");

    l_iSocketClient = socket(AF_INET, SOCK_STREAM, 0);
    if (l_iSocketClient < 0) 
    {
        log_err("Socket-client: socket declaration failed. errno %d", errno);
        log_msg("Socket-client: Communication thread closed on error");
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
        close(l_iSocketClient);
        return 0;
    }

    bcopy((char *)l_structRemoteServer->h_addr, (char *)&l_structServAddr.sin_addr.s_addr, l_structRemoteServer->h_length);
    if (connect(l_iSocketClient,(struct sockaddr *) &l_structServAddr,sizeof(l_structServAddr)) < 0)
    {
        log_err("Socket-client: connection to the server failed. errno %d", errno);
        log_msg("Socket-client: Communication thread closed on error");
        close(l_iSocketClient);
        return 0;
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
            else
            {
                log_info("Received message from server [%s]", l_cBufferTransmittedData);
            }
            bzero(l_cBufferTransmittedData, USER_COMMAND_LENGHT);
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
        else if(strlen(p_structCommon->sUserCommand) > 0)
        {
            strncpy(l_cBufferToSendData, p_structCommon->sUserCommand, USER_COMMAND_LENGHT);
            l_iReturnedReadWriteValue = write(l_iSocketClient, l_cBufferToSendData, strlen(l_cBufferToSendData));

            if(l_iReturnedReadWriteValue == 0 )
            {
                log_err("Soket-client reading function failed %s", " ");
                log_msg("Socket-client: Communication thread closed on error");
                close(l_iSocketClient);
                return 0;
            }

            /* Clean the request to avoid loop-sending */
            bzero(p_structCommon->sUserCommand, USER_COMMAND_LENGHT);
        }

        usleep(TIME_BETWEEN_TWO_REQUEST + 10);
        write(l_iSocketClient, "ping", strlen("ping"));
    }

    log_msg("Socket-client: Communication thread closed normally");
    close(l_iSocketClient);

    /* Down the flag to say to game.c that this is OK, socket is closed */
    p_structCommon->bNetworkDisconnectionRequiered = FALSE;
    return 0;
}
