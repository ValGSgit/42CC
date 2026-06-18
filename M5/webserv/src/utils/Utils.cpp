#include "../../includes/utils/Utils.hpp"

std::string Utils::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

std::vector<std::string> Utils::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == delimiter) {
            token = trim(token);
            if (!token.empty()) {
                tokens.push_back(token);
            }
            token.clear();
        } else {
            token += str[i];
        }
    }
    
    token = trim(token);
    if (!token.empty()) {
        tokens.push_back(token);
    }
    
    return tokens;
}

std::string Utils::toLowerCase(const std::string& str) {
    std::string result = str;
    for (size_t i = 0; i < result.length(); ++i) {
        if (result[i] >= 'A' && result[i] <= 'Z') {
            result[i] = result[i] + ('a' - 'A');
        }
    }
    return result;
}

std::string Utils::toUpperCase(const std::string& str) {
    std::string result = str;
    for (size_t i = 0; i < result.length(); ++i) {
        if (result[i] >= 'a' && result[i] <= 'z') {
            result[i] = result[i] - ('a' - 'A');
        }
    }
    return result;
}

bool Utils::equalsIgnoreCase(const std::string& str1, const std::string& str2) {
    if (str1.length() != str2.length()) {
        return false;
    }
    for (size_t i = 0; i < str1.length(); ++i) {
        char c1 = str1[i];
        char c2 = str2[i];
        // Convert to lowercase for comparison
        if (c1 >= 'A' && c1 <= 'Z') {
            c1 = c1 + ('a' - 'A');
        }
        if (c2 >= 'A' && c2 <= 'Z') {
            c2 = c2 + ('a' - 'A');
        }
        if (c1 != c2) {
            return false;
        }
    }
    return true;
}

bool Utils::startsWith(const std::string& str, const std::string& prefix) {
    if (prefix.length() > str.length()) return false;
    return str.substr(0, prefix.length()) == prefix;
}

bool Utils::endsWith(const std::string& str, const std::string& suffix) {
    if (suffix.length() > str.length()) return false;
    return str.substr(str.length() - suffix.length()) == suffix;
}

std::string Utils::urlDecode(const std::string& url) {
    std::string result;
    for (size_t i = 0; i < url.length(); ++i) {
        if (url[i] == '%' && i + 2 < url.length()) {
            std::string hex = url.substr(i + 1, 2);
            // Manual hex to int conversion
            int value = 0;
            for (size_t j = 0; j < hex.length(); ++j) {
                char c = hex[j];
                value *= 16;
                if (c >= '0' && c <= '9') {
                    value += c - '0';
                } else if (c >= 'A' && c <= 'F') {
                    value += c - 'A' + 10;
                } else if (c >= 'a' && c <= 'f') {
                    value += c - 'a' + 10;
                }
            }
            result += static_cast<char>(value);
            i += 2;
        } else if (url[i] == '+') {
            result += ' ';
        } else {
            result += url[i];
        }
    }
    return result;
}

std::string Utils::urlEncode(const std::string& str) {
    std::string result;
    for (size_t i = 0; i < str.length(); ++i) {
        char c = str[i];
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || 
            (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.' || c == '~') {
            result += c;
        } else {
            result += '%';
            result += "0123456789ABCDEF"[c >> 4];
            result += "0123456789ABCDEF"[c & 15];
        }
    }
    return result;
}

bool Utils::fileExists(const std::string& filepath) {
    struct stat st;
    return stat(filepath.c_str(), &st) == 0;
}

bool Utils::isDirectory(const std::string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) != 0) return false;
    return S_ISDIR(st.st_mode);
}

bool Utils::isReadable(const std::string& filepath) {
    return access(filepath.c_str(), R_OK) == 0;
}

bool Utils::isWritable(const std::string& filepath) {
    return access(filepath.c_str(), W_OK) == 0;
}

size_t Utils::getFileSize(const std::string& filepath) {
    struct stat st;
    if (stat(filepath.c_str(), &st) != 0) return 0;
    return st.st_size;
}

std::string Utils::readFile(const std::string& filepath) {
    int fd = open(filepath.c_str(), O_RDONLY);
    if (fd == -1) return "";
    
    std::string content;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    
    while ((bytes_read = read(fd, buffer, BUFFER_SIZE)) > 0) {
        content.append(buffer, bytes_read);
    }
    if (bytes_read < 0)
        std::cerr << "read failed\n";
    close(fd);
    return content;
}

bool Utils::writeFile(const std::string& filepath, void *buffer, int bytes) {//const std::string& content) {
    // From Ka Hou: I need append mode instead of O_TRUNC
    int fd = open(filepath.c_str(), O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (fd == -1) return false;
    
    ssize_t bytes_written = write(fd, buffer, bytes);//content.c_str(), content.length());
    close(fd);
    if (bytes_written == -1)
    {
        std::cerr << "writeFile failed\n";
        return false;
    }
    //std::cout << "bytes_written = " << bytes_written << std::endl;
    return bytes_written == bytes;//static_cast<ssize_t>(content.length());
}

std::string Utils::getFileExtension(const std::string& filepath) {
    size_t dot_pos = filepath.find_last_of('.');
    if (dot_pos == std::string::npos) return "";
    return toLowerCase(filepath.substr(dot_pos));
}

std::vector<std::string> Utils::listDirectory(const std::string& dirpath) {
    std::vector<std::string> files;
    DIR* dir = opendir(dirpath.c_str());
    if (!dir) return files;
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        std::string name = entry->d_name;
        if (name != "." && name != "..") {
            files.push_back(name);
        }
    }
    closedir(dir);
    
    // Manual sort (bubble sort for C++98 compatibility)
    for (size_t i = 0; i < files.size(); ++i) {
        for (size_t j = 0; j < files.size() - 1 - i; ++j) {
            if (files[j] > files[j + 1]) {
                std::string temp = files[j];
                files[j] = files[j + 1];
                files[j + 1] = temp;
            }
        }
    }
    return files;
}

