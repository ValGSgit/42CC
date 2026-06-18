#ifndef HTTP_TEMPLATES_HPP
#define HTTP_TEMPLATES_HPP

#include "../webserv.hpp"
#include "HttpStatusCodes.hpp"
#include "HttpResponse.hpp"

class HttpTemplates {
    public:
        // Generate a templated HTTP response based on status code
        static HttpResponse generateTemplateResponse(int status_code, const std::string& custom_message = "");
        
        // Generate a templated HTTP request for testing
        static std::string generateTemplateRequest(const std::string& method, const std::string& uri, 
                                                  const std::string& body = "", 
                                                  const std::map<std::string, std::string>& headers = std::map<std::string, std::string>());
        
        // Get status message from code
        static std::string getStatusMessage(int status_code);
        
        // Get descriptive error message for status codes
        static std::string getErrorDescription(int status_code);
        
        // Check if a status code is an error status
        static bool isErrorStatus(int status_code);
        
        // Generate beautiful error page HTML
        static std::string generateErrorPageHTML(int status_code, const std::string& message = "");

    private:
        // Helper function to get CSS for error pages
        static std::string getErrorPageCSS(int status_code);
        
        // Helper function to get appropriate emoji/icon for status
        static std::string getStatusIcon(int status_code);
        
        // Helper function to get color scheme for status
        static std::string getStatusColor(int status_code);
};

#endif