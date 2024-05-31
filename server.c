#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>





int main() {
    int server_fd, client_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    int opt = 1;

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    // Binding the socket to the network address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listening for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port 8080\n");

    // Accept an incoming connection
    if ((client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    printf("Connection accepted\n");

    // Read data from the client
    int valread = read(client_fd, buffer, 1024);
    printf("Received %d bytes\n", valread);

    // Print TCP segment data
    struct tcphdr *tcp_header = (struct tcphdr *)(buffer);

    printf("Source Port: %d\n", ntohs(tcp_header->source));
    printf("Destination Port: %d\n", ntohs(tcp_header->dest));
    printf("Sequence Number: %u\n", ntohl(tcp_header->seq));
    printf("Acknowledgment Number: %u\n", ntohl(tcp_header->ack_seq));
    printf("Data Offset: %d\n", tcp_header->doff * 4);
    printf("Flags: 0x%x\n", tcp_header->fin | tcp_header->syn | tcp_header->rst | tcp_header->psh | tcp_header->ack | tcp_header->urg);
    printf("Window Size: %d\n", ntohs(tcp_header->window));
    printf("Checksum: 0x%x\n", ntohs(tcp_header->check));
    printf("Urgent Pointer: %d\n", ntohs(tcp_header->urg_ptr));

    // Log the payload data
    char *data = buffer + (tcp_header->doff * 4);
    int data_length = valread - (tcp_header->doff * 4);
    printf("Data: ");
    for (int i = 0; i < data_length; i++) {
        printf("%02x ", (unsigned char)data[i]);
    }
    printf("\n");

    // Close the connection
    close(client_fd);
    close(server_fd);

    return 0;
}
