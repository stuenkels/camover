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
#define DEFAULT_NUMBER_OF_PORTS 100;
#define MAXIMUM_THREADS 5000
//if a tcp connection can't be established in 10 seconds we got other problems...
#define THREAD_TIMEOUT 5 
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
    long test_port = (long) arg;
    if(g_debugInfo){
        printf("tested port %d\n", test_port);
    }
    // Create local vars
    int client_fd;
    struct sockaddr_in serv_addr;
    //Call create socket function to recieve a file descriptior to access
    createSocket(&client_fd, &serv_addr);
    serv_addr.sin_port = htons(test_port);
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
        printf("CLOSED: %d\n", port);
        //make sure we close our file descriptors
        close(client_fd);
    }
    //all done :)
    pthread_mutex_lock(&threadActiveLock);
    g_activeThreads --;
    pthread_mutex_unlock(&threadActiveLock);
    return NULL;
}

int main(int argc, char *argv[])
{
    time_t start, end;
    
    //Process arguments 
    int i;
    bool customHighPort = false;
    for (i = 2; i < argc; i++){
        if(strcmp(argv[i],"-d")==0){
            printf("running in debug mode\n");
            g_debugInfo = true;
        }
        if(strcmp(argv[i],"-p")==0){
            if(atoi(argv[i+1])==0){
                printf("ERROR: Enter a valid TCP port\n");
                return 1;
            }
            port_low = atoi(argv[i+1]);
        }
        if(strcmp(argv[i],"-P")==0){
            if(atoi(argv[i+1])==0){
                printf("ERROR: Enter a valid TCP port\n");
                return 1;
            }
            port = atoi(argv[i+1]);
            customHighPort = true;
        }  

    }
    if(!customHighPort){
        //make sure the high port is always higher than the low port
        port = port_low + DEFAULT_NUMBER_OF_PORTS;
    }else if(port_low>port){
        printf("ERROR: High port must be greater than starting port\n");
        return 1;
    }
    strcpy(ip_addr, argv[1]);
    
    printf("Scanning ports %d to %d on host %s\n",port_low, port, ip_addr);


    //Create mutexes
    if ((pthread_mutex_init(&threadActiveLock, NULL) !=0) && (pthread_mutex_init(&openPortsLock, NULL) != 0)) { 
        printf("\n mutex inits have failed\n"); 
        return 1; 
    }else if(g_debugInfo){
        printf("mutexes created\n");
    }
    
    //start timer
    time(&start);
    int portOn = port_low;
    pthread_t threads[MAXIMUM_THREADS];
    int thread_ids[MAXIMUM_THREADS];
    long ports_to_be_tested[65535];
    //got at least this far
    pthread_t thread_id;
    while(portOn<port){
        pthread_mutex_lock(&threadActiveLock);
        if(g_activeThreads<MAXIMUM_THREADS){
            ports_to_be_tested[portOn] = portOn;
            pthread_create(&thread_id, NULL, testPort, (void *) ports_to_be_tested[portOn]);
            g_activeThreads ++;
            pthread_mutex_unlock(&threadActiveLock);
            portOn ++;
            

        }else{
            pthread_mutex_unlock(&threadActiveLock);
        }
        fflush(stdout);
        if(g_debugInfo){
            printf("Progress: %d/%d\n", portOn, port);
        }
    }
    //Wait for all threads to die out
    time_t spinlockStart, spinlockEnd;
    double elapsedSpinlockTime;
    time(&spinlockStart);
    if(g_debugInfo){
        printf("Waiting for threads to finish\n");
    }
    while(g_activeThreads != 0){
        time(&spinlockEnd);
        elapsedSpinlockTime = (double)(spinlockEnd - spinlockStart);
        if(elapsedSpinlockTime>=THREAD_TIMEOUT){
            if(g_debugInfo){
                printf("Last thread(s) timed out\n");
            }
            break;
        }

    }

    //end timer
    time(&end);
    
    double time_taken = (double)(end - start); 
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
        printf("Value of fd: %d\n",*fd);
    }

    if (*fd < 0) {
        printf("Socket creation error \n");
        printf("Errno %d\n", errno);
        return;
    }else{
        if(g_debugInfo){
            printf("created socket\n");
        }
    }

    addr->sin_family = AF_INET;
       
    if (inet_pton(AF_INET, ip_addr, &addr->sin_addr)
        <= 0) {
        printf(
            "\nInvalid address/ Address not supported \n");
        return;
    }else{
        if(g_debugInfo){
            printf("Target address %s\n", ip_addr);
        }
    }
    return;
}