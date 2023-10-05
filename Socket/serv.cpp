#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " 4023" << std::endl;
        exit(1);
    }

    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[1024];

    struct sockaddr_in serv_addr, cli_addr;
    
    portno = atoi(argv[1]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    listen(sockfd, 5);

    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0)
        error("ERROR on accept");

    // Receive the file name from the client
    bzero(buffer, sizeof(buffer));
    int n = read(newsockfd, buffer, sizeof(buffer) - 1);
    if (n < 0)
        error("ERROR reading file name from socket");

    std::string filename(buffer);
    std::ofstream file(filename, std::ios::out | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "ERROR opening file: " << filename << std::endl;
        close(newsockfd);
        close(sockfd);
        exit(1);
    }

    // Receive and save the file content
    bzero(buffer, sizeof(buffer));
    while ((n = read(newsockfd, buffer, sizeof(buffer))) > 0) {
        file.write(buffer, n);
        bzero(buffer, sizeof(buffer));
    }

    if (n < 0)
        error("ERROR writing to file");

    std::cout << "File received successfully." << std::endl;

    close(newsockfd);
    close(sockfd);
    return 0;
}