std::string Utils::joinPath(const std::string& path1, const std::string& path2) {
    if (path1.empty()) return path2;
    if (path2.empty()) return path1;
    
    std::string result = path1;
    if (result[result.length() - 1] != '/' && path2[0] != '/') {
        result += "/";
    }
    result += path2;
    return result;
}

std::string Utils::normalizePath(const std::string& path) {
    std::vector<std::string> components = split(path, '/');
    std::vector<std::string> normalized;
    
    for (size_t i = 0; i < components.size(); ++i) {
        const std::string& component = components[i];
        if (component == "." || component.empty()) {
            continue;
        } else if (component == "..") {
            if (!normalized.empty() && normalized.back() != "..") {
                normalized.pop_back();
            } else if (path[0] != '/') {
                normalized.push_back("..");
            }
        } else {
            normalized.push_back(component);
        }
    }
    
    std::string result;
    if (path[0] == '/') result = "/";
    
    for (size_t i = 0; i < normalized.size(); ++i) {
        if (i > 0) result += "/";
        result += normalized[i];
    }
    
    return result.empty() ? "/" : result;
}

std::string Utils::getDirectory(const std::string& filepath) {
    size_t slash_pos = filepath.find_last_of('/');
    if (slash_pos == std::string::npos) return ".";
    return filepath.substr(0, slash_pos);
}

std::string Utils::getFilename(const std::string& filepath) {
    size_t slash_pos = filepath.find_last_of('/');
    if (slash_pos == std::string::npos) return filepath;
    return filepath.substr(slash_pos + 1);
}

bool Utils::isPathSecure(const std::string& path, const std::string& root) {
    std::string normalized_path = normalizePath(path);
    std::string normalized_root = normalizePath(root);
    return startsWith(normalized_path, normalized_root);
}

std::string Utils::getMimeType(const std::string& filepath) {
    std::string ext = getFileExtension(filepath);
    
    if (ext == ".html" || ext == ".htm") return "text/html";
    if (ext == ".css") return "text/css";
    if (ext == ".js") return "application/javascript";
    if (ext == ".json") return "application/json";
    if (ext == ".xml") return "application/xml";
    if (ext == ".jpg" || ext == ".jpeg") return "image/jpeg";
    if (ext == ".png") return "image/png";
    if (ext == ".gif") return "image/gif";
    if (ext == ".ico") return "image/x-icon";
    if (ext == ".svg") return "image/svg+xml";
    if (ext == ".txt") return "text/plain";
    if (ext == ".pdf") return "application/pdf";
    if (ext == ".zip") return "application/zip";
    
    return "application/octet-stream";
}

std::string Utils::getStatusMessage(int status_code) {
    switch (status_code) {
        // 2xx Success
        case 200: return "OK";
        case 201: return "Created";
        case 204: return "No Content";
        
        // 3xx Redirection
        case 301: return "Moved Permanently";
        case 302: return "Found";
        case 307: return "Temporary Redirect";
        case 308: return "Permanent Redirect";
        
        // 4xx Client Errors
        case 400: return "Bad Request";
        case 401: return "Unauthorized";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 408: return "Request Timeout";
        case 409: return "Conflict";
        case 411: return "Length Required";
        case 413: return "Payload Too Large";
        case 414: return "URI Too Long";
        case 415: return "Unsupported Media Type";
        case 417: return "Expectation Failed";
        case 431: return "Request Header Fields Too Large";
        
        // 5xx Server Errors
        case 500: return "Internal Server Error";
        case 501: return "Not Implemented";
        case 502: return "Bad Gateway";
        case 503: return "Service Unavailable";
        case 504: return "Gateway Timeout";
        case 505: return "HTTP Version Not Supported";
        
        default: return "Unknown";
    }
}
std::string Utils::formatFileSize(size_t size) {
    const char* units[] = {"B", "KB", "MB", "GB"};
    int unit = 0;
    double dsize = static_cast<double>(size);
    
    while (dsize >= 1024 && unit < 3) {
        dsize /= 1024;
        unit++;
    }
    
    std::string result;
    int intPart = static_cast<int>(dsize);
    int fracPart = static_cast<int>((dsize - intPart) * 10);
    
    result = toString(intPart);
    if (fracPart > 0) {
        result += ".";
        result += toString(fracPart);
    }
    result += " ";
    result += units[unit];
    
    return result;
}

void Utils::setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return;
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

std::string Utils::getClientIP(int client_fd) {
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    if (getpeername(client_fd, (struct sockaddr*)&addr, &addr_len) == -1) {
        return "unknown";
    }
    return std::string(inet_ntoa(addr.sin_addr));
}

