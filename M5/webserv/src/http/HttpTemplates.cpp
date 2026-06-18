#include "../../includes/http/HttpTemplates.hpp"
#include "../../includes/utils/Utils.hpp"

HttpResponse HttpTemplates::generateTemplateResponse(int status_code, const std::string& custom_message) {
    HttpResponse response;
    response.setStatus(status_code);
    response.setContentType("text/html; charset=utf-8");
    
    std::string html = generateErrorPageHTML(status_code, custom_message);
    response.setBody(html);
    
    return response;
}

std::string HttpTemplates::generateTemplateRequest(const std::string& method, const std::string& uri, 
                                                  const std::string& body, 
                                                  const std::map<std::string, std::string>& headers) {
    std::string request = method + " " + uri + " HTTP/1.1\r\n";
    
    // Default headers
    bool hasHost = false;
    bool hasContentLength = false;
    bool hasConnection = false;
    
    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
        request += it->first + ": " + it->second + "\r\n";
        std::string lowerKey = Utils::toLowerCase(it->first);
        if (lowerKey == "host") hasHost = true;
        else if (lowerKey == "content-length") hasContentLength = true;
        else if (lowerKey == "connection") hasConnection = true;
    }
    
    // Add missing default headers
    if (!hasHost) {
        request += "Host: localhost\r\n";
    }
    if (!hasConnection) {
        request += "Connection: close\r\n";
    }
    if (!body.empty() && !hasContentLength) {
        request += "Content-Length: " + Utils::toString(body.length()) + "\r\n";
    }
    
    request += "\r\n"; // End headers
    
    if (!body.empty()) {
        request += body;
    }
    
    return request;
}

std::string HttpTemplates::getStatusMessage(int status_code) {
    switch (status_code) {
        // 1xx Informational
        case HTTP_CONTINUE: return "Continue";
        case HTTP_SWITCHING_PROTOCOLS: return "Switching Protocols";
        case HTTP_PROCESSING: return "Processing";
        case HTTP_EARLY_HINTS: return "Early Hints";
        
        // 2xx Success
        case HTTP_OK: return "OK";
        case HTTP_CREATED: return "Created";
        case HTTP_ACCEPTED: return "Accepted";
        case HTTP_NON_AUTHORITATIVE_INFORMATION: return "Non-Authoritative Information";
        case HTTP_NO_CONTENT: return "No Content";
        case HTTP_RESET_CONTENT: return "Reset Content";
        case HTTP_PARTIAL_CONTENT: return "Partial Content";
        case HTTP_MULTI_STATUS: return "Multi-Status";
        case HTTP_ALREADY_REPORTED: return "Already Reported";
        case HTTP_IM_USED: return "IM Used";
        
        // 3xx Redirection
        case HTTP_MULTIPLE_CHOICES: return "Multiple Choices";
        case HTTP_MOVED_PERMANENTLY: return "Moved Permanently";
        case HTTP_FOUND: return "Found";
        case HTTP_SEE_OTHER: return "See Other";
        case HTTP_NOT_MODIFIED: return "Not Modified";
        case HTTP_USE_PROXY: return "Use Proxy";
        case HTTP_TEMPORARY_REDIRECT: return "Temporary Redirect";
        case HTTP_PERMANENT_REDIRECT: return "Permanent Redirect";
        
        // 4xx Client Error
        case HTTP_BAD_REQUEST: return "Bad Request";
        case HTTP_UNAUTHORIZED: return "Unauthorized";
        case HTTP_PAYMENT_REQUIRED: return "Payment Required";
        case HTTP_FORBIDDEN: return "Forbidden";
        case HTTP_NOT_FOUND: return "Not Found";
        case HTTP_METHOD_NOT_ALLOWED: return "Method Not Allowed";
        case HTTP_NOT_ACCEPTABLE: return "Not Acceptable";
        case HTTP_PROXY_AUTHENTICATION_REQUIRED: return "Proxy Authentication Required";
        case HTTP_REQUEST_TIMEOUT: return "Request Timeout";
        case HTTP_CONFLICT: return "Conflict";
        case HTTP_GONE: return "Gone";
        case HTTP_LENGTH_REQUIRED: return "Length Required";
        case HTTP_PRECONDITION_FAILED: return "Precondition Failed";
        case HTTP_PAYLOAD_TOO_LARGE: return "Payload Too Large";
        case HTTP_URI_TOO_LONG: return "URI Too Long";
        case HTTP_UNSUPPORTED_MEDIA_TYPE: return "Unsupported Media Type";
        case HTTP_RANGE_NOT_SATISFIABLE: return "Range Not Satisfiable";
        case HTTP_EXPECTATION_FAILED: return "Expectation Failed";
        case HTTP_IM_A_TEAPOT: return "I'm a teapot";
        case HTTP_MISDIRECTED_REQUEST: return "Misdirected Request";
        case HTTP_UNPROCESSABLE_ENTITY: return "Unprocessable Entity";
        case HTTP_LOCKED: return "Locked";
        case HTTP_FAILED_DEPENDENCY: return "Failed Dependency";
        case HTTP_TOO_EARLY: return "Too Early";
        case HTTP_UPGRADE_REQUIRED: return "Upgrade Required";
        case HTTP_PRECONDITION_REQUIRED: return "Precondition Required";
        case HTTP_TOO_MANY_REQUESTS: return "Too Many Requests";
        case HTTP_REQUEST_HEADER_FIELDS_TOO_LARGE: return "Request Header Fields Too Large";
        case HTTP_UNAVAILABLE_FOR_LEGAL_REASONS: return "Unavailable For Legal Reasons";
        
        // 5xx Server Error
        case HTTP_INTERNAL_SERVER_ERROR: return "Internal Server Error";
        case HTTP_NOT_IMPLEMENTED: return "Not Implemented";
        case HTTP_BAD_GATEWAY: return "Bad Gateway";
        case HTTP_SERVICE_UNAVAILABLE: return "Service Unavailable";
        case HTTP_GATEWAY_TIMEOUT: return "Gateway Timeout";
        case HTTP_HTTP_VERSION_NOT_SUPPORTED: return "HTTP Version Not Supported";
        case HTTP_VARIANT_ALSO_NEGOTIATES: return "Variant Also Negotiates";
        case HTTP_INSUFFICIENT_STORAGE: return "Insufficient Storage";
        case HTTP_LOOP_DETECTED: return "Loop Detected";
        case HTTP_NOT_EXTENDED: return "Not Extended";
        case HTTP_NETWORK_AUTHENTICATION_REQUIRED: return "Network Authentication Required";
        
        default: return "Unknown Status";
    }
}

