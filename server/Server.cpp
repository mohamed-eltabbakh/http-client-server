#include "Server.h"
#include <cmath>

Server::Server(std::string &port_number, unsigned short backlog, unsigned long max_workers) {
    this->port_number = port_number;
    this->backlog = backlog;
    this->max_workers = max_workers;
    server_socket = new ServerSocket(port_number, backlog);
}

Server::~Server() {
    delete(server_socket);
}

void Server::run() {
    std::string http_version(HTTP_VERSION);
    while (true) {
        
        for (auto it = workers_pool.begin(); it < workers_pool.end();) {
            HttpWorkerThread *worker = *it;
            if (worker != nullptr && worker->is_done()) {
                delete worker;
                it = workers_pool.erase(it);
            } else {
                it++;
            }
        }

        while (workers_pool.size() >= max_workers) {
            sleep(2);
        }

        int connecting_socket_fd = server_socket->accept_connection();

        if (connecting_socket_fd == -1) {
            // Error
        }

        int timeout = TIMEOUT - floor(0.01 * TIMEOUT * workers_pool.size());
        HttpWorkerThread *worker = new HttpWorkerThread(connecting_socket_fd, http_version, timeout);
        workers_pool.push_back(worker);
    }
}

std::string Server::get_port_number() const {
    return port_number;
}

unsigned short Server::get_backlog() const {
    return backlog;
}

unsigned long Server::get_max_workers() const {
    return max_workers;
}

int Server::get_server_socket_fd() const {
    return server_socket->get_socket_fd();
}