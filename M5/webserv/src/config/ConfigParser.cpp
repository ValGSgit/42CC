#include "../../includes/config/ConfigParser.hpp"
#include "../../includes/utils/Utils.hpp"

ConfigParser::ConfigParser() {}

ConfigParser::~ConfigParser() {}

bool ConfigParser::parseConfig(const std::string& config_file) {
    _config_file = config_file;
    _servers.clear(); // Clear any existing servers
    
    // Validate config file exists and is readable
    if (!Utils::fileExists(config_file)) {
        std::cerr << "Error: Configuration file '" << config_file << "' does not exist" << std::endl;
        return false;
    }
    
    if (!Utils::isReadable(config_file)) {
        std::cerr << "Error: Configuration file '" << config_file << "' is not readable" << std::endl;
        return false;
    }
    
    // Read entire file using allowed functions
    std::string content = Utils::readFile(config_file);
    if (content.empty()) {
        std::cerr << "Error: Configuration file '" << config_file << "' is empty or could not be read" << std::endl;
        return false;
    }

    std::vector<std::string> lines = splitIntoLines(content);
    
    // First pass: Check for basic syntax issues
    if (!validateBasicSyntax(lines)) {
        return false;
    }
    
    for (size_t i = 0; i < lines.size(); ++i) {
        std::string line = trim(lines[i]);
        if (line.empty() || line[0] == '#') {
            continue;
        }

        if (line == "server {" || (line.find("server") == 0 && line.find("{") != std::string::npos)) {
            ServerConfig server;
            size_t new_index = parseServerBlock(lines, i + 1, server);
            
            // Always advance index, even if parsing failed
            if (new_index > i + 1) {
                i = new_index - 1; // -1 because loop will increment
            } else {
                // Server block parsing failed completely
                std::cerr << "Error: Failed to parse server block starting at line " << i + 1 << std::endl;
                return false;
            }
            
            // Only add server if it has valid port (0 means parsing failed or invalid)
            if (server.port > 0) {
                _servers.push_back(server);
            } else {
                std::cerr << "Error: Server block has invalid or missing port" << std::endl;
                return false;
            }
        } else if (line.find("server") == 0) {
            // Server directive without proper opening brace
            std::cerr << "Error: 'server' directive without opening brace at line " << i + 1 << std::endl;
            return false;
        }
    }

    return !_servers.empty();
}

