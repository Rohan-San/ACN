#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
      // These details work for our lab's PCs
        std::cerr << "Usage: " << argv[0] << " ThinkCentre-M70t 4023 hello.txt" << std::endl;
        exit(1);
    }

    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[1024];

    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        std::cerr << "ERROR, no such host" << std::endl;
        exit(1);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    // Send the file name to the server
    std::string filename(argv[3]);
    n = write(sockfd, filename.c_str(), filename.length());
    if (n < 0)
        error("ERROR writing to socket");

    std::ifstream file(filename, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "ERROR opening file: " << filename << std::endl;
        close(sockfd);
        exit(1);
    }

    // Send the file content to the server
    while (!file.eof()) {
        file.read(buffer, sizeof(buffer));
        n = write(sockfd, buffer, file.gcount());
        if (n < 0)
            error("ERROR writing to socket");
        bzero(buffer, sizeof(buffer));
    }

    file.close();
    close(sockfd);

    std::cout << "File sent successfully." << std::endl;
    return 0;
}
