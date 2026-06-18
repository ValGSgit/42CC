#include "../../includes/cgi/CgiHandler.hpp"
#include "../../includes/http/HttpRequest.hpp"
#include "../../includes/http/HttpResponse.hpp"
#include "../../includes/http/HttpStatusCodes.hpp"
#include "../../includes/utils/Utils.hpp"
#include "../../includes/server/ServerManager.hpp"
#include <algorithm>

extern ServerManager* g_server_manager;

CgiHandler::CgiHandler() : _timeout(CGI_TIMEOUT) {}

CgiHandler::~CgiHandler() {}

HttpResponse CgiHandler::executeCgi(const HttpRequest& request, const std::string& script_path, const std::vector<std::string>& good_ext) {
    if (!Utils::fileExists(script_path) || !Utils::isReadable(script_path)) {
        return HttpResponse::errorResponse(HTTP_NOT_FOUND);
    }
    
    if (!Utils::isExecutable(script_path)) {
        return HttpResponse::errorResponse(HTTP_FORBIDDEN, "The requested script had no execute permissions");
    }
    // Find CGI executable
    std::string extension = Utils::getFileExtension(script_path);
    std::string cgi_executable = findCgiExecutable(extension);
    
    if (cgi_executable.empty()) {
        return HttpResponse::errorResponse(HTTP_NOT_IMPLEMENTED);
    }
    
    if (std::find(good_ext.begin(), good_ext.end(), extension) == good_ext.end()) {
        return HttpResponse::errorResponse(HTTP_FORBIDDEN, "The file extension was not declared in the config :)");
    }
    // Create pipes for communication
    int stdin_pipe[2], stdout_pipe[2];
    if (pipe(stdin_pipe) == -1 || pipe(stdout_pipe) == -1) {
        std::cerr << "pipe failed\n";;
        return HttpResponse::errorResponse(HTTP_INTERNAL_SERVER_ERROR);
    }
    
    // Setup environment BEFORE forking
    setupEnvironment(request, script_path);
    
    // Convert environment map to array for execve
    std::vector<std::string> env_strings;
    std::vector<char*> env_array;
    
    for (std::map<std::string, std::string>::const_iterator it = _env.begin();
         it != _env.end(); ++it) {
        env_strings.push_back(it->first + "=" + it->second);
    }
    
    for (size_t i = 0; i < env_strings.size(); ++i) {
        env_array.push_back(const_cast<char*>(env_strings[i].c_str()));
    }
    env_array.push_back(NULL);
    
    // Fork process
    pid_t pid = fork();
    if (pid == -1) {
        std::cerr << "fork failed\n";
        close(stdin_pipe[0]); close(stdin_pipe[1]);
        close(stdout_pipe[0]); close(stdout_pipe[1]);
        return HttpResponse::errorResponse(HTTP_INTERNAL_SERVER_ERROR);
    }
    
    if (pid == 0) {
        // Child process
        close(stdin_pipe[1]);   // Close write end of stdin pipe
        close(stdout_pipe[0]);  // Close read end of stdout pipe
        
        // Redirect stdin and stdout
        dup2(stdin_pipe[0], STDIN_FILENO);
        dup2(stdout_pipe[1], STDOUT_FILENO);
        
        close(stdin_pipe[0]);
        close(stdout_pipe[1]);
        
        // Execute CGI script with environment
        const char* args[] = {cgi_executable.c_str(), script_path.c_str(), NULL};
        execve(cgi_executable.c_str(), const_cast<char* const*>(args), &env_array[0]);
        
        // If we get here, exec failed
        return HttpResponse::errorResponse(HTTP_INTERNAL_SERVER_ERROR, "Umm, akchually execve failed");
    }

    close(stdin_pipe[0]); 
    close(stdout_pipe[1]);
    
    // Write request body to CGI stdin
    // NOTE: Per subject requirements, chunked requests are already unchunked
    // by HttpRequest parser - CGI receives plain body and EOF marks end
    const std::vector<char>& body = request.getBody();
    if (!body.empty()) {
        ssize_t written = write(stdin_pipe[1], &body[0], body.size());
        if (written < 0 || static_cast<size_t>(written) != body.size()) {
            std::cerr << "Warning: Failed to write complete body to CGI\\n";
        }
    }
    close(stdin_pipe[1]); // Close stdin pipe to send EOF to CGI
    
    // Read CGI output using epoll for timeout handling
    std::string output;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    
    // Create epoll instance for CGI pipe
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        std::cerr << "epoll_create1 failed\n";
        close(stdout_pipe[0]);
        kill(pid, SIGTERM);
        waitpid(pid, NULL, 0);
        return HttpResponse::errorResponse(HTTP_INTERNAL_SERVER_ERROR);
    }
    
    // Add stdout pipe to epoll
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN;
    ev.data.fd = stdout_pipe[0];
    
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, stdout_pipe[0], &ev) == -1) {
        std::cerr << "epoll_ctl failed1\n";
        close(epoll_fd);
        close(stdout_pipe[0]);
        kill(pid, SIGTERM);
        waitpid(pid, NULL, 0);
        return HttpResponse::errorResponse(HTTP_INTERNAL_SERVER_ERROR);
    }
    
    // Read with timeout using epoll_wait
    time_t start_time = time(NULL);
    while (true) {
        // Calculate remaining timeout
        time_t elapsed = time(NULL) - start_time;
        int timeout_ms = (_timeout - elapsed) * 1000;
        
        if (timeout_ms <= 0) {
            // Timeout exceeded
            std::cerr << "CGI timeout: script took longer than " << _timeout << " seconds" << std::endl;
            kill(pid, SIGTERM);
            break;
        }
        
        struct epoll_event events[1];
        int nfds = epoll_wait(epoll_fd, events, 1, timeout_ms);
        
        if (nfds == -1) {
            // Per subject: no errno check after I/O operations
            // epoll_wait failure - break and handle cleanup
            std::cerr << "epoll_wait failed\n";
            break;
        } else if (nfds == 0) {
            // Timeout
            std::cerr << "CGI timeout: no data received within " << _timeout << " seconds" << std::endl;
            kill(pid, SIGTERM);
            break;
        }
        
        // Data available to read
        bytes_read = read(stdout_pipe[0], buffer, sizeof(buffer) - 1);
        if (bytes_read == 0) {
            // EOF reached - CGI script finished
            break;
        }
        if (bytes_read < 0) {
            // Read error - no errno check per subject
            // For non-blocking reads, this could be EAGAIN/EWOULDBLOCK
            // but we can't check errno, so just break
            std::cerr << "read failed\n";
            break;
        }
        buffer[bytes_read] = '\0';
        output += std::string(buffer, bytes_read);
    }
    
    close(epoll_fd);
    close(stdout_pipe[0]);
    
    // Wait for child process
    int status;
    waitpid(pid, &status, 0);
    
    if (output.empty()) {
        return HttpResponse::errorResponse(HTTP_INTERNAL_SERVER_ERROR);
    }
    
    return parseCgiOutput(output);
}

