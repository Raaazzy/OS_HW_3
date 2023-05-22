#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define VISITORS 10
#define CLIENTS 50
#define PICTURES 5

typedef struct {
    int id;
    int visitors;
} Picture;

void ClientHandle(int);

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <ip> <port>\n", argv[0]);
        return 1;
    }
    const char* ip = argv[1];
    int port = atoi(argv[2]);
    int socketClient, ServerSocket;
    struct sockaddr_in server_address, client_address;
    int active_clients = 0;
    // Creating the socket
    ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (ServerSocket == -1) {
        perror("Error creating socket");
        exit(1);
    }
    // Stuct for adress
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(ip);
    // Binding a socket to an address
    if (bind(ServerSocket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Error creating socket");
        exit(1);
    }
    // Waiting for incoming connections
    if (listen(ServerSocket, CLIENTS) == -1) {
        perror("Error while listening on a socket");
        exit(1);
    }
    printf("Server started\n");
    while (1) {
        // Accepting a client connection
        socketClient = accept(ServerSocket, (struct sockaddr*)&client_address, &sizeof(client_address));
        if (socketClient == -1) {
            perror("Error accepting connection");
            exit(1);
        }
        printf("New connection accepted\n");
        if (active_clients >= CLIENTS) {
            printf("Visitor limit exceeded\n");
            close(socketClient);
            continue;
        }
        // Create a new process to process a client
        pid_t pid = fork();
        if (pid == -1) {
            perror("Error creating process");
            exit(1);
        } else if (pid == 0) {
            // child process
            close(ServerSocket);
            // Handling customer requests
            ClientHandle(socketClient);
            exit(0);
        } else {
            // parent process
            close(socketClient);
            active_clients++;
        }
    }
    close(ServerSocket);
    return 0;
}

void ClientHandle(int socket) {
    // query processing
    Picture pictures[PICTURES];
    for (int i = 0; i < PICTURES; i++) {
        pictures[i].id = i + 1;
        pictures[i].visitors = 0;
    }
    while (1) {
        // Reading data from the client
        char buf[1024];
        int receivedBytes = recv(socket, buf, sizeof(buf), 0);
        if (receivedBytes > 0) {
            buf[receivedBytes] = '\0';
            printf("Received data from the client: %s\n", buf);
            // Processing a request from a client
            if (strcmp(buf, "Request to view pictures") == 0) {
                // Random selection of picture
                int randPicture = rand() % PICTURES;
                // Checking the number of visitors to an image
                if (pictures[randPicture].visitors >= VISITORS) {
                    // The number of visitors exceeds the limit
                    char response[] = "The number of visitors exceeds the limit";
                    send(socket, response, sizeof(response), 0);
                    printf("The number of visitors exceeds the limit");
                    continue;
                }
                pictures[randPicture].visitors++;
                char response[256];
                sprintf(response, "Picture %d: %d customers", pictures[randPicture].id, pictures[randPicture].visitors);
                printf("Sending data to the client: Picture %d: %d customers", pictures[randPicture].id, pictures[randPicture].visitors);
                send(socket, response, sizeof(response), 0);
            } else if (strcmp(buf, "Request to leave the gallery") == 0) {
                // Request to leave the gallery
                char response[] = "Exit gallery";
                send(socket, response, sizeof(response), 0);
                printf("Exit gallery");
                break;
            }
        }
    }
    close(socket);
}
