#ifndef SERVER_MANAGER_HPP
#define SERVER_MANAGER_HPP

#include "../webserv.hpp"
#include "../config/ConfigParser.hpp"
#include "../http/HttpHandler.hpp"
#ifdef BONUS
# include "../utils/SessionManager.hpp"
#endif
#include <sys/epoll.h>

// Forward declaration
class HttpHandler;

class ServerManager {
private:
    // Configuration
    std::vector<ServerConfig> _server_configs;
    std::vector<ServerSocket> _server_sockets;
    
    // Epoll management
    int _epoll_fd;
    struct epoll_event _events[MAX_CONNECTIONS];
    
    // Client tracking
    std::map<int, ClientConnection> _clients;

#ifdef BONUS
    // Session management
    SessionManager _session_manager;
#endif

    // State
    volatile bool _running;
    time_t _last_cleanup;

    // HTTP handler
    HttpHandler* _http_handler;
    
    // Constants
    static const int CLIENT_TIMEOUT = 30;
    static const int CLEANUP_INTERVAL = 5;
    
    // Private helper methods
    bool initializeServerSockets();
    bool createServerSocket(const ServerConfig& config);
    bool setupEpoll();
    void addToEpoll(int fd, uint32_t events);
    void modifyEpoll(int fd, uint32_t events);
    void removeFromEpoll(int fd);
    bool isServerSocket(int fd) const;
    const ServerSocket* findServerSocket(int fd) const;
    void handleNewConnection(const ServerSocket& server_socket);
    void cleanupTimeouts();
    void logError(const std::string& operation, const std::string& details = "");
    
public:
    ServerManager();
    ~ServerManager();
    
    // Core functions
    bool initialize(const std::string& config_file);
    void run();
    void shutdown();
    void requestShutdown();
    
    // Getters for HttpHandler
    int getEpollFd() const { return _epoll_fd; }
    const std::vector<ServerSocket>& getServerSockets() const { return _server_sockets; }
    const ServerConfig* findServerConfig(int port) const;
#ifdef BONUS
    SessionManager* getSessionManager() { return &_session_manager; }
#endif

    // Client management (called by HttpHandler)
    void closeClient(int client_fd);
    ClientConnection* getClient(int client_fd);
    
    // Debug utilities
    void printServerStatus() const;
    void printAllClients() const;
    void printServerSockets() const;
};

#endif