std::string HttpTemplates::getErrorDescription(int status_code) {
    switch (status_code) {
        case HTTP_BAD_REQUEST:
            return "The server cannot process the request due to a client error (e.g., malformed request syntax).";
        case HTTP_UNAUTHORIZED:
            return "The request requires user authentication or the authentication credentials provided are invalid.";
        case HTTP_PAYMENT_REQUIRED:
            return "Payment is required to access this resource.";
        case HTTP_FORBIDDEN:
            return "The server understood the request but refuses to authorize it. You don't have permission to access this resource.";
        case HTTP_NOT_FOUND:
            return "The requested resource could not be found on this server. Please check the URL and try again.";
        case HTTP_METHOD_NOT_ALLOWED:
            return "The request method is not supported for the requested resource.";
        case HTTP_NOT_ACCEPTABLE:
            return "The server cannot produce a response matching the list of acceptable values defined in the request's headers.";
        case HTTP_REQUEST_TIMEOUT:
            return "The server timed out waiting for the request.";
        case HTTP_CONFLICT:
            return "The request could not be completed due to a conflict with the current state of the resource.";
        case HTTP_GONE:
            return "The requested resource is no longer available and will not be available again.";
        case HTTP_LENGTH_REQUIRED:
            return "The request did not specify the length of its content, which is required by the requested resource.";
        case HTTP_PAYLOAD_TOO_LARGE:
            return "The request is larger than the server is willing or able to process.";
        case HTTP_URI_TOO_LONG:
            return "The URI provided was too long for the server to process.";
        case HTTP_UNSUPPORTED_MEDIA_TYPE:
            return "The request entity has a media type which the server or resource does not support.";
        case HTTP_TOO_MANY_REQUESTS:
            return "The user has sent too many requests in a given amount of time.";
        case HTTP_INTERNAL_SERVER_ERROR:
            return "The server encountered an unexpected condition that prevented it from fulfilling the request.";
        case HTTP_NOT_IMPLEMENTED:
            return "The server does not support the functionality required to fulfill the request.";
        case HTTP_BAD_GATEWAY:
            return "The server, while acting as a gateway or proxy, received an invalid response from the upstream server.";
        case HTTP_SERVICE_UNAVAILABLE:
            return "The server is currently unavailable (because it is overloaded or down for maintenance).";
        case HTTP_GATEWAY_TIMEOUT:
            return "The server, while acting as a gateway or proxy, did not receive a timely response from the upstream server.";
        default:
            return "An error occurred while processing your request.";
    }
}

bool HttpTemplates::isErrorStatus(int status_code) {
    return status_code >= 400;
}

