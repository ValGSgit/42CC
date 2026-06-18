#include "../includes/webserv.hpp"
#include "../includes/config/ConfigParser.hpp"
#include "../includes/http/HttpRequest.hpp"
#include "../includes/http/HttpResponse.hpp"
#include "../includes/cgi/CgiHandler.hpp"
#include "../includes/utils/Utils.hpp"
#include "../includes/server/ServerManager.hpp"

volatile sig_atomic_t g_running = 1;
ServerManager* g_server_manager = NULL;


void signalHandler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        g_running = 0;
    } else if (signal == SIGCHLD) {
        // Reap zombie child processes (from CGI)
        int status;
        while (waitpid(-1, &status, WNOHANG) > 0) { //async-safe C function
            // Child reaped successfully
        }
    }
}

int main(int argc, char **argv) {
    // Use default config if not provided
    std::string config_file = (argc > 1) ? argv[1] : "webserv.conf";
    
    if (argc > 2) {
        std::cerr << "Usage: " << argv[0] << " [config_file]" << std::endl;
        std::cerr << "  If no config file is provided, webserv.conf will be used by default" << std::endl;
        return 1;
    } else if (!Utils::fileExists(config_file) || !Utils::isReadable(config_file)) {
        std::cerr << "Config file didnt have permissions u silly!" << std::endl;
        return 1;
    }

    // Setup signal handlers
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGCHLD, signalHandler); // Prevent zombie CGI processes

    std::cout << "🌐 WebServ - HTTP Server with epoll" << std::endl;
    std::cout << "📝 Config file: " << config_file << std::endl;
    
    ServerManager manager;
    g_server_manager = &manager;
    
    if (!manager.initialize(config_file)) {
        std::cerr << "❌ Failed to initialize server" << std::endl;
        return 1;
    }
    
    manager.printServerStatus();
    manager.printServerSockets();
    
    manager.run();
    g_server_manager = NULL;
    
    return 0;
}
