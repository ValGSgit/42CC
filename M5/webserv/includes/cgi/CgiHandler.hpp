#ifndef CGI_HANDLER_HPP
#define CGI_HANDLER_HPP

#include "../webserv.hpp"

class HttpRequest;
class HttpResponse;

class CgiHandler {
    private:
        std::string _script_path;
        std::string _cgi_path;
        std::map<std::string, std::string> _env;
        int _timeout;

        void setupEnvironment(const HttpRequest& request, const std::string& script_path);
        bool isCgiScript(const std::string& filepath);
        std::string findCgiExecutable(const std::string& extension);
        HttpResponse parseCgiOutput(const std::string& output);

    public:
        CgiHandler();
        ~CgiHandler();

        HttpResponse executeCgi(const HttpRequest& request, const std::string& script_path, const std::vector<std::string>& good_ext);
        void setTimeout(int seconds);
        static bool isCgiRequest(const std::string& uri);
};

#endif