std::string HttpTemplates::generateErrorPageHTML(int status_code, const std::string& message) {
    std::string statusMessage = getStatusMessage(status_code);
    std::string description = message.empty() ? getErrorDescription(status_code) : message;
    std::string icon = getStatusIcon(status_code);
    std::string color = getStatusColor(status_code);
    
    std::string html = "<!DOCTYPE html>\n";
    html += "<html>\n";
    html += "<head>\n";
    html += "    <title>" + Utils::toString(status_code) + " " + statusMessage + "</title>\n";
    html += "    <meta charset=\"UTF-8\">\n";
    html += "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    html += "    <style>\n";
    html += getErrorPageCSS(status_code);
    html += "    </style>\n";
    html += "</head>\n";
    html += "<body>\n";
    html += "    <div class=\"floating-shapes\">\n";
    html += "        <div class=\"shape\">" + icon + "</div>\n";
    html += "        <div class=\"shape\">🌐</div>\n";
    html += "        <div class=\"shape\">⚡</div>\n";
    html += "        <div class=\"shape\">🔧</div>\n";
    html += "    </div>\n";
    html += "    \n";
    html += "    <div class=\"error-container\">\n";
    html += "        <div class=\"icon\">" + icon + "</div>\n";
    html += "        <div class=\"error-code\">" + Utils::toString(status_code) + "</div>\n";
    html += "        <h1 class=\"error-title\">" + statusMessage + "</h1>\n";
    html += "        <p class=\"error-message\">" + description + "</p>\n";
    html += "        <a href=\"/\" class=\"home-link\">🏠 Return Home</a>\n";
    html += "        <div class=\"footer\">\n";
    html += "            <p>WebServ HTTP Server</p>\n";
    html += "        </div>\n";
    html += "    </div>\n";
    html += "</body>\n";
    html += "</html>";
    
    return html;
}

std::string HttpTemplates::getErrorPageCSS(int status_code) {
    std::string color = getStatusColor(status_code);
    
    std::string css = "        * {\n";
    css += "            margin: 0;\n";
    css += "            padding: 0;\n";
    css += "            box-sizing: border-box;\n";
    css += "        }\n";
    css += "        \n";
    css += "        body {\n";
    css += "            font-family: 'Arial', sans-serif;\n";
    css += "            background: linear-gradient(135deg, " + color + " 0%, " + getStatusColor(status_code) + " 100%);\n";
    css += "            color: white;\n";
    css += "            height: 100vh;\n";
    css += "            display: flex;\n";
    css += "            align-items: center;\n";
    css += "            justify-content: center;\n";
    css += "            overflow: hidden;\n";
    css += "        }\n";
    css += "        \n";
    css += "        .error-container {\n";
    css += "            text-align: center;\n";
    css += "            background: rgba(255,255,255,0.1);\n";
    css += "            padding: 60px 40px;\n";
    css += "            border-radius: 20px;\n";
    css += "            backdrop-filter: blur(15px);\n";
    css += "            border: 1px solid rgba(255,255,255,0.2);\n";
    css += "            box-shadow: 0 20px 40px rgba(0,0,0,0.3);\n";
    css += "            animation: containerFloat 6s ease-in-out infinite;\n";
    css += "            position: relative;\n";
    css += "        }\n";
    css += "        \n";
    css += "        .error-code {\n";
    css += "            font-size: 120px;\n";
    css += "            font-weight: bold;\n";
    css += "            margin: 0;\n";
    css += "            text-shadow: 3px 3px 6px rgba(0,0,0,0.4);\n";
    css += "            background: linear-gradient(45deg, #ffd700, #ffed4e, #ffd700);\n";
    css += "            background-size: 200% 200%;\n";
    css += "            -webkit-background-clip: text;\n";
    css += "            -webkit-text-fill-color: transparent;\n";
    css += "            background-clip: text;\n";
    css += "            animation: shimmer 3s ease-in-out infinite;\n";
    css += "            display: inline-block;\n";
    css += "        }\n";
    css += "        \n";
    css += "        .icon {\n";
    css += "            font-size: 60px;\n";
    css += "            margin: 20px 0;\n";
    css += "            animation: iconBounce 2s ease-in-out infinite;\n";
    css += "        }\n";
    css += "        \n";
    css += "        .error-title {\n";
    css += "            font-size: 32px;\n";
    css += "            margin: 20px 0;\n";
    css += "            font-weight: 300;\n";
    css += "            letter-spacing: 2px;\n";
    css += "        }\n";
    css += "        \n";
    css += "        .error-message {\n";
    css += "            font-size: 18px;\n";
    css += "            margin: 25px 0;\n";
    css += "            opacity: 0.9;\n";
    css += "            line-height: 1.6;\n";
    css += "        }\n";
    css += "        \n";
    css += "        .home-link {\n";
    css += "            display: inline-block;\n";
    css += "            color: #ffd700;\n";
    css += "            text-decoration: none;\n";
    css += "            font-weight: bold;\n";
    css += "            font-size: 18px;\n";
    css += "            padding: 15px 30px;\n";
    css += "            border: 2px solid #ffd700;\n";
    css += "            border-radius: 30px;\n";
    css += "            margin-top: 20px;\n";
    css += "            transition: all 0.3s ease;\n";
    css += "        }\n";
    css += "        \n";
    css += "        .home-link:hover {\n";
    css += "            background-color: #ffd700;\n";
    css += "            color: #333;\n";
    css += "            transform: translateY(-2px);\n";
    css += "        }\n";
    css += "        \n";
    css += "        .floating-shapes {\n";
    css += "            position: absolute;\n";
    css += "            width: 100%;\n";
    css += "            height: 100%;\n";
    css += "            overflow: hidden;\n";
    css += "            z-index: 1;\n";
    css += "        }\n";
    css += "        \n";
    css += "        .shape {\n";
    css += "            position: absolute;\n";
    css += "            font-size: 30px;\n";
    css += "            animation: float 8s ease-in-out infinite;\n";
    css += "        }\n";
    css += "        \n";
    css += "        .shape:nth-child(1) { top: 20%; left: 10%; animation-delay: 0s; }\n";
    css += "        .shape:nth-child(2) { top: 60%; left: 80%; animation-delay: 2s; }\n";
    css += "        .shape:nth-child(3) { top: 80%; left: 20%; animation-delay: 4s; }\n";
    css += "        .shape:nth-child(4) { top: 30%; left: 70%; animation-delay: 6s; }\n";
    css += "        \n";
    css += "        .footer {\n";
    css += "            margin-top: 30px;\n";
    css += "            opacity: 0.7;\n";
    css += "            font-size: 14px;\n";
    css += "        }\n";
    css += "        \n";
    css += "        @keyframes containerFloat {\n";
    css += "            0%, 100% { transform: translateY(0px); }\n";
    css += "            50% { transform: translateY(-10px); }\n";
    css += "        }\n";
    css += "        \n";
    css += "        @keyframes shimmer {\n";
    css += "            0%, 100% { background-position: 0% 50%; }\n";
    css += "            50% { background-position: 100% 50%; }\n";
    css += "        }\n";
    css += "        \n";
    css += "        @keyframes iconBounce {\n";
    css += "            0%, 100% { transform: scale(1); }\n";
    css += "            50% { transform: scale(1.2); }\n";
    css += "        }\n";
    css += "        \n";
    css += "        @keyframes float {\n";
    css += "            0%, 100% { transform: translateY(0px) rotate(0deg); }\n";
    css += "            25% { transform: translateY(-20px) rotate(5deg); }\n";
    css += "            50% { transform: translateY(0px) rotate(0deg); }\n";
    css += "            75% { transform: translateY(-10px) rotate(-5deg); }\n";
    css += "        }\n";
    
    return css;
}