std::string Utils::toString(int value) {
    if (value == 0) return "0";
    
    std::string result;
    bool negative = value < 0;
    if (negative) value = -value;
    
    while (value > 0) {
        result = static_cast<char>('0' + (value % 10)) + result;
        value /= 10;
    }
    
    if (negative) result = "-" + result;
    return result;
}

std::string Utils::toString(size_t value) {
    if (value == 0) return "0";
    
    std::string result;
    while (value > 0) {
        result = static_cast<char>('0' + (value % 10)) + result;
        value /= 10;
    }
    return result;
}

int Utils::toInt(const std::string& str) {
    if (str.empty()) return 0;
    
    int result = 0;
    bool negative = false;
    size_t i = 0;
    
    if (str[0] == '-') {
        negative = true;
        i = 1;
    } else if (str[0] == '+') {
        i = 1;
    }
    
    for (; i < str.length() && str[i] >= '0' && str[i] <= '9'; ++i) {
        result = result * 10 + (str[i] - '0');
    }
    
    return negative ? -result : result;
}

size_t Utils::toSizeT(const std::string& str) {
    if (str.empty()) return 0;
    
    size_t result = 0;
    for (size_t i = 0; i < str.length() && str[i] >= '0' && str[i] <= '9'; ++i) {
        result = result * 10 + (str[i] - '0');
    }
    
    return result;
}

bool Utils::isValidHttpMethod(const std::string& method) {
    return (method == "GET" || method == "POST" || method == "DELETE" ||
            method == "PUT" || method == "HEAD" || method == "OPTIONS");
}

bool Utils::isValidUri(const std::string& uri) {
    // For backwards compatibility, this validates absolute URIs
    return isValidAbsoluteUri(uri);
}

bool Utils::isValidAbsoluteUri(const std::string& uri) {
    if (uri.empty()) return false;
    
    UriComponents components;
    if (!parseUri(uri, components)) return false;
    
    // Absolute URI must have a scheme
    if (!components.hasScheme) return false;
    
    return validateUriComponents(components);
}

bool Utils::isValidUriReference(const std::string& uriRef) {
    if (uriRef.empty()) return false;
    
    UriComponents components;
    return parseUri(uriRef, components) && validateUriComponents(components);
}

bool Utils::parseUri(const std::string& uri, UriComponents& components) {
    if (uri.empty()) return false;
    
    size_t pos = 0;
    const size_t len = uri.length();
    
    // Parse scheme (required for absolute URI)
    size_t colonPos = uri.find(':');
    if (colonPos != std::string::npos && colonPos > 0) {
        std::string potentialScheme = uri.substr(0, colonPos);
        if (isValidScheme(potentialScheme)) {
            components.scheme = toLowerCase(potentialScheme);
            components.hasScheme = true;
            pos = colonPos + 1;
        }
    }
    
    // Parse authority (optional, starts with //)
    if (pos + 1 < len && uri[pos] == '/' && uri[pos + 1] == '/') {
        components.hasAuthority = true;
        pos += 2;
        
        // Find end of authority
        size_t authEnd = len;
        for (size_t i = pos; i < len; ++i) {
            if (uri[i] == '/' || uri[i] == '?' || uri[i] == '#') {
                authEnd = i;
                break;
            }
        }
        
        components.authority = uri.substr(pos, authEnd - pos);
        if (!parseAuthority(components.authority, components)) {
            return false;
        }
        pos = authEnd;
    }
    
    // Parse path
    size_t pathEnd = len;
    for (size_t i = pos; i < len; ++i) {
        if (uri[i] == '?' || uri[i] == '#') {
            pathEnd = i;
            break;
        }
    }
    components.path = uri.substr(pos, pathEnd - pos);
    pos = pathEnd;
    
    // Parse query (optional, starts with ?)
    if (pos < len && uri[pos] == '?') {
        components.hasQuery = true;
        pos++;
        size_t queryEnd = len;
        for (size_t i = pos; i < len; ++i) {
            if (uri[i] == '#') {
                queryEnd = i;
                break;
            }
        }
        components.query = uri.substr(pos, queryEnd - pos);
        pos = queryEnd;
    }
    
    // Parse fragment (optional, starts with #)
    if (pos < len && uri[pos] == '#') {
        components.hasFragment = true;
        pos++;
        components.fragment = uri.substr(pos);
    }
    
    return true;
}

bool Utils::isExecutable(const std::string& filepath) {
    return access(filepath.c_str(), X_OK) == 0;
}

