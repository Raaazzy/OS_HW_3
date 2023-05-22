#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void showInfo(const char *info, const char *answ);

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <ip> <port>\n", argv[0]);
        return 1;
    }
    const char* ip = argv[1];
    int port = atoi(argv[2]);
    // Socket creation
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Error creating socket");
        exit(1);
    }
    // Structure for server address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(ip);
    // Establishing a connection to the server
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("Error establishing a connection to the server");
        exit(EXIT_FAILURE);
    }
    printf("Server connection established\n");
    while (1) {
        // Receiving a request from a client
        char request[256];
        int receivedBytes = recv(client_socket, request, sizeof(request), 0);
        if (receivedBytes > 0) {
            request[receivedBytes] = '\0';
            printf("Received a request from a client\n");
            send(client_socket, request, sizeof(request), 0);
            // Getting a response from the server
            char response[256];
            receivedBytes = recv(client_socket, response, sizeof(response), 0);
            if (receivedBytes > 0) {
                response[receivedBytes] = '\0';
                printf("Received a response from the server\n");
                // Redirecting the response to the client
                send(client_socket, response, sizeof(response), 0);
                showInfo("Request from client: ", request);
                showInfo("Response from the server: ", response);
            }
        }
    }
    close(client_socket);
    return 0;
}

void showInfo(const char *info, const char *answ) {
    // Displaying information about the exchange process
    printf("%s%s\n", info, ans);
}