size_t ConfigParser::parseServerBlock(const std::vector<std::string>& lines, size_t start_index, ServerConfig& server) {
    int brace_count = 1;
    size_t i = start_index;
    size_t max_iterations = lines.size() * 2; // Safety limit
    size_t iterations = 0;

    while (i < lines.size() && brace_count > 0 && iterations < max_iterations) {
        iterations++;
        std::string line = trim(lines[i]);
        if (line.empty() || line[0] == '#') {
            i++;
            continue;
        }

        if (line == "}") {
            brace_count--;
            i++;
            continue;
        }
        
        if (line.find("location") != std::string::npos && line.find("{") != std::string::npos) {
            size_t start = line.find_first_of(" \t");
            size_t end = line.find_last_of(" \t{");
            if (start != std::string::npos && end != std::string::npos && end > start) {
                std::string location = trim(line.substr(start, end - start));
                RouteConfig route;
                size_t new_i = parseLocationBlock(lines, i + 1, route);
                if (new_i > i + 1) {
                    i = new_i;
                } else {
                    // Location parsing failed - this is an error
                    return start_index; // Return original index to indicate failure
                }
                server.routes[location] = route;
                if (route.max_body_size == 0) {
                    route.max_body_size = server.max_body_size; // Inherit from server if not set
                }
                continue;
            }
        }

        std::vector<std::string> tokens = split(line, ' ');
        if (tokens.size() < 2) {
            i++;
            continue;
        }

        std::string directive = tokens[0];
        std::string value = tokens[1];
        
        // Check if directive line ends with semicolon (basic syntax validation)
        bool has_semicolon = false;
        if (!line.empty() && line[line.length() - 1] == ';') {
            has_semicolon = true;
        }
        
        // Some directives require semicolons - be strict about this (case insensitive)
        if (Utils::equalsIgnoreCase(directive, "listen") || Utils::equalsIgnoreCase(directive, "server_name") || 
            Utils::equalsIgnoreCase(directive, "root") || Utils::equalsIgnoreCase(directive, "index") || 
            Utils::equalsIgnoreCase(directive, "autoindex") || Utils::equalsIgnoreCase(directive, "client_max_body_size")) {
            if (!has_semicolon) {
                // Strict validation - return failure
                return start_index;
            }
        }
        
        // Remove semicolon if present
        if (!value.empty() && value[value.length() - 1] == ';') {
            value = value.substr(0, value.length() - 1);
        }

        if (Utils::equalsIgnoreCase(directive, "listen")) {
            // Static webserver: only ONE port allowed per server block
            if (server.port != 0) {
                std::cerr << "Error: Multiple 'listen' directives in server block (line around " << i << ")" << std::endl;
                std::cerr << "       Static webservers support only ONE port per server block" << std::endl;
                return start_index; // Signal parsing failure
            }
            
            int port = Utils::toInt(value);
            if (Utils::isValidPort(port)) {
                server.port = port;
            } else {
                std::cerr << "Error: Invalid port number '" << value << "' (line around " << i << ")" << std::endl;
                std::cerr << "       Port must be between 1 and 65535" << std::endl;
                return start_index; // Signal parsing failure
            }
        }
        else if (Utils::equalsIgnoreCase(directive, "server_name")) {
            server.server_name = value;
        }
        else if (Utils::equalsIgnoreCase(directive, "root")) {
            server.root = value;
        }
        else if (Utils::equalsIgnoreCase(directive, "index")) {
            server.index = value;
        }
        else if (Utils::equalsIgnoreCase(directive, "autoindex")) {
            server.autoindex = (value == "on");
        }
        else if (Utils::equalsIgnoreCase(directive, "client_max_body_size")) {
            server.max_body_size = Utils::toSizeT(value);
        }
        else if (Utils::equalsIgnoreCase(directive, "error_page")) {
            if (tokens.size() >= 3) {
                int status = Utils::toInt(value);
                std::string page = tokens[2];
                if (!page.empty() && page[page.length() - 1] == ';') {
                    page = page.substr(0, page.length() - 1);
                }
                server.error_pages[status] = page;
            }
        }
        i++;
    }
    
    // Check if braces are balanced
    if (brace_count != 0) {
        std::cerr << "Error: Unbalanced braces in server block" << std::endl;
        return start_index; // Return original index to indicate incomplete server block
    }

    // Set default port if none specified
    if (server.port == 0) {
        server.port = 8080;
        std::cout << "Warning: No 'listen' directive found, defaulting to port 8080" << std::endl;
    }
    
    // Set default server name if none specified
    if (server.server_name.empty()) {
        server.server_name = "localhost";
    }
    
    // Ensure at least one route exists
    if (server.routes.empty()) {
        RouteConfig default_route;
        default_route.allowed_methods.push_back("GET");
        default_route.root_directory = server.root;
        default_route.directory_listing = server.autoindex;
        server.routes["/"] = default_route;
    }
    
    // IMPORTANT: Apply inheritance and normalize all paths
    // This ensures routes inherit from server config and have proper defaults
    applyInheritanceAndNormalize(server);
    
    return i;
}

