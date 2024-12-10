#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include "list.h"

#define PORT 9001
#define ACK "ACK: "

list_t *mylist;

// Signal handler to gracefully terminate
void handle_sigint(int sig) {
    printf("\nTerminating server...\n");
    list_free(mylist); // Free the linked list memory
    exit(0);
}

int main(int argc, char const *argv[]) {
    int n, val, idx;

    // Create server socket
    int servSockD = socket(AF_INET, SOCK_STREAM, 0);

    // Buffers for receiving and sending data
    char buf[1024];
    char sbuf[1024];
    char *token;

    // Define server address
    struct sockaddr_in servAddr;

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(PORT);
    servAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket to the specified IP and port
    bind(servSockD, (struct sockaddr *)&servAddr, sizeof(servAddr));

    // Listen for connections
    listen(servSockD, 1);

    // Accept a client connection
    int clientSocket = accept(servSockD, NULL, NULL);

    // Initialize the linked list
    mylist = list_alloc();

    // Register the signal handler for graceful termination
    signal(SIGINT, handle_sigint);

    while (1) {
        // Receive messages from client
        n = recv(clientSocket, buf, sizeof(buf), 0);
        buf[n] = '\0';

        if (n > 0) {
            token = strtok(buf, " ");

            if (strcmp(token, "exit") == 0) {
                list_free(mylist);
                sprintf(sbuf, "Server shutting down...");
                send(clientSocket, sbuf, sizeof(sbuf), 0);
                break;
            } else if (strcmp(token, "get_length") == 0) {
                val = list_length(mylist);
                sprintf(sbuf, "Length = %d", val);
            } else if (strcmp(token, "add_back") == 0) {
                token = strtok(NULL, " ");
                val = atoi(token);
                list_add_to_back(mylist, val);
                sprintf(sbuf, "%s%d", ACK, val);
            } else if (strcmp(token, "add_front") == 0) {
                token = strtok(NULL, " ");
                val = atoi(token);
                list_add_to_front(mylist, val);
                sprintf(sbuf, "%s%d", ACK, val);
            } else if (strcmp(token, "add_position") == 0) {
                token = strtok(NULL, " ");
                idx = atoi(token);
                token = strtok(NULL, " ");
                val = atoi(token);
                list_add_at_index(mylist, val, idx);
                sprintf(sbuf, "%s%d at index %d", ACK, val, idx);
            } else if (strcmp(token, "remove_back") == 0) {
                val = list_remove_from_back(mylist);
                sprintf(sbuf, "Removed = %d", val);
            } else if (strcmp(token, "remove_front") == 0) {
                val = list_remove_from_front(mylist);
                sprintf(sbuf, "Removed = %d", val);
            } else if (strcmp(token, "remove_position") == 0) {
                token = strtok(NULL, " ");
                idx = atoi(token);
                val = list_remove_at_index(mylist, idx);
                sprintf(sbuf, "Removed = %d at index %d", val, idx);
            } else if (strcmp(token, "get") == 0) {
                token = strtok(NULL, " ");
                idx = atoi(token);
                val = list_get_elem_at(mylist, idx);
                sprintf(sbuf, "Value at index %d = %d", idx, val);
            } else if (strcmp(token, "print") == 0) {
                sprintf(sbuf, "%s", listToString(mylist));
            } else {
                sprintf(sbuf, "Unknown command");
            }

            // Send response to client
            send(clientSocket, sbuf, sizeof(sbuf), 0);
        }

        memset(buf, '\0', 1024);
    }

    close(clientSocket);
    close(servSockD);

    return 0;
}
