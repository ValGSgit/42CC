#include "../../includes/http/HttpHandler.hpp"
#include "../../includes/server/ServerManager.hpp"

HttpHandler::~HttpHandler() {}

HttpHandler::HttpHandler(HttpHandler const &other) : _server_manager(other._server_manager), _epoll_fd(other._epoll_fd), _is_child(other._is_child) {
    *this = other;
}

HttpHandler &HttpHandler::operator=(HttpHandler const &other) {
    if (this == &other)
        return (*this);
    return (*this);
}

HttpHandler::HttpHandler(ServerManager* manager, int epoll_fd) 
    : _server_manager(manager), _epoll_fd(epoll_fd), _is_child(false) {}

const ServerConfig* HttpHandler::findServerForClient(int client_fd) {
    ClientConnection* client = _server_manager->getClient(client_fd);
    if (!client) {
        return NULL;
    }
    return _server_manager->findServerConfig(client->server_port);
}

std::string HttpHandler::getMethodAllowed(const std::string& uri, const ServerConfig& config)
{
    std::string MethodAllowed;

    // Try to find the most specific matching route
    std::string route_path = uri;
    
    // Keep going up the directory tree until we find a matching route
    while (true) {
        try {
            const RouteConfig& route = config.routes.at(route_path); //at not allowed
            
            // Found a matching route! Check if method is allowed
            for (size_t i = 0; i < route.allowed_methods.size(); ++i) {
                    if (i != 0)
                        MethodAllowed += ", ";
                    MethodAllowed += route.allowed_methods[i];  // Method is allowed
            }
            
            // Route found but method not allowed
            return MethodAllowed;
            
        } catch (const std::exception& e) {
            // Route not found, try parent directory
            if (route_path == "/") {
                // Reached root, no route found
                // Allow GET by default for backwards compatibility
                return ("GET");
            }
            
            // Remove last path segment and try again
            size_t last_slash = route_path.find_last_of('/');
            if (last_slash == 0) {
                route_path = "/";  // Parent is root
            } else if (last_slash != std::string::npos) {
                route_path = route_path.substr(0, last_slash);
            } else {
                // No slash found (shouldn't happen with valid URIs)
                return ("GET");
            }
        }
    }
}

bool HttpHandler::methodAllowed(const std::string& uri, const std::string& method, const ServerConfig& config) {
    // Try to find the most specific matching route
    std::string route_path = uri;
    
    // Keep going up the directory tree until we find a matching route
    while (true) {
        try {
            const RouteConfig& route = config.routes.at(route_path); //at not allowed
            
            // Found a matching route! Check if method is allowed
            for (size_t i = 0; i < route.allowed_methods.size(); ++i) {
                if (method == route.allowed_methods[i]) {
                    return true;  // Method is allowed
                }
            }
            
            // Route found but method not allowed
            return false;
            
        } catch (const std::exception& e) {
            // Route not found, try parent directory
            if (route_path == "/") {
                // Reached root, no route found
                // Allow GET by default for backwards compatibility
                return (method == "GET");
            }
            
            // Remove last path segment and try again
            size_t last_slash = route_path.find_last_of('/');
            if (last_slash == 0) {
                route_path = "/";  // Parent is root
            } else if (last_slash != std::string::npos) {
                route_path = route_path.substr(0, last_slash);
            } else {
                // No slash found (shouldn't happen with valid URIs)
                return (method == "GET");
            }
        }
    }
}

// Find the most specific matching route for a URI
const RouteConfig* HttpHandler::findMatchingRoute(const std::string& uri, const ServerConfig& config) {
    std::string route_path = uri;
    
    // Keep going up the directory tree until we find a matching route
    while (true) {
        std::map<std::string, RouteConfig>::const_iterator it = config.routes.find(route_path);
        if (it != config.routes.end()) {
            return &(it->second);  // Found matching route
        }
        
        // Route not found, try parent directory
        if (route_path == "/") {
            return NULL;  // No matching route found
        }
        
        // Remove last path segment and try again
        size_t last_slash = route_path.find_last_of('/');
        if (last_slash == 0) {
            route_path = "/";  // Parent is root
        } else if (last_slash != std::string::npos) {
            route_path = route_path.substr(0, last_slash);
        } else {
            return NULL;  // No slash found
        }
    }
}
    
