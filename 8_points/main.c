#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define CLIENTS 10

void shopInfo(const char *info, const char *answ);
void *handle(void *clietnSocket);

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <ip> <port>\n", argv[0]);
        return 1;
    }
    const char *ip = argv[1];
    int port = atoi(argv[2]);
    // Socket creation
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating socket");
        exit(1);
    }
    // Structure for server address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(ip);
    // Binding a socket to a server address
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("Error binding socket to server address");
        exit(EXIT_FAILURE);
    }
    if (listen(server_socket, CLIENTS) == -1) {
        perror("Error binding socket to server address");
        exit(EXIT_FAILURE);
    }
    printf("Server started\n");
    while (1) {
        // Accepting an incoming connection
        struct sockaddr_in client_address;
        int client_socket = accept(server_socket, (struct sockaddr *)&client_address, sizeof(client_address));
        if (client_socket == -1) {
            perror("Error accepting connection");
            continue;
        }
        printf("New connection accepted\n");
        // Creating a separate thread for client processing
        pthread_t client_thread;
        int *clientSocket = malloc(sizeof(int));
        *clientSocket = client_socket;
        if (pthread_create(&client_thread, NULL, handle, (void *) clientSocket) != 0) {
            perror("Error creating thread for client processing");
            close(client_socket);
            free(clientSocket);
        }
    }
    close(server_socket);
    return 0;
}

void shopInfo(const char *info, const char *answ) {
    // Displaying information about the exchange process
    printf("%s%s\n", info, ans);
}

void *handle(void *clietnSocket) {
    int client_socket = *(int *)clietnSocket;
    char request[256];
    int receivedBytes;
    while ((receivedBytes = recv(client_socket, request, sizeof(request), 0)) > 0) {
        request[receivedBytes] = '\0';
        printf("Received a request from a client: %s\n", request);
        send(client_socket, request, receivedBytes, 0);
        // Getting a response from the server
        char response[256];
        receivedBytes = recv(client_socket, response, sizeof(response), 0);
        if (receivedBytes > 0) {
            response[receivedBytes] = '\0';
            printf("Received a response from the server: %s\n", response);
            // Redirecting the response to the client
            send(client_socket, response, receivedBytes, 0);
            // Displaying information about the exchange process
            shopInfo("Request from client: ", request);
            shopInfo("Response from the server: ", response);
        }
    }
    close(client_socket);
    free(clietnSocket);
    pthread_exit(NULL);
}
