#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Function to search student details
void find_student(char *reg_no, char *response)
{
    FILE *fp;
    char file_reg[50];
    char name[50];
    char branch[50];
    char college[100];

    int found = 0;

    // Open student database file
    fp = fopen("students.txt", "r");

    if (fp == NULL)
    {
        strcpy(response, "Error opening file");
        return;
    }

    // Read records one by one
    while (fscanf(fp, "%s %s %s %s",
                  file_reg, name, branch, college) != EOF)
    {
        // Compare registration number
        if (strcmp(file_reg, reg_no) == 0)
        {
            sprintf(response,
                    "Name: %s, Branch: %s, College: %s",
                    name, branch, college);

            found = 1;
            break;
        }
    }

    // If registration number is not found
    if (!found)
    {
        strcpy(response, "Data not found");
    }

    fclose(fp);
}

int main()
{
    int server_fd;
    int new_socket;

    struct sockaddr_in address;
    int addrlen = sizeof(address);

    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE];

    // Create TCP socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    printf("Server socket created successfully.\n");

    // Configure server address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket to port
    if (bind(server_fd,
             (struct sockaddr *)&address,
             sizeof(address)) < 0)
    {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server bound to port %d.\n", PORT);

    // Listen for client connection
    if (listen(server_fd, 5) < 0)
    {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server is waiting for client...\n");

    // Accept client connection
    new_socket = accept(server_fd,
                        (struct sockaddr *)&address,
                        (socklen_t *)&addrlen);

    if (new_socket < 0)
    {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Client connected successfully.\n");

    // Continuous communication
    while (1)
    {
        memset(buffer, 0, BUFFER_SIZE);
        memset(response, 0, BUFFER_SIZE);

        // Receive registration number
        int bytes_received = read(new_socket,
                                  buffer,
                                  BUFFER_SIZE - 1);

        if (bytes_received <= 0)
        {
            printf("Client disconnected.\n");
            break;
        }

        buffer[bytes_received] = '\0';

        printf("Client Query: %s\n", buffer);

        // Check exit condition
        if (strcmp(buffer, "bye") == 0)
        {
            printf("Client requested to terminate connection.\n");
            break;
        }

        // Search student record
        find_student(buffer, response);

        // Send response to client
        send(new_socket,
             response,
             strlen(response),
             0);
    }

    // Close sockets
    close(new_socket);
    close(server_fd);

    printf("Server connection closed.\n");

    return 0;
}
