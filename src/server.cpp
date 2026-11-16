
#include <iostream>
#include <string>
#include <cstring>
#include <thread>
#include <mutex>
#include <cerrno>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_MESSAGE_SIZE 4096

std::mutex cout_mutex;

// Sending the complete message, even if send()
// transmits only part of it.
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

// Reading one newline-terminated message.
// Handles TCP fragmentation and multiple messages.
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

        if (received == 0) {
            return !message.empty();
        }

        if (ch == '\n')
            return true;

        if (message.size() >= MAX_MESSAGE_SIZE)
            return false;

        message += ch;
    }
}

// Handles an individual client.
void handleClient(int client_socket, int client_id) {

    {
        std::lock_guard<std::mutex> lock(cout_mutex);

        std::cout << "[+] Client " << client_id
                  << " connected.\n";
    }

    std::string message;

    while (recvLine(client_socket, message)) {

        {
            std::lock_guard<std::mutex> lock(cout_mutex);

            std::cout << "[Client " << client_id
                      << "] " << message << "\n";
        }

        std::string response =
            "Server received: " + message + "\n";

        if (!sendAll(client_socket, response)) {

            std::lock_guard<std::mutex> lock(cout_mutex);

            std::cerr << "[-] Send failed for client "
                      << client_id << "\n";

            break;
        }
    }

    {
        std::lock_guard<std::mutex> lock(cout_mutex);

        std::cout << "[-] Client " << client_id
                  << " disconnected.\n";
    }

    close(client_socket);
}

int main() {

    // 1. Creating server socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // 2. Allowing address reuse
    int opt = 1;

    if (setsockopt(
            server_fd,
            SOL_SOCKET,
            SO_REUSEADDR,
            &opt,
            sizeof(opt)
        ) < 0) {

        perror("setsockopt failed");
        close(server_fd);
        return 1;
    }

    // 3. Configuring server address
    sockaddr_in server_address{};

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    // 4. Binding
    if (bind(
            server_fd,
            (sockaddr*)&server_address,
            sizeof(server_address)
        ) < 0) {

        perror("Bind failed");
        close(server_fd);
        return 1;
    }

    // 5. Listening
    if (listen(server_fd, 10) < 0) {

        perror("Listen failed");
        close(server_fd);
        return 1;
    }

    std::cout << "NetForge TCP Server running on port "
              << PORT << "\n";

    int client_id = 0;

    // 6. Accepting clients continuously
    while (true) {

        sockaddr_in client_address{};
        socklen_t client_len = sizeof(client_address);

        int client_socket = accept(
            server_fd,
            (sockaddr*)&client_address,
            &client_len
        );

        if (client_socket < 0) {

            if (errno == EINTR)
                continue;

            perror("Accept failed");
            continue;
        }

        client_id++;

        // 7. Creating a separate thread for each client
        try {

            std::thread(
                handleClient,
                client_socket,
                client_id
            ).detach();

        } catch (const std::exception& e) {

            std::cerr << "Thread creation failed: "
                      << e.what() << "\n";

            close(client_socket);
        }
    }

    close(server_fd);

    return 0;
}