void HttpHandler::processRequest(int client_fd, int server_port) {
    HttpRequest& request = _client_requests[client_fd];
    HttpResponse& response = _client_responses[client_fd];
    
    const ServerConfig* config = _server_manager->findServerConfig(server_port);
    if (!config) {
        std::cerr << "Error: No server configuration found for port " << server_port << std::endl;
        response = HttpResponse::errorResponse(HTTP_INTERNAL_SERVER_ERROR);
        _response_buffers[client_fd] = response.getResponseString();
        _response_offsets[client_fd] = 0;
        // Switch to write mode
        struct epoll_event ev;
        ev.events = EPOLLOUT | EPOLLET;
        ev.data.fd = client_fd;
        epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, client_fd, &ev);
        return;
    }

    std::cout << "Processing " << request.methodToString() << " " << request.getUri() << std::endl;
    
    // Check for client request errors FIRST (before route matching)
    // This ensures malformed requests get proper 400 errors instead of 404
    if (request.getStatus()) {
        response = HttpResponse::errorResponseWithConfig(request.getStatus(), config);
        _response_buffers[client_fd] = response.getResponseString();
        _response_offsets[client_fd] = 0;
        // Switch to write mode
        struct epoll_event ev;
        ev.events = EPOLLOUT | EPOLLET;
        ev.data.fd = client_fd;
        epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, client_fd, &ev);
        return;
    }
    
    std::string uri = request.getUri();
    std::cout << "Requested URI: " << uri << std::endl;
    const RouteConfig* route = findMatchingRoute(uri, *config);
    
    if (!route) {
        std::cerr << "Warning: No matching route found for URI: " << uri << std::endl;
        response = HttpResponse::errorResponseWithConfig(HTTP_NOT_FOUND, config);
        _response_buffers[client_fd] = response.getResponseString();
        _response_offsets[client_fd] = 0;
        // Switch to write mode
        struct epoll_event ev;
        ev.events = EPOLLOUT | EPOLLET;
        ev.data.fd = client_fd;
        epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, client_fd, &ev);
        return;
    }
    
    // Check max body size
    size_t max_body_size = config->max_body_size;
    if (route->max_body_size > 0)
        max_body_size = route->max_body_size;
    
    if (request.getContentLength() > max_body_size)
        response = HttpResponse::errorResponseWithConfig(HTTP_PAYLOAD_TOO_LARGE, config);
    // Handle OPTIONS method (should be checked BEFORE method allowed check)
    else if (request.getMethod() == METHOD_OPTIONS) {
        // OPTIONS returns allowed methods for the URI
        response.setStatus(HTTP_OK);
        response.setAllow(getMethodAllowed(uri, *config));
        response.setContentLength(0);
    }
    // Handle STATUS method - returns server status information
    else if (request.getMethod() == METHOD_STATUS) {
        response = handleStatus(server_port);
    }
    // Handle known but unsupported methods (TRACE, CONNECT, PATCH)
    else if (request.getMethod() == METHOD_TRACE || 
             request.getMethod() == METHOD_CONNECT || 
             request.getMethod() == METHOD_PATCH) {
        response = HttpResponse::errorResponseWithConfig(HTTP_METHOD_NOT_ALLOWED, config);
        response.setAllow(getMethodAllowed(uri, *config));
    }
    // Check for configured redirects in routes FIRST
    else {
        //const RouteConfig* route = findMatchingRoute(uri, *config); // Moved up
        if (route && !route->redirect_url.empty() && route->redirect_code > 0) {
            // This route has a redirect configured
            std::string redirect_location = route->redirect_url;
            
            // If redirect URL is relative and doesn't start with http/https, make it absolute
            if (redirect_location.find("http://") != 0 && redirect_location.find("https://") != 0) {
                // For relative URLs, you might want to construct full URL
                // For now, we'll just use the redirect_url as-is (works for relative redirects)
            }
            
            response = HttpResponse::redirectResponse(redirect_location, route->redirect_code);
        }
        // Check if method is allowed BEFORE checking Content-Length
        else if (!methodAllowed(uri, request.methodToString(), *config)) {
            response = HttpResponse::errorResponseWithConfig(HTTP_METHOD_NOT_ALLOWED, config);
            response.setAllow(getMethodAllowed(uri, *config));
        }
#ifdef BONUS
        // Session API endpoints (MUST be checked BEFORE generic POST handler!)
        else if (uri == "/api/session/login" && request.getMethod() == METHOD_POST) {
            SessionManager* sm = _server_manager->getSessionManager();

            // Parse username from request body
            std::string username = "demo_user"; // default
            std::string body;// body in string, not in binary
            const std::vector<char> &body_vec = request.getBody();
            
            // Safely convert vector to string - check size first
            if (!body_vec.empty()) {
                body.assign(body_vec.begin(), body_vec.end());
            }
            
            // Simple JSON parsing to extract username
            size_t username_pos = body.find("\"username\"");
            if (username_pos != std::string::npos) {
                size_t colon_pos = body.find(":", username_pos);
                if (colon_pos != std::string::npos) {
                    size_t quote_start = body.find("\"", colon_pos);
                    size_t quote_end = body.find("\"", quote_start + 1);
                    if (quote_start != std::string::npos && quote_end != std::string::npos) {
                        username = body.substr(quote_start + 1, quote_end - quote_start - 1);
                        // Sanitize username (remove special chars, limit length)
                        if (username.length() > 30) username = Utils::sanitizeFilename(username);
                    }
                }
            }

            // CHECK: Does this username already have an active session? (OPTION 3)
            std::string existing_session_id = sm->getSessionByUsername(username);
            
            if (!existing_session_id.empty()) {
                // User is already logged in - REJECT the new login
                response.setStatus(409); // HTTP 409 Conflict
                response.setContentType("application/json");
                response.setBody("{\"success\": false, \"message\": \"User '" + username + "' is already logged in. Please logout first.\"}");
            } else {
                // No active session - proceed with login
                std::string session_id = sm->createSession();

                // Store data in the session
                SessionData* session = sm->getSession(session_id);
                if (session) {
                    session->data["username"] = username;
                    session->data["authenticated"] = "true";
                    session->data["role"] = "user";
                    
                    // Register username with this session
                    sm->registerUsername(session_id, username);
                }

                response.setStatus(HTTP_OK);
                response.setContentType("application/json");
                response.setCookie("SESSIONID", session_id, 3600);  // 1 hour expiry
                response.setBody("{\"success\": true, \"message\": \"Login successful\", \"session_id\": \"" + session_id + "\", \"username\": \"" + username + "\"}");
            }
        }
        else if (uri == "/api/session/profile" && request.getMethod() == METHOD_GET) {
            // Get session data (protected endpoint)
            std::string session_id = request.getCookie("SESSIONID");

            if (session_id.empty()) {
                response.setStatus(HTTP_UNAUTHORIZED);
                response.setContentType("application/json");
                response.setBody("{\"success\": false, \"message\": \"Not authenticated\"}");
            } else {
                SessionManager* sm = _server_manager->getSessionManager();
                SessionData* session = sm->getSession(session_id);

                if (!session || session->data["authenticated"] != "true") {
                    response.setStatus(HTTP_UNAUTHORIZED);
                    response.setContentType("application/json");
                    response.setBody("{\"success\": false, \"message\": \"Invalid or expired session\"}");
                } else {
                    // Return user profile
                    std::string username = session->data["username"];
                    std::string role = session->data["role"];
                    response.setStatus(HTTP_OK);
                    response.setContentType("application/json");
                    response.setBody("{\"success\": true, \"username\": \"" + username + "\", \"role\": \"" + role + "\"}");
                }
            }
        }
        else if (uri == "/api/session/logout" && request.getMethod() == METHOD_POST) {
            // Destroy session
            std::string session_id = request.getCookie("SESSIONID");

            if (!session_id.empty()) {
                SessionManager* sm = _server_manager->getSessionManager();
                sm->destroySession(session_id);
            }

            response.setStatus(HTTP_OK);
            response.setContentType("application/json");
            response.clearCookie("SESSIONID");
            response.setBody("{\"success\": true, \"message\": \"Logged out successfully\"}");
        }
        else if (uri == "/api/session/info" && request.getMethod() == METHOD_GET) {
            // Get session manager stats
            SessionManager* sm = _server_manager->getSessionManager();
            size_t active_sessions = sm->getActiveSessionCount();

            response.setStatus(HTTP_OK);
            response.setContentType("application/json");
            response.setBody("{\"active_sessions\": " + Utils::toString(active_sessions) + "}");
        }
        else if (uri == "/api/session/clear" && request.getMethod() == METHOD_POST) {
            // Clear all sessions (for testing purposes)
            SessionManager* sm = _server_manager->getSessionManager();
            sm->destroyAllSessions();

            response.setStatus(HTTP_OK);
            response.setContentType("application/json");
            response.setBody("{\"success\": true, \"message\": \"All sessions cleared\"}");
        }
#endif
        // API test endpoint
        else if (uri == "/api/test") {
            response = handleJsonApi(request);
        }
        // CGI handling
        else if (uri.find("/cgi-bin/") == 0) {
            // Ensure route is valid before forking
            if (!route) {
                response = HttpResponse::errorResponseWithConfig(HTTP_NOT_FOUND, config);
            } else {
                pid_t pid = fork();
                if (pid == -1) {
                    std::cerr << "fork failed\n";
                    response = HttpResponse::errorResponseWithConfig(HTTP_INTERNAL_SERVER_ERROR, config);
                }
                else if (pid == 0) {
                // Child process
                _is_child = true;
                CgiHandler cgi;
                std::string script_path = config->root + uri;
                response = cgi.executeCgi(request, script_path, route->cgi_extensions);
                _response_buffers[client_fd] = response.getResponseString();
                _response_offsets[client_fd] = 0;
                handleWrite(client_fd);
                _server_manager->requestShutdown();
                return;
            }
            else {
                // Parent process - reset for next request
                struct epoll_event ev;
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = client_fd;
                if (epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, client_fd, &ev) == -1) {
                    std::cerr << "epoll_ctl failed2\n";
                    close(client_fd);
                    return;
                }
            
                // Reset client tracking data
                _client_buffers[client_fd] = "";
                _client_requests[client_fd] = HttpRequest();
                _client_responses[client_fd] = HttpResponse();
                _response_buffers[client_fd] = "";
                _response_offsets[client_fd] = 0;
                
                // Register client in ServerManager
                ClientConnection* client = _server_manager->getClient(client_fd);
                if (client) {
                    client->fd = client_fd;
                    client->state = STATE_READING_HEADERS;
                    client->last_activity = time(NULL);
                }
                return;
            }
            }
        }
        // Generic POST handler for uploads (after all API endpoints!)
        else if (request.getMethod() == METHOD_POST) {
            if (route) {
                response = handleUpload(request, *config, client_fd);
            } else {
                response = HttpResponse::errorResponseWithConfig(HTTP_NOT_FOUND, config);
            }
        }
        // Generic DELETE handler
        else if (request.getMethod() == METHOD_DELETE) {
            if (route) {
                response = handleDelete(request, *config, client_fd);
            } else {
                response = HttpResponse::errorResponseWithConfig(HTTP_NOT_FOUND, config);
            }
        }
        // Root index
        else if (uri == "/") {
            std::string index_path = config->root + "/" + config->index;
            response = HttpResponse::fileResponse(index_path);
        }
        // Static files
        else if (uri.find("/static/") == 0) {
            std::string filepath = config->root + uri;
            response = HttpResponse::fileResponse(filepath);
        } else {
            // Try to serve as file or directory
            if (!route) {
                response = HttpResponse::errorResponseWithConfig(HTTP_NOT_FOUND, config);
            } else {
                std::string filepath = route->root_directory + uri;
                if (Utils::fileExists(filepath)) {
                    if (Utils::isDirectory(filepath)) {
                        // Check if directory listing is enabled for this route
                        if (route->directory_listing) {
                            response = HttpResponse::directoryListingResponse(filepath, uri);
                        } else {
                            // Try index file
                            std::string index_path = Utils::joinPath(filepath, route->index_file);
                            if (Utils::fileExists(index_path) && !Utils::isDirectory(index_path)) {
                                response = HttpResponse::fileResponse(index_path);
                            } else {
                                response = HttpResponse::errorResponseWithConfig(HTTP_FORBIDDEN, config);
                            }
                        }
                    } else {
                        response = HttpResponse::fileResponse(filepath);
                    }
                } else {
                    response = HttpResponse::errorResponseWithConfig(HTTP_NOT_FOUND, config);
                }
            }
        }
    }
    // remove body for HEAD
    if (request.getMethod() == METHOD_HEAD) {
            response.removeBody();
    }

    if (_client_requests[client_fd].getHeader("connection") == "close")
        response.setHeader("Connection", "close");
    
    // Prepare response for sending
    _response_buffers[client_fd] = response.getResponseString();
    _response_offsets[client_fd] = 0;
    
    // Switch to write mode
    struct epoll_event ev;
    ev.events = EPOLLOUT | EPOLLET;
    ev.data.fd = client_fd;
    epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, client_fd, &ev);
}

