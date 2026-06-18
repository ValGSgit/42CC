#include "../../includes/http/HttpResponse.hpp"
#include "../../includes/utils/Utils.hpp"

HttpResponse::HttpResponse() : _status(HTTP_OK), _headers_sent(false) {
    setDefaultHeaders();
}

HttpResponse::~HttpResponse() {}

void HttpResponse::setStatus(int status) {
    _status = status;
}

void HttpResponse::setHeader(const std::string& key, const std::string& value) {
    _headers[key] = value;
}

void HttpResponse::setBody(const std::string& body) {
    _body = body;
    setContentLength(_body.length());
}

void HttpResponse::removeBody() {
    // For HEAD requests: Keep Content-Length but clear body
    // Content-Length was already set when body was added
    _body = "";
}

void HttpResponse::appendBody(const std::string& data) {
    _body += data;
    setContentLength(_body.length());
}

int HttpResponse::getStatus() const {
    return _status;
}

const std::string& HttpResponse::getBody() const {
    return _body;
}

const std::string& HttpResponse::getResponseString() {
    if (_response_string.empty()) {
        buildResponseString();
    }
    return _response_string;
}

void HttpResponse::reset() {
    _status = HTTP_OK;
    _headers.clear();
#ifdef BONUS
    _set_cookie_headers.clear();
#endif
    _body.clear();
    _response_string.clear();
    _headers_sent = false;
    setDefaultHeaders();
}

void HttpResponse::setDefaultHeaders() {
    // SECURITY FIX: Remove version information from Server header
    _headers["Server"] = "WebServ";
    
    std::time_t time = std::time(NULL);
    char buffer[30];
    std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S", std::localtime(&time));
    _headers["Date"] = buffer;
    _headers["Connection"] = "keep-alive";
    
    // SECURITY FIX: Added security headers for browser protection
    // _headers["X-Content-Type-Options"] = "nosniff";
    // _headers["X-Frame-Options"] = "DENY";
    // _headers["X-XSS-Protection"] = "1; mode=block";
    // _headers["Referrer-Policy"] = "no-referrer";
}

void HttpResponse::setContentType(const std::string& content_type) {
    _headers["Content-Type"] = content_type;
}

void HttpResponse::setContentLength(size_t length) {
    _headers["Content-Length"] = Utils::toString(length);
}

void HttpResponse::setAllow(const std::string& methods) {
    _headers["Allow"] = methods;
}

#ifdef BONUS
/**
 * Sets a cookie in the HTTP response
 * Creates a Set-Cookie header with the specified parameters
 *
 * @param name Cookie name
 * @param value Cookie value
 * @param max_age Expiration time in seconds (-1 for session cookie)
 * @param path Cookie path (default: "/")
 * @param http_only If true, cookie is not accessible via JavaScript
 * @param secure If true, cookie only sent over HTTPS
 */
void HttpResponse::setCookie(const std::string& name, const std::string& value,
                              int max_age, const std::string& path,
                              bool http_only, bool secure) {
    std::string cookie = name + "=" + value;
    cookie += "; Path=" + path;

    if (max_age >= 0) {
        cookie += "; Max-Age=" + Utils::toString(max_age);
    }
    if (http_only) {
        cookie += "; HttpOnly";
    }
    if (secure) {
        cookie += "; Secure";
    }

    _set_cookie_headers.push_back(cookie);
}

/**
 * Clears a cookie by setting it with Max-Age=0
 * This tells the browser to delete the cookie
 *
 * @param name Cookie name to clear
 */
void HttpResponse::clearCookie(const std::string& name) {
    setCookie(name, "", 0);
}
#endif

std::string HttpResponse::statusToString(int status) const {
    return Utils::toString((int)status) + " " + Utils::getStatusMessage((int)status);
}

void HttpResponse::buildResponseString() {
    _response_string = "HTTP/1.1 " + statusToString(_status) + "\r\n";

    for (std::map<std::string, std::string>::const_iterator it = _headers.begin();
         it != _headers.end(); ++it) {
        _response_string += it->first + ": " + it->second + "\r\n";
    }

#ifdef BONUS
    // Add all Set-Cookie headers (multiple are allowed)
    for (size_t i = 0; i < _set_cookie_headers.size(); ++i) {
        _response_string += "Set-Cookie: " + _set_cookie_headers[i] + "\r\n";
    }
#endif

    _response_string += "\r\n" + _body;
}

HttpResponse HttpResponse::messageResponse(int status, const std::string& title, const std::string& message) {
    HttpResponse response;
    response.setStatus(status);
    response.setContentType("text/html");
    
    std::string body = "<!DOCTYPE html><html><head><title>" + title + "</title></head><body>";
    body += "<h1>" + title + "</h1>";
    if (!message.empty()) {
        body += "<p>" + message + "</p>";
    }
    body += "</body></html>";
    
    response.setBody(body);
    return response;
}

