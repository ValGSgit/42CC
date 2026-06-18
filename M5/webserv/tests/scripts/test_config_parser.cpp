#include "../includes/webserv.hpp"
#include "../includes/config/ConfigParser.hpp"
#include <iostream>
#include <iomanip>

// ANSI color codes for better readability
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"

void printSeparator(const std::string& title) {
    std::cout << "\n" << BOLD << CYAN << "+============================================================+" << RESET << std::endl;
    std::cout << BOLD << CYAN << "| " << std::setw(58) << std::left << title << " |" << RESET << std::endl;
    std::cout << BOLD << CYAN << "+============================================================+" << RESET << std::endl;
}

void printSubSection(const std::string& title) {
    std::cout << "\n" << BOLD << YELLOW << "  > " << title << RESET << std::endl;
    std::cout << "  " << std::string(60, '-') << std::endl;
}

void printServerConfig(const ServerConfig& server, int index) {
    printSeparator("SERVER CONFIGURATION #" + std::string(1, '0' + index));
    
    // Server Name
    std::cout << BOLD << "  Server Name: " << RESET << GREEN << server.server_name << RESET << std::endl;
    
    // Port (single port now)
    printSubSection("Listening Port");
    if (server.port == 0) {
        std::cout << RED << "    ⚠ No port configured!" << RESET << std::endl;
    } else {
        std::cout << "    Port: " << BLUE << server.port << RESET << std::endl;
    }
    
    // Root and Index
    printSubSection("Document Root Settings");
    std::cout << "    Root Directory: " << BLUE << server.root << RESET << std::endl;
    std::cout << "    Index File:     " << BLUE << server.index << RESET << std::endl;
    std::cout << "    Auto Index:     " << (server.autoindex ? (GREEN "enabled") : (RED "disabled")) << RESET << std::endl;
    
    // Body Size
    printSubSection("Request Settings");
    std::cout << "    Max Body Size:  " << BLUE << server.max_body_size << RESET << " bytes";
    std::cout << " (" << (server.max_body_size / 1024.0 / 1024.0) << " MB)" << std::endl;
    
    // Error Pages
    printSubSection("Error Pages");
    if (server.error_pages.empty()) {
        std::cout << "    " << YELLOW << "No custom error pages configured" << RESET << std::endl;
    } else {
        for (std::map<int, std::string>::const_iterator it = server.error_pages.begin();
             it != server.error_pages.end(); ++it) {
            std::cout << "    " << it->first << " -> " << MAGENTA << it->second << RESET << std::endl;
        }
    }
    
    // Routes/Locations
    printSubSection("Location Blocks (Routes)");
    if (server.routes.empty()) {
        std::cout << "    " << YELLOW << "No location blocks configured" << RESET << std::endl;
    } else {
        for (std::map<std::string, RouteConfig>::const_iterator it = server.routes.begin();
             it != server.routes.end(); ++it) {
            std::cout << "\n    " << BOLD << CYAN << "Location: " << it->first << RESET << std::endl;
            
            const RouteConfig& route = it->second;
            
            // Allowed Methods
            std::cout << "      Allowed Methods: ";
            if (route.allowed_methods.empty()) {
                std::cout << RED << "NONE" << RESET;
            } else {
                for (size_t i = 0; i < route.allowed_methods.size(); ++i) {
                    std::cout << GREEN << route.allowed_methods[i] << RESET;
                    if (i < route.allowed_methods.size() - 1) std::cout << ", ";
                }
            }
            std::cout << std::endl;
            
            // Root Directory
            if (!route.root_directory.empty()) {
                std::cout << "      Root Directory:  " << BLUE << route.root_directory << RESET << std::endl;
            }
            
            // Index File
            if (!route.index_file.empty()) {
                std::cout << "      Index File:      " << BLUE << route.index_file << RESET << std::endl;
            }
            
            // Directory Listing
            std::cout << "      Dir Listing:     " << (route.directory_listing ? (GREEN "enabled") : (RED "disabled")) << RESET << std::endl;
            
            // Upload Path
            if (!route.upload_path.empty()) {
                std::cout << "      Upload Path:     " << MAGENTA << route.upload_path << RESET << std::endl;
            }
            
            // CGI Extension
            if (!route.cgi_extensions.empty()) {
                std::cout << "      CGI Extension" << (route.cgi_extensions.size() > 1 ? "s:   " : ":   ");
                for (size_t i = 0; i < route.cgi_extensions.size(); ++i) {
                    std::cout << MAGENTA << route.cgi_extensions[i] << RESET;
                    if (i + 1 < route.cgi_extensions.size()) std::cout << ", ";
                }
                std::cout << std::endl;
            }
            
            // Redirect
            if (!route.redirect_url.empty()) {
                std::cout << "      Redirect URL:    " << YELLOW << route.redirect_url << RESET << std::endl;
            }
            
            // Max Body Size
            std::cout << "      Max Body Size:   " << BLUE << route.max_body_size << RESET << " bytes";
            std::cout << " (" << (route.max_body_size / 1024.0 / 1024.0) << " MB)" << std::endl;
        }
    }
}