void CgiHandler::setupEnvironment(const HttpRequest& request, const std::string& script_path) {
    // Clear environment
    _env.clear();
    
    // CGI environment variables - SECURITY FIX: Sanitize all user inputs
    _env["REQUEST_METHOD"] = request.methodToString();
    _env["REQUEST_URI"] = Utils::sanitizeForShell(request.getUri());
    _env["QUERY_STRING"] = Utils::sanitizeForShell(request.getQueryString());
    _env["SERVER_NAME"] = "localhost";
    _env["SERVER_PORT"] = "8080";
    _env["SERVER_PROTOCOL"] = "HTTP/1.1";
    _env["SERVER_SOFTWARE"] = "WebServ/1.0";
    _env["GATEWAY_INTERFACE"] = "CGI/1.1";
    _env["SCRIPT_NAME"] = Utils::sanitizeForShell(request.getUri());
    _env["SCRIPT_FILENAME"] = script_path;
    _env["PATH_INFO"] = "";
    _env["PATH_TRANSLATED"] = "";
    _env["DOCUMENT_ROOT"] = "./www";
    _env["REDIRECT_STATUS"] = "200";  // Required for PHP CGI
    
    // Content related
    std::string content_type = request.getHeader("Content-Type");
    std::string content_length = request.getHeader("Content-Length");
    
    if (!content_type.empty()) {
        _env["CONTENT_TYPE"] = Utils::sanitizeForShell(content_type);
    }
    if (!content_length.empty()) {
        _env["CONTENT_LENGTH"] = Utils::sanitizeForShell(content_length);
    } else {
        _env["CONTENT_LENGTH"] = Utils::toString(request.getBody().size());
    }
    
    // HTTP headers (convert to HTTP_HEADER_NAME format)
    // SECURITY FIX: Sanitize all header values to prevent injection
    const std::map<std::string, std::string>& headers = request.getHeaders();
    for (std::map<std::string, std::string>::const_iterator it = headers.begin();
         it != headers.end(); ++it) {
        std::string env_name = "HTTP_" + Utils::toUpperCase(it->first);
        // Replace hyphens with underscores
        for (size_t i = 0; i < env_name.length(); ++i) {
            if (env_name[i] == '-') {
                env_name[i] = '_';
            }
        }
        _env[env_name] = Utils::sanitizeForShell(it->second);
    }
}