bool Utils::parseAuthority(const std::string& authority, UriComponents& components) {
    if (authority.empty()) return true;
    
    size_t pos = 0;
    
    // Check for userinfo (ends with @)
    size_t atPos = authority.find('@');
    if (atPos != std::string::npos) {
        components.hasUserinfo = true;
        components.userinfo = authority.substr(0, atPos);
        if (!isValidUserinfo(components.userinfo)) return false;
        pos = atPos + 1;
    }
    
    // Parse host and port
    std::string hostPort = authority.substr(pos);
    
    // Check for IPv6 literal (enclosed in [])
    if (!hostPort.empty() && hostPort[0] == '[') {
        size_t closeBracket = hostPort.find(']');
        if (closeBracket == std::string::npos) return false;
        
        components.host = hostPort.substr(0, closeBracket + 1);
        if (!isValidIPv6Literal(components.host)) return false;
        
        // Check for port after IPv6 literal
        if (closeBracket + 1 < hostPort.length()) {
            if (hostPort[closeBracket + 1] != ':') return false;
            components.hasPort = true;
            components.port = hostPort.substr(closeBracket + 2);
            if (!isValidPortString(components.port)) return false;
        }
    } else {
        // IPv4 or registered name, possibly with port
        size_t colonPos = hostPort.find_last_of(':');
        if (colonPos != std::string::npos && colonPos > 0) {
            // Check if this colon is for port (not part of IPv6)
            std::string afterColon = hostPort.substr(colonPos + 1);
            bool isPort = true;
            for (size_t i = 0; i < afterColon.length(); ++i) {
                if (afterColon[i] < '0' || afterColon[i] > '9') {
                    isPort = false;
                    break;
                }
            }
            
            if (isPort && !afterColon.empty()) {
                components.hasPort = true;
                components.host = hostPort.substr(0, colonPos);
                components.port = afterColon;
                if (!isValidPortString(components.port)) return false;
            } else {
                components.host = hostPort;
            }
        } else {
            components.host = hostPort;
        }
        
        // Validate host
        if (!isValidHost(components.host)) return false;
    }
    
    return true;
}

bool Utils::validateUriComponents(const UriComponents& components) {
    // Scheme validation
    if (components.hasScheme && !isValidScheme(components.scheme)) {
        return false;
    }
    
    // Path validation based on authority presence
    if (components.hasAuthority) {
        // When authority is present, path must be empty or start with /
        if (!components.path.empty() && components.path[0] != '/') {
            return false;
        }
    } else {
        // When no authority, path cannot start with //
        if (components.path.length() >= 2 && 
            components.path[0] == '/' && components.path[1] == '/') {
            return false;
        }
    }
    
    // Validate path characters
    if (!isValidPath(components.path)) return false;
    
    // Validate query if present
    if (components.hasQuery && !isValidQuery(components.query)) return false;
    
    // Validate fragment if present
    if (components.hasFragment && !isValidFragment(components.fragment)) return false;
    
    return true;
}

bool Utils::isValidScheme(const std::string& scheme) {
    if (scheme.empty()) return false;
    
    // Must start with letter
    char first = scheme[0];
    if (!((first >= 'A' && first <= 'Z') || (first >= 'a' && first <= 'z'))) {
        return false;
    }
    
    // Subsequent characters must be letter, digit, +, -, or .
    for (size_t i = 1; i < scheme.length(); ++i) {
        char c = scheme[i];
        if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
              (c >= '0' && c <= '9') || c == '+' || c == '-' || c == '.')) {
            return false;
        }
    }
    
    return true;
}

bool Utils::isValidUserinfo(const std::string& userinfo) {
    for (size_t i = 0; i < userinfo.length(); ++i) {
        char c = userinfo[i];
        if (!isUnreservedChar(c) && !isSubDelimChar(c) && c != ':' && 
            !isValidPercentEncoding(userinfo, i)) {
            return false;
        }
        if (userinfo[i] == '%') i += 2; // Skip percent-encoded sequence
    }
    return true;
}

bool Utils::isValidHost(const std::string& host) {
    if (host.empty()) return true; // Empty host is valid in some contexts
    
    // Check if it's an IPv4 address
    if (isValidIPv4Address(host)) return true;
    
    // Check if it's a registered name
    return isValidRegisteredName(host);
}

bool Utils::isValidIPv4Address(const std::string& ip) {
    std::vector<std::string> octets = split(ip, '.');
    if (octets.size() != 4) return false;
    
    for (size_t i = 0; i < octets.size(); ++i) {
        const std::string& octet = octets[i];
        if (octet.empty() || octet.length() > 3) return false;
        
        // Check for leading zeros (except for "0")
        if (octet.length() > 1 && octet[0] == '0') return false;
        
        // Validate digits and range
        int value = 0;
        for (size_t j = 0; j < octet.length(); ++j) {
            if (octet[j] < '0' || octet[j] > '9') return false;
            value = value * 10 + (octet[j] - '0');
        }
        
        if (value > 255) return false;
    }
    
    return true;
}

bool Utils::isValidIPv6Literal(const std::string& literal) {
    if (literal.length() < 3 || literal[0] != '[' || 
        literal[literal.length() - 1] != ']') {
        return false;
    }
    
    std::string ipv6 = literal.substr(1, literal.length() - 2);
    
    // Check for future IP literal format (starts with 'v')
    if (!ipv6.empty() && (ipv6[0] == 'v' || ipv6[0] == 'V')) {
        return isValidIPvFuture(ipv6);
    }
    
    // Basic IPv6 validation (simplified)
    return isValidIPv6Address(ipv6);
}

