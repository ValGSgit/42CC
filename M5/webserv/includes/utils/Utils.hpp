#ifndef UTILS_HPP
#define UTILS_HPP

#include "../webserv.hpp"
#include "SessionManager.hpp"

class Utils {
    public:
        // String utilities
        static std::string trim(const std::string& str);
        static std::vector<std::string> split(const std::string& str, char delimiter);
        static std::string toLowerCase(const std::string& str);
        static std::string toUpperCase(const std::string& str);
        static bool equalsIgnoreCase(const std::string& str1, const std::string& str2);
        static bool startsWith(const std::string& str, const std::string& prefix);
        static bool endsWith(const std::string& str, const std::string& suffix);
        static std::string urlDecode(const std::string& url);
        static std::string urlEncode(const std::string& str);

        // File utilities
        static bool fileExists(const std::string& filepath);
        static bool isDirectory(const std::string& path);
        static bool isReadable(const std::string& filepath);
        static bool isWritable(const std::string& filepath);
        static bool    isExecutable(const std::string& filepath);
        static size_t getFileSize(const std::string& filepath);
        static std::string readFile(const std::string& filepath);
        static bool writeFile(const std::string& filepath, void *buffer, int bytes);//, const std::string& content);
        static std::string getFileExtension(const std::string& filepath);
        static std::vector<std::string> listDirectory(const std::string& dirpath);

        // Path utilities
        static std::string joinPath(const std::string& path1, const std::string& path2);
        static std::string normalizePath(const std::string& path);
        static std::string getDirectory(const std::string& filepath);
        static std::string getFilename(const std::string& filepath);
        static bool isPathSecure(const std::string& path, const std::string& root);

        // HTTP utilities
        static std::string getMimeType(const std::string& filepath);
        static std::string getStatusMessage(int status_code);
        static std::string formatFileSize(size_t size);

        // Network utilities
        static void setNonBlocking(int fd);
        static std::string getClientIP(int client_fd);
        
        // Conversion utilities
        static std::string toString(int value);
        static std::string toString(size_t value);
        static int toInt(const std::string& str);
        static size_t toSizeT(const std::string& str);

        // Validation utilities
        static bool isValidHttpMethod(const std::string& method);
        static bool isValidUri(const std::string& uri);
        static bool isValidAbsoluteUri(const std::string& uri);
        static bool isValidUriReference(const std::string& uriRef);
        static bool isValidPort(int port);
        
        // Security utilities
        static std::string sanitizeFilename(const std::string& filename);
        static bool isSafePath(const std::string& path);
        static std::string sanitizeForShell(const std::string& input);
        static bool isValidHttpVersion(const std::string& version);
        static bool containsLF(const std::string& input);
        static bool isAllowedUploadExtension(const std::string& filename);
        
        // URI parsing and validation - RFC 3986 compliant
        struct UriComponents {
            std::string scheme;
            std::string authority;
            std::string userinfo;
            std::string host;
            std::string port;
            std::string path;
            std::string query;
            std::string fragment;
            bool hasScheme;
            bool hasAuthority;
            bool hasUserinfo;
            bool hasPort;
            bool hasQuery;
            bool hasFragment;
            
            UriComponents() : hasScheme(false), hasAuthority(false), hasUserinfo(false), 
                             hasPort(false), hasQuery(false), hasFragment(false) {}
        };
        
        static bool parseUri(const std::string& uri, UriComponents& components);
        static bool parseAuthority(const std::string& authority, UriComponents& components);
        static bool validateUriComponents(const UriComponents& components);
        static std::string normalizeUri(const std::string& uri);
        static std::string reconstructUri(const UriComponents& components);
        
        // URI component validation
        static bool isValidScheme(const std::string& scheme);
        static bool isValidUserinfo(const std::string& userinfo);
        static bool isValidHost(const std::string& host);
        static bool isValidIPv4Address(const std::string& ip);
        static bool isValidIPv6Address(const std::string& ipv6);
        static bool isValidIPv6Literal(const std::string& literal);
        static bool isValidIPvFuture(const std::string& ipvf);
        static bool isValidRegisteredName(const std::string& name);
        static bool isValidPortString(const std::string& port);
        static bool isValidPath(const std::string& path);
        static bool isValidQuery(const std::string& query);
        static bool isValidFragment(const std::string& fragment);
        
        // URI character validation helpers
        static bool isUnreservedChar(char c);
        static bool isSubDelimChar(char c);
        static bool isPChar(char c);
        static bool isValidPercentEncoding(const std::string& str, size_t pos);
        static bool isHexDigit(char c);
        
        // URI normalization helpers
        static std::string normalizePercentEncoding(const std::string& str);
        static std::string removeDotSegments(const std::string& path);
        
        // Debug utilities - Display structure data
        static void printRouteConfig(const RouteConfig& route, const std::string& route_path = "");
        static void printServerConfig(const ServerConfig& config);
        static void printServerSocket(const ServerSocket& socket);
        static void printClientConnection(const ClientConnection& client);
#ifdef BONUS
        static void printSessionData(const SessionData& session);
#endif
        static std::string connectionStateToString(ConnectionState state);
        static std::string httpMethodToString(HttpMethod method);
};

#endif