size_t ConfigParser::parseLocationBlock(const std::vector<std::string>& lines, size_t start_index, RouteConfig& route) {
    int brace_count = 1;
    size_t i = start_index;
    size_t max_iterations = lines.size(); // Safety limit
    size_t iterations = 0;
    route.max_body_size = 0; // 0 means inherit from server by default

    while (i < lines.size() && brace_count > 0 && iterations < max_iterations) {
        iterations++;
        std::string line = trim(lines[i]);
        if (line.empty() || line[0] == '#') {
            i++;
            continue;
        }

        if (line.find("{") != std::string::npos) {
            brace_count++;
            i++;
            continue;
        }
        if (line == "}") {
            brace_count--;
            i++;
            continue;
        }

        std::vector<std::string> tokens = split(line, ' ');
        if (tokens.size() < 2) {
            i++;
            continue;
        }

        std::string directive = tokens[0];
        std::string value = tokens[1];
        
        // Remove semicolon if present
        if (!value.empty() && value[value.length() - 1] == ';') {
            value = value.substr(0, value.length() - 1);
        }

        if (Utils::equalsIgnoreCase(directive, "allow_methods")) {
            for (size_t j = 1; j < tokens.size(); ++j) {
                std::string method = tokens[j];
                if (!method.empty() && method[method.length() - 1] == ';') {
                    method = method.substr(0, method.length() - 1);
                }
                // Normalize method to uppercase
                for (size_t k = 0; k < method.length(); ++k) {
                    method[k] = std::toupper((unsigned char)method[k]);
                }
                if (!method.empty() && isValidMethod(method)) {
                    route.allowed_methods.push_back(method);
                }
            }
        }
        else if (Utils::equalsIgnoreCase(directive, "root")) {
            route.root_directory = value;
        }
        else if (Utils::equalsIgnoreCase(directive, "index")) {
            route.index_file = value;
        }
        else if (Utils::equalsIgnoreCase(directive, "autoindex")) {
            route.directory_listing = (value == "on");
        }
        else if (Utils::equalsIgnoreCase(directive, "upload_pass")) {
            route.upload_path = value;
        }
        else if (Utils::equalsIgnoreCase(directive, "cgi_extension")) {
            // Parse ALL extensions from the line
            for (size_t j = 1; j < tokens.size(); ++j) {
                std::string ext = tokens[j];
                // Remove semicolon if present
                if (!ext.empty() && ext[ext.length() - 1] == ';') {
                    ext = ext.substr(0, ext.length() - 1);
                }
                // Validate extension starts with '.'
                if (!ext.empty() && ext[0] == '.') {
                    route.cgi_extensions.push_back(ext);
                }
            }
        }
        else if (Utils::equalsIgnoreCase(directive, "return")) {
            // Format: return <status_code> <uri>;
            // Example: return 301 /new-location;
            if (tokens.size() >= 3) {
                // Parse status code
                std::string status_str = tokens[1];
                int status_code = Utils::toInt(status_str);
                
                // Validate it's a redirect status code (3xx)
                if (status_code >= 300 && status_code < 400) {
                    route.redirect_code = status_code;
                    
                    // Parse redirect URL (could be relative or absolute)
                    route.redirect_url = tokens[2];
                    
                    // Remove trailing semicolon if present
                    if (!route.redirect_url.empty() && route.redirect_url[route.redirect_url.length() - 1] == ';') {
                        route.redirect_url = route.redirect_url.substr(0, route.redirect_url.length() - 1);
                    }
                } else {
                    std::cerr << "Warning: Invalid redirect status code " << status_code 
                              << " (must be 3xx). Ignoring return directive." << std::endl;
                }
            } else if (tokens.size() == 2) {
                // Old format compatibility: return <uri>; (default to 301)
                route.redirect_code = 301;
                route.redirect_url = tokens[1];
                if (!route.redirect_url.empty() && route.redirect_url[route.redirect_url.length() - 1] == ';') {
                    route.redirect_url = route.redirect_url.substr(0, route.redirect_url.length() - 1);
                }
            }
        }
        else if (Utils::equalsIgnoreCase(directive, "client_max_body_size")) {
            route.max_body_size = Utils::toSizeT(value);
        }
        i++;
    }
    
    // Check if braces are balanced
    if (brace_count != 0) {
        return start_index; // Return original index to indicate incomplete location block
    }
    
    // Set default methods if none specified
    if (route.allowed_methods.empty()) {
        route.allowed_methods.push_back("GET");
        std::cerr << "Warning: No methods specified for location, defaulting to GET" << std::endl;
    }
    
    if (!route.cgi_extensions.empty()) {
        bool has_post = false;
        for (size_t x = 0; x < route.allowed_methods.size(); ++x) {
            if (route.allowed_methods[x] == "POST") {
                has_post = true;
                break;
            }
        }
        if (!has_post) {
            std::cerr << "Warning: CGI route without POST method" << std::endl;
        }
    }
    return i;
}

bool ConfigParser::validateBasicSyntax(const std::vector<std::string>& lines) {
    int brace_count = 0;
    bool in_server_block = false;
    
    for (size_t i = 0; i < lines.size(); ++i) {
        std::string line = trim(lines[i]);
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Count braces
        for (size_t j = 0; j < line.length(); ++j) {
            if (line[j] == '{') {
                brace_count++;
                if (line.find("server") != std::string::npos) {
                    in_server_block = true;
                }
            } else if (line[j] == '}') {
                brace_count--;
                if (brace_count < 0) {
                    return false; // More closing braces than opening
                }
            }
        }
        
        // Check for server directive without opening brace
        if (line.find("server") == 0 && line.find("{") == std::string::npos && !in_server_block) {
            return false;
        }
    }
    
    // Check final brace balance
    return brace_count == 0;
}