bool Utils::isValidIPv6Address(const std::string& ipv6) {
    // Simplified IPv6 validation
    if (ipv6.empty()) return false;
    
    size_t doubleColonCount = 0;
    size_t pos = 0;
    while ((pos = ipv6.find("::", pos)) != std::string::npos) {
        doubleColonCount++;
        pos += 2;
    }
    
    // At most one :: allowed
    if (doubleColonCount > 1) return false;
    
    // Split by single colons and validate segments
    std::vector<std::string> segments;
    std::string segment;
    
    for (size_t i = 0; i < ipv6.length(); ++i) {
        if (ipv6[i] == ':') {
            if (i + 1 < ipv6.length() && ipv6[i + 1] == ':') {
                // Double colon - add current segment if not empty
                if (!segment.empty()) {
                    segments.push_back(segment);
                    segment.clear();
                }
                segments.push_back("::"); // Marker for double colon
                i++; // Skip second colon
            } else {
                // Single colon
                segments.push_back(segment);
                segment.clear();
            }
        } else {
            segment += ipv6[i];
        }
    }
    
    if (!segment.empty()) {
        segments.push_back(segment);
    }
    
    // Validate each segment
    for (size_t i = 0; i < segments.size(); ++i) {
        if (segments[i] == "::") continue;
        
        const std::string& seg = segments[i];
        if (seg.empty()) {
            // Empty segment only allowed at start/end with double colon
            if (doubleColonCount == 0 || (i != 0 && i != segments.size() - 1)) {
                return false;
            }
            continue;
        }
        
        // Check for embedded IPv4 (last segment might be IPv4)
        if (i == segments.size() - 1 && seg.find('.') != std::string::npos) {
            if (!isValidIPv4Address(seg)) return false;
            continue;
        }
        
        // Validate hex segment (1-4 hex digits)
        if (seg.length() > 4) return false;
        for (size_t j = 0; j < seg.length(); ++j) {
            char c = seg[j];
            if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || 
                  (c >= 'a' && c <= 'f'))) {
                return false;
            }
        }
    }
    
    return true;
}

bool Utils::isValidIPvFuture(const std::string& ipvf) {
    // Format: v<version>.<unreserved/sub-delims/:>+
    if (ipvf.length() < 3 || ipvf[0] != 'v') return false;
    
    size_t dotPos = ipvf.find('.');
    if (dotPos == std::string::npos || dotPos == 1) return false;
    
    // Validate version (hex digits)
    for (size_t i = 1; i < dotPos; ++i) {
        char c = ipvf[i];
        if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || 
              (c >= 'a' && c <= 'f'))) {
            return false;
        }
    }
    
    // Validate remaining characters
    for (size_t i = dotPos + 1; i < ipvf.length(); ++i) {
        char c = ipvf[i];
        if (!isUnreservedChar(c) && !isSubDelimChar(c) && c != ':') {
            return false;
        }
    }
    
    return true;
}

bool Utils::isValidRegisteredName(const std::string& name) {
    for (size_t i = 0; i < name.length(); ++i) {
        char c = name[i];
        if (!isUnreservedChar(c) && !isSubDelimChar(c) && 
            !isValidPercentEncoding(name, i)) {
            return false;
        }
        if (name[i] == '%') i += 2; // Skip percent-encoded sequence
    }
    return true;
}

bool Utils::isValidPortString(const std::string& port) {
    if (port.empty()) return true; // Empty port is valid
    
    // Must be all digits
    for (size_t i = 0; i < port.length(); ++i) {
        if (port[i] < '0' || port[i] > '9') return false;
    }
    
    // Convert to number and check range
    int portNum = toInt(port);
    return portNum >= 0 && portNum <= 65535;
}

bool Utils::isValidPath(const std::string& path) {
    for (size_t i = 0; i < path.length(); ++i) {
        char c = path[i];
        if (!isPChar(c) && c != '/' && !isValidPercentEncoding(path, i)) {
            return false;
        }
        if (path[i] == '%') i += 2; // Skip percent-encoded sequence
    }
    return true;
}

bool Utils::isValidQuery(const std::string& query) {
    for (size_t i = 0; i < query.length(); ++i) {
        char c = query[i];
        if (!isPChar(c) && c != '/' && c != '?' && !isValidPercentEncoding(query, i)) {
            return false;
        }
        if (query[i] == '%') i += 2; // Skip percent-encoded sequence
    }
    return true;
}

bool Utils::isValidFragment(const std::string& fragment) {
    for (size_t i = 0; i < fragment.length(); ++i) {
        char c = fragment[i];
        if (!isPChar(c) && c != '/' && c != '?' && !isValidPercentEncoding(fragment, i)) {
            return false;
        }
        if (fragment[i] == '%') i += 2; // Skip percent-encoded sequence
    }
    return true;
}

bool Utils::isUnreservedChar(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
           (c >= '0' && c <= '9') || c == '-' || c == '.' || c == '_' || c == '~';
}

bool Utils::isSubDelimChar(char c) {
    return c == '!' || c == '$' || c == '&' || c == '\'' || c == '(' || c == ')' ||
           c == '*' || c == '+' || c == ',' || c == ';' || c == '=';
}

bool Utils::isPChar(char c) {
    return isUnreservedChar(c) || isSubDelimChar(c) || c == ':' || c == '@';
}

bool Utils::isValidPercentEncoding(const std::string& str, size_t pos) {
    if (pos + 2 >= str.length() || str[pos] != '%') return false;
    
    char c1 = str[pos + 1];
    char c2 = str[pos + 2];
    
    return isHexDigit(c1) && isHexDigit(c2);
}