// return the index of boundary in the buffer if found
// what if the boundary lays between buffer?
static int check_boundary(const std::vector<char> &buffer, const char *boundary, size_t len)
{
    size_t	i;
	size_t	j;
    size_t len_boundary = 0;

    while (boundary[len_boundary])
        len_boundary++;
    if (!*boundary)
        return (0);
    i = 0;
	while (i < len)
	{
		j = 0;
		if (buffer[i] == boundary[j])
		{
			while (buffer[i + j] == boundary[j] && boundary[j] != '\0')
			{
				j++;
				if (j == len_boundary && i + j <= len)
					return (i);
			}
		}
		i++;
	}
	return (0);
}

HttpResponse HttpHandler::handleDelete(const HttpRequest& request, const ServerConfig& config, int client_fd)
{
    (void)config;
    (void)client_fd;
    std::string file_path = config.root + request.getUri();
    
    // Check if path exists
    if (!Utils::fileExists(file_path))
        return HttpResponse::errorResponse(HTTP_NOT_FOUND, "File not found!");
    
    // Check if it's a directory - directories cannot be deleted via HTTP DELETE
    if (Utils::isDirectory(file_path))
        return HttpResponse::errorResponse(HTTP_FORBIDDEN, "Cannot delete a directory");
    
    // Try to open file to verify we can delete it
    int fd = open(file_path.c_str(), O_WRONLY, 0644);
    if (fd == -1)
        return HttpResponse::errorResponse(HTTP_FORBIDDEN, "Cannot access file!");
    close(fd);
    
    // Delete the file
    if (std::remove(file_path.c_str()) != 0)
        return HttpResponse::errorResponse(HTTP_INTERNAL_SERVER_ERROR, "Failed to delete file");
    
    return HttpResponse::messageResponse(HTTP_OK, "File deleted!");
}



