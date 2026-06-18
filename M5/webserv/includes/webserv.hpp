#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <csignal>

// System includes
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <dirent.h>
#include <errno.h>

// Constants
#define BUFFER_SIZE 8192
#define MAX_CONNECTIONS 1024
#define CONNECTION_TIMEOUT 60
#define CGI_TIMEOUT 10

// HTTP Methods
enum HttpMethod {
    METHOD_GET,
    METHOD_POST,
    METHOD_PUT,
    METHOD_HEAD,
    METHOD_DELETE,
    METHOD_OPTIONS,
    METHOD_TRACE,
    METHOD_CONNECT,
    METHOD_PATCH,
    METHOD_STATUS,
    METHOD_UNKNOWN
};

// Connection States
enum ConnectionState {
    STATE_READING_HEADERS,
    STATE_READING_BODY,
    STATE_PROCESSING,
    STATE_WRITING_RESPONSE,
    STATE_DONE,
    STATE_ERROR
};

// Forward declarations
struct RouteConfig {
    std::vector<std::string> allowed_methods;
    std::string root_directory;
    std::string index_file;
    bool directory_listing;
    std::string upload_path;
    std::vector<std::string> cgi_extensions;
    std::string redirect_url;
    int redirect_code;  // HTTP status code for redirect (301, 302, 307, 308)
    size_t max_body_size;

    RouteConfig() : directory_listing(false), redirect_code(0), max_body_size(1048576) {}
};

struct ServerConfig {
    int port;
    std::string server_name;
    std::map<std::string, RouteConfig> routes;
    size_t max_body_size;
    std::map<int, std::string> error_pages;
    std::string root;
    std::string index;
    bool autoindex;

    ServerConfig() : port(0), max_body_size(1048576), root("./www"), index("index.html"), autoindex(false) {}
};

// Server Socket Structure
struct ServerSocket {
    int fd;
    int port;
    const ServerConfig* config;
    
    ServerSocket() : fd(-1), port(0), config(NULL) {}
};

// Client Connection Structure
struct ClientConnection {
    int fd;
    int server_port;  // Which server port accepted this connection
    ConnectionState state;
    time_t last_activity;
    std::string buffer;
    size_t bytes_sent;
    bool keep_alive;
    
    ClientConnection() : fd(-1), server_port(0), state(STATE_READING_HEADERS), 
                        last_activity(0), bytes_sent(0), keep_alive(false) {}
};

extern volatile sig_atomic_t g_running;

#endif