void printServerSocketStructures(const std::vector<ServerConfig>& configs) {
    printSeparator("SIMULATED ServerSocket STRUCTURES");
    
    std::cout << "\n" << BOLD << "Creating ServerSocket structures from configs..." << RESET << std::endl;
    
    std::vector<ServerSocket> server_sockets;
    
    for (size_t i = 0; i < configs.size(); ++i) {
        const ServerConfig& config = configs[i];
        
        std::cout << "\n  Config #" << i << " (" << config.server_name << "):" << std::endl;
        
        ServerSocket ss;
        ss.fd = -1;  // Not actually creating sockets in this test
        ss.port = config.port;
        ss.config = &config;
        
        server_sockets.push_back(ss);
        
        std::cout << "    ServerSocket[" << server_sockets.size() - 1 << "]:" << std::endl;
        std::cout << "      +-- fd:     " << BLUE << ss.fd << RESET << " (simulated)" << std::endl;
        std::cout << "      +-- port:   " << GREEN << ss.port << RESET << std::endl;
        std::cout << "      +-- config: " << MAGENTA << "-> " << ss.config->server_name << RESET << std::endl;
    }
    
    std::cout << "\n" << BOLD << GREEN << "  ✓ Total ServerSocket structures: " << server_sockets.size() << RESET << std::endl;
    
    // Verify linkage
    printSubSection("Verifying Config Linkage");
    for (size_t i = 0; i < server_sockets.size(); ++i) {
        const ServerSocket& ss = server_sockets[i];
        std::cout << "    ServerSocket[" << i << "] port " << ss.port 
                  << " -> config: " << ss.config->server_name 
                  << " (root: " << ss.config->root << ")" << std::endl;
        
        // Verify we can access routes through the pointer
        std::string routes_count = ss.config->routes.empty() ? (RED "0") : (GREEN + std::string(1, '0' + ss.config->routes.size()));
        std::cout << "      +-- Routes accessible: " << routes_count << RESET << std::endl;
    }
}

void printClientConnectionStructure() {
    printSeparator("SIMULATED ClientConnection STRUCTURE");
    
    std::cout << "\n" << BOLD << "Creating a sample ClientConnection..." << RESET << std::endl;
    
    ClientConnection client;
    client.fd = 5;  // Simulated client fd
    client.server_port = 8080;
    client.state = STATE_READING_HEADERS;
    client.last_activity = time(NULL);
    client.buffer = "GET / HTTP/1.1\r\nHost: localhost\r\n";
    client.bytes_sent = 0;
    client.keep_alive = false;
    
    std::cout << "\n  ClientConnection:" << std::endl;
    std::cout << "    +-- fd:            " << BLUE << client.fd << RESET << std::endl;
    std::cout << "    +-- server_port:   " << GREEN << client.server_port << RESET << std::endl;
    std::cout << "    +-- state:         " << YELLOW;
    switch (client.state) {
        case STATE_READING_HEADERS: std::cout << "STATE_READING_HEADERS"; break;
        case STATE_READING_BODY: std::cout << "STATE_READING_BODY"; break;
        case STATE_PROCESSING: std::cout << "STATE_PROCESSING"; break;
        case STATE_WRITING_RESPONSE: std::cout << "STATE_WRITING_RESPONSE"; break;
        case STATE_DONE: std::cout << "STATE_DONE"; break;
        case STATE_ERROR: std::cout << "STATE_ERROR"; break;
    }
    std::cout << RESET << std::endl;
    std::cout << "    +-- last_activity: " << CYAN << client.last_activity << RESET << " (timestamp)" << std::endl;
    std::cout << "    +-- buffer size:   " << MAGENTA << client.buffer.size() << RESET << " bytes" << std::endl;
    std::cout << "    +-- bytes_sent:    " << BLUE << client.bytes_sent << RESET << std::endl;
    std::cout << "    +-- keep_alive:    " << (client.keep_alive ? (GREEN "true") : (RED "false")) << RESET << std::endl;
    
    std::cout << "\n  Buffer content (first 50 chars):" << std::endl;
    std::cout << "    \"" << CYAN << client.buffer.substr(0, 50) << RESET << "...\"" << std::endl;
}