HttpResponse HttpHandler::handleUpload(const HttpRequest& request, const ServerConfig& config, int client_fd) {
    (void) client_fd;
    std::cout << "Handling file upload" << std::endl;
    const std::vector<char>& body = request.getBody();
    
    // Handle empty POST (no body) - return 200 OK
    if (body.empty()) {
        return HttpResponse::messageResponse(HTTP_OK, "Upload Ready", "Upload endpoint ready");
    }
    
    // default upload path
    std::string upload_dir = config.root + "/uploads/";
    // Find upload path from config
    try
    {
        const RouteConfig &route = config.routes.at(request.getUri());
        if (route.upload_path != "")
            upload_dir = route.upload_path + "/";
    }
    catch(const std::exception& e){}
    // use temp string to find things
    std::string temp = request.getHeader("content-type");
    std::size_t needle = temp.find("boundary=");
    std::string boundary;
    std::string boundary_end;
    std::string file_info;
    size_t start = 0;
    size_t size = body.size();
    // if boundary is found
    if (needle != std::string::npos)
    {
        boundary = "--" + temp.substr(needle + 9, temp.find_first_of("\r\n", needle + 9) - needle - 9);
        boundary_end = "\r\n" + boundary + "--";
        int i = 0;
        while (body[i])
        {
            file_info += body[i];
            if (body[i] == '\r')
            {
                if (body[i + 1] == '\n' && body[i + 2] == '\r' && body[i + 3] == '\n')
                {
                    i += 4;
                    break ;
                }
            }
            i++;
        }
        start = i;
        size = check_boundary(body, boundary_end.c_str(), body.size());
        size -= start;
    }
    _file_info = file_info;
    // Simple upload handling - save to uploads directory
    std::string filename;
    filename = "uploaded_file_" + Utils::toString(static_cast<int>(time(NULL)));
    temp = _file_info;
    needle = temp.find("filename=");
    if (needle != std::string::npos)
        filename = temp.substr(needle + 10, temp.find_first_of("\"", needle + 10) - needle - 10);

    // SECURITY FIX: Sanitize filename to prevent path traversal
    filename = Utils::sanitizeFilename(filename);
    
    // SECURITY FIX: Check if filename has safe path (no .., /, etc.)
    if (!Utils::isSafePath(filename)) {
        return HttpResponse::errorResponse(HTTP_FORBIDDEN, "Invalid filename");
    }
    //std::cout << "filename = " << filename << "\n";
/*     // SECURITY FIX: Validate file extension
    if (!Utils::isAllowedUploadExtension(filename)) {
        return HttpResponse::errorResponse(HTTP_FORBIDDEN, "File type not allowed");
    } */
    
    std::string filepath = upload_dir + filename;
    // if file already exist, add suffix
    if (Utils::fileExists(filepath))
        filepath = filepath + "_copy_" + Utils::toString(static_cast<int>(time(NULL)));
    
    // If no data to write (empty file or just boundary markers), create empty file
    if (size == 0) {
        std::ofstream file(filepath.c_str());
        if (file.is_open()) {
            file.close();
            return HttpResponse::messageResponse(HTTP_CREATED, "Upload Successful", "Empty file uploaded successfully!");
        } else {
            return HttpResponse::errorResponse(HTTP_INTERNAL_SERVER_ERROR, "Failed to create file");
        }
    }
    
    size_t total_write = 0;
    while (size > BUFFER_SIZE)
    {
        if (!Utils::writeFile(filepath, (void *)&body[start], BUFFER_SIZE))
        {
            std::remove(filepath.c_str());
            return HttpResponse::errorResponse(HTTP_INTERNAL_SERVER_ERROR, "Failed to save file"); // also remove file!
        }
        total_write += BUFFER_SIZE;
        size -= BUFFER_SIZE;
        start += BUFFER_SIZE;
    }
    // write the leftover
    if (Utils::writeFile(filepath, (void *)&body[start], size))
        return HttpResponse::messageResponse(HTTP_CREATED, "Upload Successful", "File uploaded successfully!");
    else
    {
        std::remove(filepath.c_str());
        return HttpResponse::errorResponse(HTTP_INTERNAL_SERVER_ERROR, "Failed to save file");
    }
}

