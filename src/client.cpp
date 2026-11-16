
#include <iostream>
#include <string>
#include <cerrno>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define MAX_MESSAGE_SIZE 4096

// Sending all bytes, handling partial writes.
bool sendAll(int socket_fd, const std::string& message) {

    size_t total_sent = 0;

    while (total_sent < message.size()) {

        ssize_t sent = send(
            socket_fd,
            message.data() + total_sent,
            message.size() - total_sent,
            MSG_NOSIGNAL
        );

        if (sent < 0) {

            if (errno == EINTR)
                continue;

            return false;
        }

        if (sent == 0)
            return false;

        total_sent += static_cast<size_t>(sent);
    }

    return true;
}

// Reading a complete newline-terminated message.
bool recvLine(int socket_fd, std::string& message) {

    message.clear();

    while (true) {

        char ch;

        ssize_t received = recv(socket_fd, &ch, 1, 0);

        if (received < 0) {

            if (errno == EINTR)
                continue;

            return false;
        }

        if (received == 0)
            return false;

        if (ch == '\n')
            return true;

        if (message.size() >= MAX_MESSAGE_SIZE)
            return false;

        message += ch;
    }
}

int main() {

    // 1. Creating socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (client_socket < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // 2. Configuring server address
    sockaddr_in server_address{};

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    if (inet_pton(
            AF_INET,
            SERVER_IP,
            &server_address.sin_addr
        ) <= 0) {

        std::cerr << "Invalid server IP address.\n";
        close(client_socket);
        return 1;
    }

    // 3. Connecting to server
    if (connect(
            client_socket,
            (sockaddr*)&server_address,
            sizeof(server_address)
        ) < 0) {

        perror("Connection failed");
        close(client_socket);
        return 1;
    }

    std::cout << "Connected to NetForge server!\n";
    std::cout << "Type 'exit' to disconnect.\n";

    std::string message;
    std::string response;

    // 4. Exchanging messages
    while (true) {

        std::cout << "\nYou: ";

        if (!std::getline(std::cin, message))
            break;

        if (message == "exit")
            break;

        if (message.size() > MAX_MESSAGE_SIZE) {

            std::cout << "Message too long. Maximum "
                      << MAX_MESSAGE_SIZE << " bytes.\n";

            continue;
        }

        // Adding newline as message delimiter
        message += '\n';

        if (!sendAll(client_socket, message)) {

            perror("Sending failed");
            break;
        }

        if (!recvLine(client_socket, response)) {

            std::cout << "Server disconnected or response failed.\n";
            break;
        }

        std::cout << "Server: " << response << "\n";
    }

    // 5. Graceful shutdown
    shutdown(client_socket, SHUT_RDWR);
    close(client_socket);

    std::cout << "Connection closed.\n";

    return 0;
}
