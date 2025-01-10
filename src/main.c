// Client side C program to demonstrate Socket
// programming
#include <arpa/inet.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <time.h>
#include <getopt.h>
#include <sys/time.h>

#define DEFAULT_NUMBER_OF_PORTS 100;
#define MAXIMUM_THREADS 100
//if a tcp connection can't be established in 10 seconds we got problems
#define THREAD_TIMEOUT 10
int g_port_on;
int port = 100;
int port_low = 1;
char ip_addr[32];
bool g_debugInfo = false;

void createSocket(int*, struct sockaddr_in*);

int g_openPorts[65535];
int g_openPortsIndex = 0;
int g_activeThreads = 0;
pthread_mutex_t openPortsLock; 
pthread_mutex_t threadActiveLock;

//Thread entry function
void *testPort(void *arg){
    int client_fd;
    struct sockaddr_in serv_addr;

    struct timeval timeout;
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;

    // Keep the thread pool alive!
    while(1){
        pthread_mutex_lock(&threadActiveLock);
        int test_port = g_port_on;
        g_port_on ++;
        pthread_mutex_unlock(&threadActiveLock);
        // If the port on to test is greater than the highest port to check then kill the thread
        if(test_port > port){
            // printf("exiting the thread pool");
            return NULL;
        }
        if(g_debugInfo){
            printf("tested port %d\n", test_port);
        }
        // Create local vars
    
        //Call create socket function to recieve a file descriptior to access
        createSocket(&client_fd, &serv_addr);
        serv_addr.sin_port = htons(test_port);
        setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout);
        //Attempt connection to server on port
        if(connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == 0){
            if(g_debugInfo){
                printf("Port %d is open\n", test_port);
            }
            //memory saftey! make sure thread is the only one accessing shared resources
            pthread_mutex_lock(&openPortsLock);
            //add to list of open ports
            g_openPorts[g_openPortsIndex] = test_port;
            g_openPortsIndex ++;
            //unlock mutex
            pthread_mutex_unlock(&openPortsLock); 
            //terminate connection to server
            close(client_fd);
        }else if(g_debugInfo){
            printf("CLOSED: %d\n", test_port);
            //make sure we close our file descriptors
            close(client_fd);
        }
    
    }
    //all done :)
    printf("exiting the thread pool");
    return NULL;
}

int main(int argc, char *argv[])
{
    int i;
    time_t start, end;
    // Argument testing vars
    bool customHighPort = false;
    bool unknowOption = false;
    bool validIPAddress = true;
    bool validPortLow = true;
    bool validPortHigh = true;
    //Process arguments 
    int opt;
    while((opt = getopt(argc, argv, ":p:P:a:d")) != -1)  
    {  
        switch(opt)  
        {  
            case 'p':
            if(atoi(optarg)==0){
                validPortLow = false;
                break;
            }
            port_low = atoi(optarg);
            break;

            case 'P':
            if(atoi(optarg)==0){
                validPortHigh = false;
                break;
            }
            port = atoi(optarg);
            customHighPort = true;
            break;

            case 'a':
                int test_ip;
                if((inet_pton(AF_INET, optarg, &test_ip)== 0)){
                    validIPAddress = false;
                    printf("IP Entered: %s\n", optarg);
                    break;
                }else{
                    strcpy(ip_addr, optarg);
                    break;
                }  
            case 'd':
                g_debugInfo = true;
                break;
            case ':':  
                printf("Option Missing a Value\n");
                return 1;
                break; 

            case '?':  
                printf("unknown option: -%c\n", optopt);
                unknowOption = true;
                break;  
        }  
    }

    if(!(validPortLow&&validPortHigh)){
        printf("ERROR: Enter a valid TCP port\n");
        return -1;
    }

    if(!validIPAddress){
        printf("ERROR: Invalid IP address\n");
        return -1;
    }

    if(unknowOption){
        return 1;
    }

    if(!customHighPort){
        //make sure the high port is always higher than the low port
        port = port_low + DEFAULT_NUMBER_OF_PORTS;
    }else if(port_low>port){
        printf("ERROR: High port must be greater than starting port\n");
        return 1;
    }
    
    
    //Create mutexes
    if ((pthread_mutex_init(&threadActiveLock, NULL) !=0) && (pthread_mutex_init(&openPortsLock, NULL) != 0)) { 
        printf("\n mutex inits have failed\n"); 
        return 1; 
    }else if(g_debugInfo){
        printf("mutexes created\n");
    }
    
    //Lets'a go!
    printf("Scanning ports %d to %d on host %s\n",port_low, port, ip_addr);

    //start timer
    time(&start);
    
    g_port_on = port_low;
    pthread_t threads[MAXIMUM_THREADS];
    // Create thread pool
    for(i=0; i<MAXIMUM_THREADS; i++){
       if(pthread_create(&(threads[i]), NULL, testPort, NULL)!=0){
                printf("Cannot create thread\n");
        }
    }
    
    //Wait for all threads to terminate    
    for(i = 0; i<MAXIMUM_THREADS; i++){
        pthread_join(threads[i], NULL);
    }

    //end timer
    time(&end);
    
    long time_taken = (long)(end - start); 
    //print out time taken - perhaps verbose only feature
    printf("Mapped %d ports in %f seconds\n", port-port_low, time_taken); 
    
    //print out list of open ports
    for(i=0; i<g_openPortsIndex; i++){
        printf("Port %d is open\n", g_openPorts[i]);
    }
    //destory locks
    pthread_mutex_destroy(&openPortsLock); 
    pthread_mutex_destroy(&threadActiveLock);
    return 0;
}

void createSocket(int* fd, struct sockaddr_in* addr){

    *fd = socket(AF_INET, SOCK_STREAM, 0);
    if(g_debugInfo){
        // printf("Value of fd: %d\n",*fd);
    }

    if (*fd < 0) {
        printf("Socket creation error \n");
        printf("Errno %d\n", errno);
        return;
    }else{
        if(g_debugInfo){
            // printf("created socket\n");
        }
    }

    addr->sin_family = AF_INET;
      
    if (inet_pton(AF_INET, ip_addr, &addr->sin_addr)<= 0) {
        printf(
            "\nInvalid address/ Address not supported \n");
        return;
    }else{
        if(g_debugInfo){
            // printf("Target address %s\n", ip_addr);
        }
    }
    return;
}