HttpResponse HttpHandler::handleJsonApi(const HttpRequest& request) {
    std::cout << "Handling JSON API request" << std::endl;
    
    HttpResponse response;
    response.setStatus(HTTP_OK);
    response.setContentType("application/json");

    std::string json_body = "{\n";
    json_body += "  \"message\": \"Test API endpoint\",\n";
    json_body += "  \"method\": \"" + request.methodToString() + "\",\n";
    json_body += "  \"uri\": \"" + request.getUri() + "\",\n";
    //json_body += "  \"timestamp\": \"" + need to implement a get time + "\",\n";
    json_body += "  \"headers\": {\n";
    
    const std::map<std::string, std::string>& headers = request.getHeaders();
    bool first = true;
    for (std::map<std::string, std::string>::const_iterator it = headers.begin();
            it != headers.end(); ++it) {
        if (!first) json_body += ",\n";
        json_body += "    \"" + it->first + "\": \"" + it->second + "\"";
        first = false;
    }
        
    json_body += "\n  }\n}";
    response.setBody(json_body);
        
    return response;
}

HttpResponse HttpHandler::handleStatus(int server_port) {
    std::cout << "Handling STATUS request" << std::endl;
    
    HttpResponse response;
    response.setStatus(HTTP_OK);
    response.setContentType("text/plain");
    
    const ServerConfig* config = _server_manager->findServerConfig(server_port);
    
    // Build status information
    std::string status_body;
    status_body += "╔══════════════════════════════════════════════════════════════╗\n";
    status_body += "║                  WEBSERV STATUS REPORT                       ║\n";
    status_body += "╚══════════════════════════════════════════════════════════════╝\n\n";
    
    // Server time
    time_t now = time(NULL);
    char time_str[100];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&now));
    status_body += "Server Time: " + std::string(time_str) + "\n\n";
    
    // Port information
    status_body += "═══ PORT INFORMATION ═══\n";
    status_body += "Port: " + Utils::toString(server_port) + "\n";
    if (config) {
        status_body += "Server Name: " + config->server_name + "\n";
        status_body += "Root Directory: " + config->root + "\n";
        status_body += "Index File: " + config->index + "\n";
        status_body += "Autoindex: " + std::string(config->autoindex ? "enabled" : "disabled") + "\n";
        status_body += "Max Body Size: " + Utils::toString(config->max_body_size) + " bytes\n";
    }
    status_body += "\n";
    
    // Active connections
    status_body += "═══ ACTIVE CONNECTIONS ═══\n";
    int total_clients = 0;
    int reading_headers = 0;
    int reading_body = 0;
    int processing = 0;
    int writing = 0;
    int done = 0;
    int error = 0;
    
    // Count connections on this port
    std::vector<int> port_clients;
    std::map<int, std::string> client_info;
    
    for (std::map<int, HttpRequest>::iterator it = _client_requests.begin();
         it != _client_requests.end(); ++it) {
        ClientConnection* client = _server_manager->getClient(it->first);
        if (client && client->server_port == server_port) {
            port_clients.push_back(it->first);
            total_clients++;
            
            switch (client->state) {
                case STATE_READING_HEADERS: reading_headers++; break;
                case STATE_READING_BODY: reading_body++; break;
                case STATE_PROCESSING: processing++; break;
                case STATE_WRITING_RESPONSE: writing++; break;
                case STATE_DONE: done++; break;
                case STATE_ERROR: error++; break;
            }
            
            // Build client info string
            std::string info = "  Client fd=" + Utils::toString(it->first);
            info += " | State=";
            switch (client->state) {
                case STATE_READING_HEADERS: info += "READING_HEADERS"; break;
                case STATE_READING_BODY: info += "READING_BODY"; break;
                case STATE_PROCESSING: info += "PROCESSING"; break;
                case STATE_WRITING_RESPONSE: info += "WRITING"; break;
                case STATE_DONE: info += "DONE"; break;
                case STATE_ERROR: info += "ERROR"; break;
            }
            info += " | Last Activity: " + Utils::toString(static_cast<int>(now - client->last_activity)) + "s ago\n";
            
            if (_client_requests[it->first].isHeadersComplete()) {
                info += "    Method: " + _client_requests[it->first].methodToString();
                info += " | URI: " + _client_requests[it->first].getUri() + "\n";
            }
            
            client_info[it->first] = info;
        }
    }
    
    status_body += "Total Connections: " + Utils::toString(total_clients) + "\n";
    status_body += "  Reading Headers: " + Utils::toString(reading_headers) + "\n";
    status_body += "  Reading Body: " + Utils::toString(reading_body) + "\n";
    status_body += "  Processing: " + Utils::toString(processing) + "\n";
    status_body += "  Writing Response: " + Utils::toString(writing) + "\n";
    status_body += "  Done: " + Utils::toString(done) + "\n";
    status_body += "  Error: " + Utils::toString(error) + "\n\n";
    
    // Client details
    if (!port_clients.empty()) {
        status_body += "═══ CLIENT DETAILS ═══\n";
        for (size_t i = 0; i < port_clients.size(); ++i) {
            status_body += client_info[port_clients[i]];
        }
        status_body += "\n";
    }
    
    // Routes configured
    if (config && !config->routes.empty()) {
        status_body += "═══ CONFIGURED ROUTES ═══\n";
        for (std::map<std::string, RouteConfig>::const_iterator it = config->routes.begin();
             it != config->routes.end(); ++it) {
            status_body += "  Route: " + it->first + "\n";
            status_body += "    Allowed Methods: ";
            for (size_t i = 0; i < it->second.allowed_methods.size(); ++i) {
                if (i > 0) status_body += ", ";
                status_body += it->second.allowed_methods[i];
            }
            status_body += "\n";
            if (!it->second.root_directory.empty())
                status_body += "    Root: " + it->second.root_directory + "\n";
            if (!it->second.redirect_url.empty())
                status_body += "    Redirect: " + it->second.redirect_url + 
                              " (" + Utils::toString(it->second.redirect_code) + ")\n";
            if (it->second.directory_listing)
                status_body += "    Directory Listing: enabled\n";
            if (!it->second.cgi_extensions.empty()) {
                status_body += "    CGI Extensions: ";
                for (size_t i = 0; i < it->second.cgi_extensions.size(); ++i) {
                    if (i > 0) status_body += ", ";
                    status_body += it->second.cgi_extensions[i];
                }
                status_body += "\n";
            }
        }
        status_body += "\n";
    }
    
