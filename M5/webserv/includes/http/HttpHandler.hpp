#ifndef HTTP_HANDLER_HPP
#define HTTP_HANDLER_HPP

#include "../webserv.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "../cgi/CgiHandler.hpp"
#include <sys/epoll.h>
#include "../utils/Utils.hpp"
#include "../config/ConfigParser.hpp"
#include <cstdio>

// Forward declaration
class ServerManager;

class HttpHandler {
private:
    HttpHandler(HttpHandler const &other);
    HttpHandler &operator=(HttpHandler const &other);

    ServerManager* _server_manager;
    int _epoll_fd;
    bool _is_child;
    std::string _file_info;
    
    // Client request/response tracking
    std::map<int, std::string> _client_buffers;
    std::map<int, HttpRequest> _client_requests;
    std::map<int, HttpResponse> _client_responses;
    std::map<int, std::string> _response_buffers;
    std::map<int, size_t> _response_offsets;

    void processRequest(int client_fd, int server_port);
    HttpResponse handleUpload(const HttpRequest& request, const ServerConfig& config, int client_fd);
    HttpResponse handleDelete(const HttpRequest& request, const ServerConfig& config, int client_fd);
    HttpResponse handleJsonApi(const HttpRequest& request);
    HttpResponse handleStatus(int server_port);
    const ServerConfig* findServerForClient(int client_fd);
    std::string getMethodAllowed(const std::string& uri, const ServerConfig& config);
    bool methodAllowed(const std::string& uri, const std::string& method, const ServerConfig& config);
    const RouteConfig* findMatchingRoute(const std::string& uri, const ServerConfig& config);
    
public:
    HttpHandler(ServerManager* manager, int epoll_fd);
    ~HttpHandler();
    
    void handleRead(int fd);
    void handleWrite(int fd);
    void acceptConnection(int server_fd, int server_port);
    void closeConnection(int client_fd);
    bool IsChild() const;
};

#endif