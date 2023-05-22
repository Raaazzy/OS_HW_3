#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <ip> <port>\n", argv[0]);
        return 1;
    }
    const char* ip = argv[1];
    int port = atoi(argv[2]);
    // Creating the socket
    int socketClient = socket(AF_INET, SOCK_STREAM, 0);
    if (socketClient == -1) {
        perror("Error socket creating");
        exit(1);
    }
    // Stuct for adress
    struct ServerAdress server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(ip);
    // Received a response from the server
    if (connect(socketClient, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("Error with server connection");
        exit(EXIT_FAILURE);
    }
    printf("Connection with server established\n");
    while (1) {
        char request[] = "Reqeust on wathing on pictures";
        send(socketClient, request, sizeof(request), 0);
        // Getting a response from the server
        char response[256];
        int receivedBytes = recv(socketClient, response, sizeof(response), 0);
        if (receivedBytes > 0) {
            response[receivedBytes] = '\0';
            printf("Received a response from the server: %s\n", response);
            // Server response processing
            if (strncmp(response, "Too many visitors", strlen("Too many visitors")) == 0) {
                // Waiting answer from the server
                printf("Waiting answer from the server: %s\n", response);
                sleep(1);
                continue;
            } else if (strncmp(response, "Exit from the gallery", strlen("Exit from the gallery")) == 0) {
                // Exit from the gallery
                printf("Received a response from the server: %s\n", response);
                break;
            }
        }
    }
    char exit_request[] = "Gallery exit request";
    send(socketClient, exit_request, sizeof(exit_request), 0);
    // Closing the client socket
    close(socketClient);
    return 0;
}
