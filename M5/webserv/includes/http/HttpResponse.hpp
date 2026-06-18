#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include "../webserv.hpp"
#include "HttpStatusCodes.hpp"
#include <ctime>

class HttpRequest;

class HttpResponse {
    private:
        int _status;
        std::map<std::string, std::string> _headers;
#ifdef BONUS
        std::vector<std::string> _set_cookie_headers;
#endif
        std::string _body;
        std::string _response_string;
        bool _headers_sent;

        std::string statusToString(int status) const;
        std::string getCurrentTime();
        void buildResponseString();

    public:
        HttpResponse();
        ~HttpResponse();

        void setStatus(int status);
        void setHeader(const std::string& key, const std::string& value);
        void setBody(const std::string& body);
        void removeBody();
        void appendBody(const std::string& data);
        
        int getStatus() const;
        const std::string& getBody() const;
        const std::string& getResponseString();

        void reset();
        void setDefaultHeaders();
        void setContentType(const std::string& content_type);
        void setContentLength(size_t length);
        void setAllow(const std::string& methods);
        // void print() const;  // UNUSED - Commented out

#ifdef BONUS
        void setCookie(const std::string& name, const std::string& value,
                      int max_age = -1, const std::string& path = "/",
                      bool http_only = true, bool secure = false);
        void clearCookie(const std::string& name);
#endif

        // Static response builders
        static HttpResponse errorResponse(int status, const std::string& message = "");
        static HttpResponse errorResponseWithConfig(int status, const ServerConfig* config, const std::string& message = "");
        static HttpResponse messageResponse(int status, const std::string& title = "", const std::string& message = "");
        static HttpResponse fileResponse(const std::string& filepath);
        static HttpResponse directoryListingResponse(const std::string& path, const std::string& uri);
        static HttpResponse redirectResponse(const std::string& location, int status_code = HTTP_MOVED_PERMANENTLY);
};

#endif