bool Utils::isHexDigit(char c) {
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

std::string Utils::normalizeUri(const std::string& uri) {
    UriComponents components;
    if (!parseUri(uri, components)) {
        return uri; // Return original if parsing fails
    }
    
    // Normalize scheme to lowercase
    if (components.hasScheme) {
        components.scheme = toLowerCase(components.scheme);
    }
    
    // Normalize host to lowercase
    if (components.hasAuthority && !components.host.empty()) {
        components.host = toLowerCase(components.host);
    }
    
    // Normalize percent-encoding
    components.path = normalizePercentEncoding(components.path);
    if (components.hasQuery) {
        components.query = normalizePercentEncoding(components.query);
    }
    if (components.hasFragment) {
        components.fragment = normalizePercentEncoding(components.fragment);
    }
    if (components.hasUserinfo) {
        components.userinfo = normalizePercentEncoding(components.userinfo);
    }
    if (components.hasAuthority) {
        components.host = normalizePercentEncoding(components.host);
    }
    
    // Remove dot segments from path
    components.path = removeDotSegments(components.path);
    
    // Remove default port for known schemes
    if (components.hasPort && components.hasScheme) {
        int port = toInt(components.port);
        if ((components.scheme == "http" && port == 80) ||
            (components.scheme == "https" && port == 443) ||
            (components.scheme == "ftp" && port == 21)) {
            components.hasPort = false;
            components.port.clear();
        }
    }
    
    // Reconstruct URI
    return reconstructUri(components);
}

std::string Utils::normalizePercentEncoding(const std::string& str) {
    std::string result;
    result.reserve(str.length());
    
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '%' && i + 2 < str.length() && 
            isHexDigit(str[i + 1]) && isHexDigit(str[i + 2])) {
            
            // Convert hex to uppercase
            result += '%';
            result += toUpperCase(std::string(1, str[i + 1]))[0];
            result += toUpperCase(std::string(1, str[i + 2]))[0];
            
            // Check if this represents an unreserved character
            int value = 0;
            char c1 = str[i + 1];
            char c2 = str[i + 2];
            
            // Convert hex digits to value
            if (c1 >= '0' && c1 <= '9') value += (c1 - '0') * 16;
            else if (c1 >= 'A' && c1 <= 'F') value += (c1 - 'A' + 10) * 16;
            else if (c1 >= 'a' && c1 <= 'f') value += (c1 - 'a' + 10) * 16;
            
            if (c2 >= '0' && c2 <= '9') value += (c2 - '0');
            else if (c2 >= 'A' && c2 <= 'F') value += (c2 - 'A' + 10);
            else if (c2 >= 'a' && c2 <= 'f') value += (c2 - 'a' + 10);
            
            char decodedChar = static_cast<char>(value);
            
            // If it's an unreserved character, decode it
            if (isUnreservedChar(decodedChar)) {
                result.erase(result.length() - 3); // Remove % and two hex digits
                result += decodedChar;
            }
            
            i += 2; // Skip the hex digits
        } else {
            result += str[i];
        }
    }
    
    return result;
}

std::string Utils::removeDotSegments(const std::string& path) {
    if (path.empty()) return path;
    
    std::string input = path;
    std::string output;
    
    while (!input.empty()) {
        // A: ../  or ./
        if (startsWith(input, "../")) {
            input = input.substr(3);
        } else if (startsWith(input, "./")) {
            input = input.substr(2);
        }
        // B: /./ or /.
        else if (startsWith(input, "/./")) {
            input = "/" + input.substr(3);
        } else if (input == "/.") {
            input = "/";
        }
        // C: /../ or /..
        else if (startsWith(input, "/../")) {
            input = "/" + input.substr(4);
            // Remove last segment from output
            size_t lastSlash = output.find_last_of('/');
            if (lastSlash != std::string::npos) {
                output = output.substr(0, lastSlash);
            } else {
                output.clear();
            }
        } else if (input == "/..") {
            input = "/";
            // Remove last segment from output
            size_t lastSlash = output.find_last_of('/');
            if (lastSlash != std::string::npos) {
                output = output.substr(0, lastSlash);
            } else {
                output.clear();
            }
        }
        // D: . or ..
        else if (input == "." || input == "..") {
            input.clear();
        }
        // E: move first segment to output
        else {
            if (input[0] == '/') {
                size_t nextSlash = input.find('/', 1);
                if (nextSlash != std::string::npos) {
                    output += input.substr(0, nextSlash);
                    input = input.substr(nextSlash);
                } else {
                    output += input;
                    input.clear();
                }
            } else {
                size_t nextSlash = input.find('/');
                if (nextSlash != std::string::npos) {
                    output += input.substr(0, nextSlash);
                    input = input.substr(nextSlash);
                } else {
                    output += input;
                    input.clear();
                }
            }
        }
    }
    
    return output;
}

std::string Utils::reconstructUri(const UriComponents& components) {
    std::string result;
    
    // Add scheme
    if (components.hasScheme) {
        result += components.scheme + ":";
    }
    
    // Add authority
    if (components.hasAuthority) {
        result += "//";
        if (components.hasUserinfo) {
            result += components.userinfo + "@";
        }
        result += components.host;
        if (components.hasPort) {
            result += ":" + components.port;
        }
    }
    
    // Add path
    result += components.path;
    
    // Add query
    if (components.hasQuery) {
        result += "?" + components.query;
    }
    
    // Add fragment
    if (components.hasFragment) {
        result += "#" + components.fragment;
    }
    
    return result;
}

bool Utils::isValidPort(int port) {
    return port > 0 && port <= 65535;
}