std::string HttpTemplates::getStatusIcon(int status_code) {
    if (status_code >= 400 && status_code < 500) {
        switch (status_code) {
            case HTTP_BAD_REQUEST: return "🤔";
            case HTTP_UNAUTHORIZED: return "🔐";
            case HTTP_FORBIDDEN: return "🔒";
            case HTTP_NOT_FOUND: return "🔍";
            case HTTP_METHOD_NOT_ALLOWED: return "🛑";
            case HTTP_REQUEST_TIMEOUT: return "⏰";
            case HTTP_TOO_MANY_REQUESTS: return "🚦";
            default: return "❌";
        }
    } else if (status_code >= 500) {
        switch (status_code) {
            case HTTP_INTERNAL_SERVER_ERROR: return "⚠️";
            case HTTP_NOT_IMPLEMENTED: return "🚧";
            case HTTP_BAD_GATEWAY: return "🌐";
            case HTTP_SERVICE_UNAVAILABLE: return "🔧";
            default: return "💻";
        }
    } else if (status_code >= 300 && status_code < 400) {
        return "↗️";
    } else if (status_code >= 200 && status_code < 300) {
        return "✅";
    } else {
        return "ℹ️";
    }
}

std::string HttpTemplates::getStatusColor(int status_code) {
    if (status_code >= 400 && status_code < 500) {
        switch (status_code) {
            case HTTP_BAD_REQUEST: return "#f39c12";
            case HTTP_UNAUTHORIZED: return "#8e44ad";
            case HTTP_FORBIDDEN: return "#e74c3c";
            case HTTP_NOT_FOUND: return "#667eea";
            case HTTP_METHOD_NOT_ALLOWED: return "#8e44ad";
            default: return "#e67e22";
        }
    } else if (status_code >= 500) {
        return "#ff6b6b";
    } else if (status_code >= 300 && status_code < 400) {
        return "#3498db";
    } else if (status_code >= 200 && status_code < 300) {
        return "#27ae60";
    } else {
        return "#95a5a6";
    }
}