std::string CgiHandler::findCgiExecutable(const std::string& extension) {
    if (extension == ".php") {
        if (Utils::fileExists("/usr/bin/php-cgi")) return "/usr/bin/php-cgi";
        if (Utils::fileExists("/usr/bin/php")) return "/usr/bin/php";
        if (Utils::fileExists("/usr/local/bin/php-cgi")) return "/usr/local/bin/php-cgi";
        if (Utils::fileExists("/usr/local/bin/php")) return "/usr/local/bin/php";
    } else if (extension == ".py") {
        if (Utils::fileExists("/usr/bin/python3")) return "/usr/bin/python3";
        if (Utils::fileExists("/usr/bin/python")) return "/usr/bin/python";
        if (Utils::fileExists("/usr/local/bin/python3")) return "/usr/local/bin/python3";
    } else if (extension == ".pl") {
        if (Utils::fileExists("/usr/bin/perl")) return "/usr/bin/perl";
        if (Utils::fileExists("/usr/local/bin/perl")) return "/usr/local/bin/perl";
    } else if (extension == ".rb") {
        if (Utils::fileExists("/usr/bin/ruby")) return "/usr/bin/ruby";
        if (Utils::fileExists("/usr/local/bin/ruby")) return "/usr/local/bin/ruby";
    } else if (extension == ".sh") {
        if (Utils::fileExists("/bin/bash")) return "/bin/bash";
        if (Utils::fileExists("/usr/bin/bash")) return "/usr/bin/bash";
    }
    
    return "";
}

HttpResponse CgiHandler::parseCgiOutput(const std::string& output) {
    HttpResponse response;
    
    // Find the separator between headers and body
    size_t header_end = output.find("\r\n\r\n");
    if (header_end == std::string::npos) {
        header_end = output.find("\n\n");
        if (header_end == std::string::npos) {
            // No headers, treat everything as body
            response.setStatus(HTTP_OK);
            response.setContentType("text/html");
            response.setBody(output);
            return response;
        }
    }
    
    // Parse headers
    std::string headers = output.substr(0, header_end);
    std::string body = output.substr(header_end + (output.find("\r\n\r\n") != std::string::npos ? 4 : 2));
    
    std::vector<std::string> header_lines = Utils::split(headers, '\n');
    bool status_set = false;
    
    for (size_t i = 0; i < header_lines.size(); ++i) {
        std::string line = Utils::trim(header_lines[i]);
        if (line.empty()) continue;
        
        size_t colon_pos = line.find(':');
        if (colon_pos != std::string::npos) {
            std::string key = Utils::trim(line.substr(0, colon_pos));
            std::string value = Utils::trim(line.substr(colon_pos + 1));
            
            if (Utils::toLowerCase(key) == "status") {
                int status_code = Utils::toInt(value);
                if (status_code > 0) {
                    response.setStatus(status_code);
                    status_set = true;
                }
            } else {
                response.setHeader(key, value);
            }
        }
    }
    
    if (!status_set) {
        response.setStatus(HTTP_OK);
    }
    
    response.setBody(body);
    return response;
}

void CgiHandler::setTimeout(int seconds) {
    _timeout = seconds;
}

bool isCgiRequest(const std::string& uri, const RouteConfig& route) {
    // Check if URI matches any CGI extension
    for (size_t i = 0; i < route.cgi_extensions.size(); ++i) {
        const std::string& ext = route.cgi_extensions[i];
        if (uri.length() >= ext.length()) {
            if (uri.substr(uri.length() - ext.length()) == ext) {
                return true;
            }
        }
    }
    return false;
}