#ifdef BONUS
    // Session information
    SessionManager* sm = _server_manager->getSessionManager();
    if (sm) {
        status_body += "═══ SESSION INFORMATION ═══\n";
        status_body += "Active Sessions: " + Utils::toString(sm->getActiveSessionCount()) + "\n\n";
    }
#endif
    
    // Memory usage
    status_body += "═══ MEMORY USAGE ═══\n";
    status_body += "Request Buffers: " + Utils::toString(_client_buffers.size()) + "\n";
    status_body += "Response Buffers: " + Utils::toString(_response_buffers.size()) + "\n\n";
    
    status_body += "╚══════════════════════════════════════════════════════════════╝\n";
    status_body += "End of Status Report\n";
    
    response.setBody(status_body);
    
    return response;
}

void HttpHandler::handleRead(int client_fd) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    while ((bytes_read = read(client_fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0';
        _client_buffers[client_fd] += buffer;
        
        // Try to parse request
        if (_client_requests[client_fd].parseRequest(_client_buffers[client_fd], buffer, bytes_read)) {
            ClientConnection* client = _server_manager->getClient(client_fd);
            if (client) {
                processRequest(client_fd, client->server_port);
            } else {
                std::cerr << "Error: Client connection data not found for fd " << client_fd << std::endl;
                closeConnection(client_fd);
            }
            return;
        }
    }
    
    // bytes_read <= 0 - connection closed or would block
    // For EPOLLET (edge-triggered), EAGAIN/EWOULDBLOCK means no more data available
    // Don't close connection, just return and wait for next epoll event
    // Timeout handling is done separately in ServerManager::cleanupTimeouts()
    if (bytes_read == 0) {
        std::cout << "Client disconnected (fd: " << client_fd << ")" << std::endl;
        closeConnection(client_fd);
    }
    // bytes_read < 0: EAGAIN/EWOULDBLOCK (normal for non-blocking), or error
    // Don't check errno per subject requirements - let timeout handling deal with stuck connections
}

void HttpHandler::handleWrite(int client_fd) {
    std::string& buffer = _response_buffers[client_fd];
    size_t& offset = _response_offsets[client_fd];
    
    if (offset >= buffer.length()) {
        return;
    }
    ssize_t bytes_sent = write(client_fd, buffer.c_str() + offset, buffer.length() - offset);
    if (bytes_sent > 0) {
        offset += bytes_sent;
        if (offset >= buffer.length()) {
            std::cout << "✓ Response sent successfully to fd " << client_fd << std::endl;
            if (_client_requests[client_fd].getHeader("connection") == "close") {
                closeConnection(client_fd);
                return;
            }
            else if (_client_requests[client_fd].getHeader("connection") == "keep-alive")
            // Reset for next request
            {
                struct epoll_event ev;
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = client_fd;
                if (epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, client_fd, &ev) == -1) {
                    /* So if we get here and u see this its most likely the execution of a CGI script
                        that was running in a child process and the script took longer than epoll_wait loop time
                        so it doesnt get reset in the original events and times out*/
                    std::cerr << "epoll_ctl failed3\n";
                    close(client_fd);
                    return;
                }
            
                // Reset client tracking data
                _client_buffers[client_fd] = "";
                _client_requests[client_fd] = HttpRequest();
                _client_responses[client_fd] = HttpResponse();
                _response_buffers[client_fd] = "";
                _response_offsets[client_fd] = 0;
                
                // Register client in ServerManager
                ClientConnection* client = _server_manager->getClient(client_fd);
                if (client) {
                    client->fd = client_fd;
                    client->state = STATE_READING_HEADERS;
                    client->last_activity = time(NULL);
                }
            }
        }
    } else if (bytes_sent == -1) {
        // Write failed - connection will be closed by timeout if stuck
        // Don't check errno per subject requirements
        return;
    }
}