// Security utility functions
std::string Utils::sanitizeFilename(const std::string& filename) {
    std::string result;
    
    // Remove path components - only keep the actual filename
    size_t last_slash = filename.find_last_of("/\\");
    std::string clean_name;
    if (last_slash != std::string::npos) {
        clean_name = filename.substr(last_slash + 1);
    } else {
        clean_name = filename;
    }
    
    // Remove dangerous characters and sequences
    for (size_t i = 0; i < clean_name.length(); ++i) {
        char c = clean_name[i];
        
        // Skip null bytes
        if (c == '\0') continue;
        
        // Skip path traversal attempts
        if (c == '.' && i + 1 < clean_name.length() && clean_name[i + 1] == '.') {
            ++i; // Skip both dots
            continue;
        }
        
        // Allow alphanumeric, dots, dashes, underscores
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || 
            (c >= '0' && c <= '9') || c == '.' || c == '-' || c == '_') {
            result += c;
        } else {
            result += '_'; // Replace unsafe chars with underscore
        }
    }
    
    // Don't allow empty filename or just dots
    if (result.empty() || result == "." || result == "..") {
        result = "upload_file";
    }
    
    return result;
}

bool Utils::isSafePath(const std::string& path) {
    // Check for null bytes
    if (path.find('\0') != std::string::npos) {
        return false;
    }
    
    // Check for path traversal attempts
    if (path.find("..") != std::string::npos) {
        return false;
    }
    
    // Check for absolute paths (starting with /)
    if (!path.empty() && path[0] == '/') {
        return false;
    }
    
    // Check for backslashes (Windows-style paths)
    if (path.find('\\') != std::string::npos) {
        return false;
    }
    
    return true;
}

std::string Utils::sanitizeForShell(const std::string& input) {
    std::string result;
    
    // List of dangerous shell metacharacters
    const std::string dangerous = "&|;<>$`\n*?[]{}()!#'\"\\";
    
    for (size_t i = 0; i < input.length(); ++i) {
        char c = input[i];
        
        // Skip null bytes
        if (c == '\0') continue;
        
        // Skip control characters except space and tab
        if (c < 32 && c != ' ' && c != '\t') continue;
        
        // Skip dangerous shell metacharacters
        if (dangerous.find(c) != std::string::npos) {
            continue;
        }
        
        result += c;
    }
    
    return result;
}

bool Utils::isValidHttpVersion(const std::string& version) {
    return (version == "HTTP/1.0" || version == "HTTP/1.1");
}

bool Utils::containsLF(const std::string& input) {
    return (input.find('\r') != std::string::npos || 
            input.find('\n') != std::string::npos);
}

bool Utils::isAllowedUploadExtension(const std::string& filename) {
    // Whitelist of safe file extensions
    static const char* allowed[] = {
        ".txt", ".pdf", ".jpg", ".jpeg", ".png", ".gif", ".bmp",
        ".doc", ".docx", ".xls", ".xlsx", ".zip", ".tar", ".gz",
        ".mp3", ".mp4", ".avi", ".mov", ".wav", ".css", ".json",
        NULL
    };

    std::string ext = getFileExtension(filename);
    ext = toLowerCase(ext);
    
    //std::cout << "ext = " << ext << "\n";
    // Empty extension is not allowed
    if (ext.empty()) {
        return true;
    }
    
    for (int i = 0; allowed[i] != NULL; ++i) {
        if (ext == allowed[i]) {
            return true;
        }
    }
    
    return false;
}

// Debug utilities - Display structure data

std::string Utils::httpMethodToString(HttpMethod method) {
    switch (method) {
        case METHOD_GET: return "GET";
        case METHOD_POST: return "POST";
        case METHOD_PUT: return "PUT";
        case METHOD_HEAD: return "HEAD";
        case METHOD_DELETE: return "DELETE";
        default: return "UNKNOWN";
    }
}