const std::vector<ServerConfig>& ConfigParser::getServers() const {
    return _servers;
}

std::string ConfigParser::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

std::vector<std::string> ConfigParser::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == delimiter) {
            if (!token.empty()) {
                token = trim(token);
                if (!token.empty()) {
                    tokens.push_back(token);
                }
                token.clear();
            }
        } else {
            token += str[i];
        }
    }
    
    // Add last token if not empty
    if (!token.empty()) {
        token = trim(token);
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    
    return tokens;
}

std::vector<std::string> ConfigParser::splitIntoLines(const std::string& content) {
    std::vector<std::string> lines;
    std::string line;
    
    for (size_t i = 0; i < content.length(); ++i) {
        if (content[i] == '\n') {
            lines.push_back(line);
            line.clear();
        } else if (content[i] != '\r') {  // Skip carriage return
            line += content[i];
        }
    }
    
    // Add last line if it doesn't end with newline
    if (!line.empty()) {
        lines.push_back(line);
    }
    
    return lines;
}

bool ConfigParser::isValidMethod(const std::string& method) {
    std::string upper = method;
    for (size_t i = 0; i < upper.length(); ++i) {
        upper[i] = std::toupper(upper[i]);
    }
    
    return (upper == "GET" || upper == "POST" || upper == "DELETE" || 
            upper == "PUT" || upper == "HEAD" || upper == "OPTIONS");
}

// Apply inheritance from server config to all routes and normalize paths
// This implements Nginx-style inheritance:
// 1. Routes inherit root, index, autoindex, max_body_size from server if not specified
// 2. All paths are normalized (no trailing slashes in roots)
// 3. After this, HttpHandler can just use route.root_directory directly
void ConfigParser::applyInheritanceAndNormalize(ServerConfig& server) {
    // Set server-level defaults if not specified
    if (server.root.empty()) {
        server.root = "./www";
    }
    if (server.index.empty()) {
        server.index = "index.html";
    }
    if (server.max_body_size == 0) {
        server.max_body_size = 1048576; // 1MB default
    }
    
    // Normalize server root (remove trailing slash)
    if (!server.root.empty() && server.root[server.root.length() - 1] == '/') {
        server.root = server.root.substr(0, server.root.length() - 1);
    }
    
    // Apply inheritance to each route
    for (std::map<std::string, RouteConfig>::iterator it = server.routes.begin(); 
         it != server.routes.end(); ++it) {
        RouteConfig& route = it->second;
        
        // Inherit root from server if not specified
        if (route.root_directory.empty()) {
            route.root_directory = server.root;
        }
        
        // Normalize route root (remove trailing slash)
        if (!route.root_directory.empty() && 
            route.root_directory[route.root_directory.length() - 1] == '/') {
            route.root_directory = route.root_directory.substr(0, route.root_directory.length() - 1);
        }
        
        // Inherit index from server if not specified
        if (route.index_file.empty()) {
            route.index_file = server.index;
        }
        
        // Inherit max_body_size from server if not specified (or use route-specific)
        if (route.max_body_size == 0) {
            route.max_body_size = server.max_body_size;
        }
        
        // Note: directory_listing (autoindex) defaults to false in RouteConfig constructor
        // If it wasn't explicitly set in location block, it stays false
        // This matches Nginx behavior where autoindex is off by default
    }
}

// UNUSED FUNCTION - Commented out for now
/*
void ConfigParser::printConfig() const {
    for (size_t i = 0; i < _servers.size(); ++i) {
        const ServerConfig& server = _servers[i];
        std::cout << "Server " << i + 1 << ":" << std::endl;
        std::cout << "  Server name: " << server.server_name << std::endl;
        std::cout << "  Port: " << server.port << std::endl;
        std::cout << "  Root: " << server.root << std::endl;
        std::cout << "  Index: " << server.index << std::endl;
        std::cout << "  Max body size: " << server.max_body_size << std::endl;
        std::cout << "  Routes: " << server.routes.size() << std::endl;
    }
}
*/