void HttpHandler::acceptConnection(int server_fd, int server_port) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd == -1) {
        // EAGAIN/EWOULDBLOCK is normal for edge-triggered epoll when no more connections
        // Other errors are logged but don't crash the server
        return;
    }
    
    // Validate file descriptor
    if (client_fd < 0 || client_fd >= MAX_CONNECTIONS) {
        std::cerr << "Invalid client file descriptor: " << client_fd << std::endl;
        close(client_fd);
        return;
    }
    
    Utils::setNonBlocking(client_fd);
        
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = client_fd;
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1) {
        std::cerr << "epoll_ctl failed4\n";
        close(client_fd);
        return;
    }

    // Initialize client tracking data
    _client_buffers[client_fd] = "";
    _client_requests[client_fd] = HttpRequest();
    _client_responses[client_fd] = HttpResponse();
    _response_buffers[client_fd] = "";
    _response_offsets[client_fd] = 0;
    
    // Register client in ServerManager
    ClientConnection* client = _server_manager->getClient(client_fd);
    if (client) {
        client->fd = client_fd;
        client->server_port = server_port;
        client->state = STATE_READING_HEADERS;
        client->last_activity = time(NULL);
    }
        
    std::cout << "✓ New connection accepted (fd: " << client_fd << ", port: " << server_port << ")" << std::endl;
}

void HttpHandler::closeConnection(int client_fd) {
    // Validate file descriptor before operations
    if (client_fd < 0) {
        return;
    }
    
    epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
    close(client_fd);

    // Clean up local tracking data
    _client_buffers.erase(client_fd);
    _client_requests.erase(client_fd);
    _client_responses.erase(client_fd);
    _response_buffers.erase(client_fd);
    _response_offsets.erase(client_fd);

    // Clean up in ServerManager
    _server_manager->closeClient(client_fd);

    std::cout << "Connection closed (fd: " << client_fd << ")" << std::endl;
}

bool HttpHandler::IsChild() const { return _is_child; }