std::string Utils::connectionStateToString(ConnectionState state) {
    switch (state) {
        case STATE_READING_HEADERS: return "READING_HEADERS";
        case STATE_READING_BODY: return "READING_BODY";
        case STATE_PROCESSING: return "PROCESSING";
        case STATE_WRITING_RESPONSE: return "WRITING_RESPONSE";
        case STATE_DONE: return "DONE";
        case STATE_ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

void Utils::printRouteConfig(const RouteConfig& route, const std::string& route_path) {
    std::cout << "  ┌─ Route: " << (route_path.empty() ? "(default)" : route_path) << std::endl;
    
    // Allowed methods
    std::cout << "  │  Methods: ";
    if (route.allowed_methods.empty()) {
        std::cout << "(all allowed)";
    } else {
        for (size_t i = 0; i < route.allowed_methods.size(); ++i) {
            std::cout << route.allowed_methods[i];
            if (i < route.allowed_methods.size() - 1) std::cout << ", ";
        }
    }
    std::cout << std::endl;
    
    // Root directory
    std::cout << "  │  Root: " << (route.root_directory.empty() ? "(not set)" : route.root_directory) << std::endl;
    
    // Index file
    std::cout << "  │  Index: " << (route.index_file.empty() ? "(not set)" : route.index_file) << std::endl;
    
    // Directory listing
    std::cout << "  │  Directory Listing: " << (route.directory_listing ? "enabled" : "disabled") << std::endl;
    
    // Upload path
    std::cout << "  │  Upload Path: " << (route.upload_path.empty() ? "(not set)" : route.upload_path) << std::endl;
    
    // CGI extensions
    std::cout << "  │  CGI Extensions: ";
    if (route.cgi_extensions.empty()) {
        std::cout << "(none)";
    } else {
        for (size_t i = 0; i < route.cgi_extensions.size(); ++i) {
            std::cout << route.cgi_extensions[i];
            if (i < route.cgi_extensions.size() - 1) std::cout << ", ";
        }
    }
    std::cout << std::endl;
    
    // Redirect
    if (!route.redirect_url.empty()) {
        std::cout << "  │  Redirect: " << route.redirect_url << " (code: " << route.redirect_code << ")" << std::endl;
    } else {
        std::cout << "  │  Redirect: (none)" << std::endl;
    }
    
    // Max body size
    std::cout << "  │  Max Body Size: " << formatFileSize(route.max_body_size) << std::endl;
    std::cout << "  └─" << std::endl;
}

void Utils::printServerConfig(const ServerConfig& config) {
    std::cout << "╔══════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║              SERVER CONFIGURATION                    ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════╝" << std::endl;
    
    std::cout << "Port: " << config.port << std::endl;
    std::cout << "Server Name: " << (config.server_name.empty() ? "(not set)" : config.server_name) << std::endl;
    std::cout << "Root: " << config.root << std::endl;
    std::cout << "Index: " << config.index << std::endl;
    std::cout << "Autoindex: " << (config.autoindex ? "enabled" : "disabled") << std::endl;
    std::cout << "Max Body Size: " << formatFileSize(config.max_body_size) << std::endl;
    
    // Error pages
    std::cout << "\nError Pages:" << std::endl;
    if (config.error_pages.empty()) {
        std::cout << "  (none configured)" << std::endl;
    } else {
        for (std::map<int, std::string>::const_iterator it = config.error_pages.begin(); 
             it != config.error_pages.end(); ++it) {
            std::cout << "  " << it->first << ": " << it->second << std::endl;
        }
    }
    
    // Routes
    std::cout << "\nRoutes (" << config.routes.size() << "):" << std::endl;
    if (config.routes.empty()) {
        std::cout << "  (no routes configured)" << std::endl;
    } else {
        for (std::map<std::string, RouteConfig>::const_iterator it = config.routes.begin(); 
             it != config.routes.end(); ++it) {
            printRouteConfig(it->second, it->first);
        }
    }
    std::cout << std::endl;
}

void Utils::printServerSocket(const ServerSocket& socket) {
    std::cout << "╔══════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║              SERVER SOCKET                           ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════╝" << std::endl;
    
    std::cout << "File Descriptor: " << socket.fd << std::endl;
    std::cout << "Port: " << socket.port << std::endl;
    std::cout << "Config Pointer: " << (socket.config ? "valid" : "NULL") << std::endl;
    
    if (socket.config) {
        std::cout << "\n--- Associated Config ---" << std::endl;
        printServerConfig(*socket.config);
    }
    std::cout << std::endl;
}

void Utils::printClientConnection(const ClientConnection& client) {
    std::cout << "╔══════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║            CLIENT CONNECTION                         ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════╝" << std::endl;
    
    std::cout << "File Descriptor: " << client.fd << std::endl;
    std::cout << "Server Port: " << client.server_port << std::endl;
    std::cout << "State: " << connectionStateToString(client.state) << std::endl;
    std::cout << "Last Activity: " << client.last_activity << " (" << time(NULL) - client.last_activity << "s ago)" << std::endl;
    std::cout << "Buffer Size: " << client.buffer.size() << " bytes" << std::endl;
    std::cout << "Bytes Sent: " << client.bytes_sent << std::endl;
    std::cout << "Keep-Alive: " << (client.keep_alive ? "yes" : "no") << std::endl;
    
    // Show buffer preview if not empty
    if (!client.buffer.empty()) {
        std::cout << "\nBuffer Preview (first 200 chars):" << std::endl;
        std::string preview = client.buffer.substr(0, std::min(static_cast<size_t>(200), client.buffer.size()));
        std::cout << "---" << std::endl;
        std::cout << preview;
        if (client.buffer.size() > 200) std::cout << "...";
        std::cout << std::endl << "---" << std::endl;
    }
    std::cout << std::endl;
}

#ifdef BONUS
void Utils::printSessionData(const SessionData& session) {
    std::cout << "╔══════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║              SESSION DATA                            ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════╝" << std::endl;
    
    std::cout << "Session ID: " << session.session_id << std::endl;
    std::cout << "Created At: " << session.created_at << std::endl;
    std::cout << "Last Accessed: " << session.last_accessed << " (" << time(NULL) - session.last_accessed << "s ago)" << std::endl;
    std::cout << "Expires At: " << session.expires_at << " (in " << session.expires_at - time(NULL) << "s)" << std::endl;
    
    std::cout << "\nSession Data (" << session.data.size() << " entries):" << std::endl;
    if (session.data.empty()) {
        std::cout << "  (no data)" << std::endl;
    } else {
        for (std::map<std::string, std::string>::const_iterator it = session.data.begin(); 
             it != session.data.end(); ++it) {
            std::cout << "  " << it->first << " = " << it->second << std::endl;
        }
    }
    std::cout << std::endl;
}
#endif