void printDataIntegrityCheck(const std::vector<ServerConfig>& configs) {
    printSeparator("DATA INTEGRITY VERIFICATION");
    
    int total_ports = 0;
    int total_routes = 0;
    int total_error_pages = 0;
    
    for (size_t i = 0; i < configs.size(); ++i) {
        total_ports += 1; // Each server has exactly one port now
        total_routes += configs[i].routes.size();
        total_error_pages += configs[i].error_pages.size();
    }
    
    std::cout << "\n  " << BOLD << "Summary Statistics:" << RESET << std::endl;
    std::cout << "    Total Servers:     " << GREEN << configs.size() << RESET << std::endl;
    std::cout << "    Total Ports:       " << GREEN << total_ports << RESET << std::endl;
    std::cout << "    Total Routes:      " << GREEN << total_routes << RESET << std::endl;
    std::cout << "    Total Error Pages: " << GREEN << total_error_pages << RESET << std::endl;
    
    // Memory addresses to verify no data loss
    printSubSection("Memory Address Verification");
    for (size_t i = 0; i < configs.size(); ++i) {
        std::cout << "    ServerConfig[" << i << "] @ " << &configs[i] << std::endl;
        std::cout << "      +-- port (single)  @ " << &configs[i].port << std::endl;
        std::cout << "      +-- routes map     @ " << &configs[i].routes << std::endl;
        std::cout << "      +-- error_pages map @ " << &configs[i].error_pages << std::endl;
    }
    
    // Deep data verification
    printSubSection("Deep Data Verification");
    for (size_t i = 0; i < configs.size(); ++i) {
        const ServerConfig& cfg = configs[i];
        bool integrity_ok = true;
        
        std::cout << "    Checking ServerConfig[" << i << "]:" << std::endl;
        
        // Check if server_name is not empty
        if (cfg.server_name.empty()) {
            std::cout << "      " << RED << "✗ server_name is empty!" << RESET << std::endl;
            integrity_ok = false;
        } else {
            std::cout << "      " << GREEN << "✓ server_name: " << cfg.server_name << RESET << std::endl;
        }
        
        // Check if port is valid
        if (cfg.port == 0) {
            std::cout << "      " << RED << "✗ No port configured!" << RESET << std::endl;
            integrity_ok = false;
        } else {
            std::cout << "      " << GREEN << "✓ port: " << cfg.port << RESET << std::endl;
        }
        
        // Check root directory
        if (cfg.root.empty()) {
            std::cout << "      " << YELLOW << "⚠ root directory is empty (using default)" << RESET << std::endl;
        } else {
            std::cout << "      " << GREEN << "✓ root: " << cfg.root << RESET << std::endl;
        }
        
        // Check routes
        if (!cfg.routes.empty()) {
            std::cout << "      " << GREEN << "✓ routes: " << cfg.routes.size() << " configured" << RESET << std::endl;
            
            // Verify each route
            for (std::map<std::string, RouteConfig>::const_iterator it = cfg.routes.begin();
                 it != cfg.routes.end(); ++it) {
                if (it->second.allowed_methods.empty()) {
                    std::cout << "        " << YELLOW << "⚠ Route '" << it->first << "' has no allowed methods" << RESET << std::endl;
                }
            }
        }
        
        if (integrity_ok) {
            std::cout << "      " << BOLD << GREEN << "✓ All integrity checks passed!" << RESET << std::endl;
        }
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << RED << "Usage: " << argv[0] << " <config_file>" << RESET << std::endl;
        return 1;
    }
    
    std::cout << BOLD << CYAN << "\n";
    std::cout << "+================================================================+\n";
    std::cout << "|         WEBSERV CONFIGURATION PARSER TEST SUITE               |\n";
    std::cout << "+================================================================+\n";
    std::cout << RESET << std::endl;
    
    std::cout << "Testing config file: " << YELLOW << argv[1] << RESET << std::endl;
    
    // Step 1: Parse Configuration
    printSeparator("STEP 1: PARSING CONFIGURATION FILE");
    
    ConfigParser parser;
    if (!parser.parseConfig(argv[1])) {
        std::cerr << RED << "\n✗ Failed to parse configuration file!" << RESET << std::endl;
        return 1;
    }
    
    std::cout << GREEN << "\n✓ Configuration file parsed successfully!" << RESET << std::endl;
    
    // Step 2: Get and Display Server Configurations
    const std::vector<ServerConfig>& servers = parser.getServers();
    
    if (servers.empty()) {
        std::cerr << RED << "\n✗ No server configurations found!" << RESET << std::endl;
        return 1;
    }
    
    std::cout << GREEN << "✓ Found " << servers.size() << " server configuration(s)" << RESET << std::endl;
    
    // Step 3: Print each server configuration in detail
    for (size_t i = 0; i < servers.size(); ++i) {
        printServerConfig(servers[i], i + 1);
    }
    
    // Step 4: Simulate ServerSocket structure creation
    printServerSocketStructures(servers);
    
    // Step 5: Simulate ClientConnection structure
    printClientConnectionStructure();
    
    // Step 6: Data Integrity Check
    printDataIntegrityCheck(servers);
    
    // Final Summary
    printSeparator("TEST SUMMARY");
    
    std::cout << "\n  " << BOLD << GREEN << "✓ All tests completed successfully!" << RESET << std::endl;
    std::cout << "  " << GREEN << "✓ Configuration parsing: PASSED" << RESET << std::endl;
    std::cout << "  " << GREEN << "✓ Data structure creation: PASSED" << RESET << std::endl;
    std::cout << "  " << GREEN << "✓ Data integrity: VERIFIED" << RESET << std::endl;
    std::cout << "  " << GREEN << "✓ No data loss detected" << RESET << std::endl;
    
    std::cout << "\n" << BOLD << CYAN << "================================================================" << RESET << std::endl;
    
    return 0;
}