HttpResponse HttpResponse::errorResponse(int status, const std::string& message) {
    HttpResponse response;
    response.setStatus(status);
    response.setContentType("text/html");
    
    std::string body = "<!DOCTYPE html><html><head><title>Error " + 
                      Utils::toString((int)status) + "</title></head><body>";
    body += "<h1>Error " + Utils::toString((int)status) + "</h1>";
    if (!message.empty()) {
        body += "<p>" + message + "</p>";
    }
    body += "</body></html>";
    
    response.setBody(body);
    return response;
}

HttpResponse HttpResponse::errorResponseWithConfig(int status, const ServerConfig* config, const std::string& message) {
    // Try to use custom error page from config
    if (config && !config->error_pages.empty()) {
        std::map<int, std::string>::const_iterator it = config->error_pages.find(status);
        if (it != config->error_pages.end()) {
            // Custom error page configured
            std::string error_page_path = config->root + it->second;
            if (Utils::fileExists(error_page_path) && Utils::isReadable(error_page_path)) {
                HttpResponse response = fileResponse(error_page_path);
                response.setStatus(status);
                return response;
            }
        }
    }
    // Fall back to default error response
    return errorResponse(status, message);
}

HttpResponse HttpResponse::fileResponse(const std::string& filepath) {
    HttpResponse response;
    
    if (!Utils::fileExists(filepath) || !Utils::isReadable(filepath)) {
        return errorResponse(HTTP_NOT_FOUND);
    }
    std::string content = Utils::readFile(filepath);
    std::string mime_type = Utils::getMimeType(filepath);
    
    response.setContentType(mime_type);
    response.setBody(content);
    
    return response;
}

HttpResponse HttpResponse::directoryListingResponse(const std::string& path, const std::string& uri) {
    HttpResponse response;
    
    if (!Utils::isDirectory(path)) {
        return errorResponse(HTTP_NOT_FOUND);
    }
    
    std::vector<std::string> files = Utils::listDirectory(path);
    
    std::string body = "<!DOCTYPE html><html><head><title>Directory listing for " + uri + "</title>";
    body += "<style>body{font-family:Arial,sans-serif;margin:40px;}h1{color:#333;}ul{list-style:none;}</style>";
    body += "</head><body><h1>Directory listing for " + uri + "</h1><ul>";
    
    if (uri != "/") {
        body += "<li><a href=\"../\">../</a></li>";
    }
    
    for (size_t i = 0; i < files.size(); ++i) {
        std::string file_path = Utils::joinPath(path, files[i]);
        std::string display_name = files[i];
        if (Utils::isDirectory(file_path)) {
            display_name += "/";
        }
        body += "<li><a href=\"" + display_name + "\">" + display_name + "</a></li>";
    }
    
    body += "</ul></body></html>";
    
    response.setContentType("text/html");
    response.setBody(body);
    
    return response;
}

HttpResponse HttpResponse::redirectResponse(const std::string& location, int status_code) {
    HttpResponse response;
    response.setStatus(status_code);
    response.setHeader("Location", location);
    response.setBody("<!DOCTYPE html><html><head><title>Redirect</title></head><body><h1>Redirecting...</h1></body></html>");
    return response;
}

// UNUSED FUNCTION - Commented out for now
/*
void HttpResponse::print() const {
    std::cout << "╔══════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║            HTTP RESPONSE                             ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════╝" << std::endl;
    
    std::cout << "Status: " << _status << " - " << statusToString(_status) << std::endl;
    std::cout << "Headers Sent: " << (_headers_sent ? "yes" : "no") << std::endl;
    
    std::cout << "\nHeaders (" << _headers.size() << "):" << std::endl;
    if (_headers.empty()) {
        std::cout << "  (no headers)" << std::endl;
    } else {
        for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); 
             it != _headers.end(); ++it) {
            std::cout << "  " << it->first << ": " << it->second << std::endl;
        }
    }
    
#ifdef BONUS
    if (!_set_cookie_headers.empty()) {
        std::cout << "\nSet-Cookie Headers (" << _set_cookie_headers.size() << "):" << std::endl;
        for (size_t i = 0; i < _set_cookie_headers.size(); ++i) {
            std::cout << "  " << _set_cookie_headers[i] << std::endl;
        }
    }
#endif
    
    std::cout << "\nBody Size: " << _body.size() << " bytes" << std::endl;
    if (!_body.empty() && _body.size() <= 500) {
        std::cout << "Body Preview:" << std::endl;
        std::cout << "---" << std::endl;
        std::cout << _body.substr(0, 500);
        std::cout << std::endl << "---" << std::endl;
    } else if (!_body.empty()) {
        std::cout << "Body Preview (first 500 chars):" << std::endl;
        std::cout << "---" << std::endl;
        std::cout << _body.substr(0, 500) << "...";
        std::cout << std::endl << "---" << std::endl;
    }
    
    if (!_response_string.empty()) {
        std::cout << "\nFull Response Size: " << _response_string.size() << " bytes" << std::endl;
    }
    std::cout << std::endl;
}
*/
