#include <sys/socket.h>
#include <stdio.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>

volatile sig_atomic_t exitSig = 0;

void sig_handler(int signal){
    (void)signal;
    exitSig = 1;
}

int main(int argc, char *argv[]){

    int runDaemon = 0;
    int file;
    char buf[1024];
    ssize_t cnt;
    struct sigaction sa;
    struct addrinfo hints;
    struct addrinfo *res;

    // Signal action
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sig_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if(sigaction(SIGINT, &sa, NULL) == -1){
        exit(EXIT_FAILURE);
    }
    if(sigaction(SIGTERM, &sa, NULL) == -1){
        exit(EXIT_FAILURE);
    }
    
    // daemon flag
    if(argc == 2 && strcmp(argv[1], "-d") == 0){
        runDaemon = 1;
    }
    openlog(NULL,0,LOG_USER);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if(getaddrinfo(NULL, "9000", &hints, &res) != 0){
        freeaddrinfo(res);
        return(-1);
    }

    int socket_server = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if(socket_server == -1){
        freeaddrinfo(res);
        return(-1);
    }

    int tmp = 1;
    setsockopt(socket_server, SOL_SOCKET, SO_REUSEADDR, &tmp, sizeof(tmp));

    if(bind(socket_server, res->ai_addr, res->ai_addrlen) == -1){
        freeaddrinfo(res);
        return(-1);
    }

    freeaddrinfo(res);

    if(listen(socket_server, 10) == -1){
        close(socket_server);
        return(-1);
    }

    if(runDaemon){
        pid_t pid = fork();
        if(pid < 0){
            exit(EXIT_FAILURE);
        }else if (pid > 0){
            exit(EXIT_SUCCESS);
        }

        if(setsid() == -1){
            exit(EXIT_FAILURE);
        }
        chdir("/");
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
    }

    while(!exitSig){
        struct sockaddr_in client;
        socklen_t client_len = sizeof(client);

        int socket_client = accept(socket_server, (struct sockaddr *)&client, &client_len);
        if(socket_client == -1){
            if(exitSig){  // signal exit!!!!!
                break;
            }
        }

        // log ip
        char ipClient[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client.sin_addr, ipClient, sizeof(ipClient));        
        syslog(LOG_INFO, "Accepted connection from %s\r\n", ipClient);

        // Open file and write packets
        file = open("/var/tmp/aesdsocketdata", O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (file == -1) {
            syslog(LOG_ERR, "Error: Could not create/open the file.\n");
            close(file);
            close(socket_client); 
        }

        while((cnt = recv(socket_client, buf, sizeof(buf), 0)) > 0){
            write(file, buf, cnt);
            
            // packets delimited by newline
            if(memchr(buf, '\n', cnt)){
                break;
            }
        }
        close(file);

        // loopback file to client
        file = open("/var/tmp/aesdsocketdata", O_RDONLY);
        while((cnt = read(file, buf, sizeof(buf))) > 0){
            send(socket_client, buf, cnt, 0);
        }
        close(file); 

        // connection cleanup   
        syslog(LOG_INFO, "Close connection from %s\r\n", ipClient);
        close(socket_client);
    } //end while(!exitSig)

    // clean up
    close(socket_server);
    remove("/var/tmp/aesdsocketdata");
    closelog();

    return(0);
}
