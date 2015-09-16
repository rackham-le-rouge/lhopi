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


int tcpSocketServer(structProgramInfo* p_structCommon)
{
    int l_iSocket;
    int l_iSocketNewConnection;
    int l_iSocketCounter;
    socklen_t l_structClientLen;
    char l_cBufferTransmittedData[256];
    struct sockaddr_in l_structServAddr;
    struct sockaddr_in l_structClientAddr;
    pthread_t l_structThreadID;

    bzero((char *) &l_structServAddr, sizeof(l_structServAddr));
    bzero(l_cBufferTransmittedData,256);

    l_iSocketCounter = 0;
    l_structServAddr.sin_family = AF_INET;
    l_structServAddr.sin_addr.s_addr = INADDR_ANY;                                        
    l_structServAddr.sin_port = htons(TCP_PORT);

    l_iSocket = socket(AF_INET, SOCK_STREAM, 0); 

    if (l_iSocket < 0)
    { 
        log_err("Socket-server: error opening socket. err %d", errno);
        return errno;
    }

    if (bind(l_iSocket, (struct sockaddr *) &l_structServAddr, sizeof(l_structServAddr)) < 0)
    {
        log_err("Socket-server: error binding port. errno %d", errno);
        return errno;
    }

    if(listen(l_iSocket,5) < 0)
    {
        log_err("Socket-server: listen failed. errno %d", errno);
        return errno;
    }

    l_structClientLen = sizeof(l_structClientAddr);

    /* Add all clients */
    while((l_iSocketNewConnection = accept(l_iSocket, (struct sockaddr *) &l_structClientAddr, &l_structClientLen)))
    {
        p_structCommon->iClientsSockets[l_iSocketCounter++] = l_iSocketNewConnection;
        if(pthread_create( &l_structThreadID , NULL ,  tcpSocketServerConnectionHander , (void*) p_structCommon) < 0)
        {
            log_err("Could not create the thread %s", " ");
            return errno;
        }
    }

    /* If failed to accept a connection */
    if (l_iSocketNewConnection < 0)
    {
        log_err("Socket-server: Connection requested by peer, but failed to establish. Retrieved socket is empty. errno %d", errno);
        return errno;
    }

    close(l_iSocket);
    return 0;
}



void* tcpSocketServerConnectionHander(void* p_structCommonShared)
{
    structProgramInfo* p_structCommon = (structProgramInfo*)p_structCommonShared;
    char l_cBufferTransmittedData[USER_COMMAND_LENGHT];
    int l_iReturnedReadWriteValue;
    int l_iCurrentSocketIndex;

    l_iCurrentSocketIndex = 0;

    while(p_structCommon->iClientsSockets[l_iCurrentSocketIndex] > 0)
    {
        l_iCurrentSocketIndex++;
        if(l_iCurrentSocketIndex == MAX_CONNECTED_CLIENTS)
        {
            log_warn("Client asking queue is full %s", " ");
            l_iCurrentSocketIndex--;
            break;
        }
    }

    l_iReturnedReadWriteValue = read(p_structCommon->iClientsSockets[l_iCurrentSocketIndex], l_cBufferTransmittedData, USER_COMMAND_LENGHT - 1);

    l_iReturnedReadWriteValue = write(p_structCommon->iClientsSockets[l_iCurrentSocketIndex], "Test", 4);

    if(l_iReturnedReadWriteValue == 0)
    {
        log_err("Soket writing function failed %s", " ");
    }

    close(p_structCommon->iClientsSockets[l_iCurrentSocketIndex]);
        
    return 0;
}

int tcpSocketClient(structProgramInfo* p_structCommon, char p_bIPV4)
{
    int l_iSocketClient;
    struct sockaddr_in l_structServAddr;
    struct hostent* l_structRemoteServer;
    char l_cBufferTransmittedData[USER_COMMAND_LENGHT];
    struct in_addr l_structIpV4Addr;
    struct in6_addr l_structIpV6Addr;

    bzero((char *) &l_structServAddr, sizeof(l_structServAddr));
    bzero(l_cBufferTransmittedData,USER_COMMAND_LENGHT);
    l_structServAddr.sin_family = AF_INET;
    l_structServAddr.sin_port = htons(TCP_PORT);


    l_iSocketClient = socket(AF_INET, SOCK_STREAM, 0);
    if (l_iSocketClient < 0) 
    {
        log_err("Socket-client: socket declaration failed. errno %d", errno);
        return errno;
    }

    /* l_structRemoteServer = gethostbyname(p_structCommon->sServerAddress); // no ipv6 */
    /* have to be replaced by getaddrinfo FIXME */
    if(p_bIPV4 == TRUE)
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
        return errno;
    }

    bcopy((char *)l_structRemoteServer->h_addr, (char *)&l_structServAddr.sin_addr.s_addr, l_structRemoteServer->h_length);
    if (connect(l_iSocketClient,(struct sockaddr *) &l_structServAddr,sizeof(l_structServAddr)) < 0)
    {
        log_err("Socket-client: connection to the server failed. errno %d", errno);
        return errno;
    }

    /* Have to be integrated in the right function */

    /*
    l_iReturnedReadWriteValue = write(l_iSocketClient,l_cBufferTransmittedData,strlen(l_cBufferTransmittedData));
    l_iReturnedReadWriteValue = read(l_iSocketClient,l_cBufferTransmittedData,255);

    close(l_iSocketClient);
    */


    return 0;
}
