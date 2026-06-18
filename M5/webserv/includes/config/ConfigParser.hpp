#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include "../webserv.hpp"

class ConfigParser {
    private:
        std::vector<ServerConfig> _servers;
        std::string _config_file;

        std::vector<std::string> splitIntoLines(const std::string& content);
        size_t parseServerBlock(const std::vector<std::string>& lines, size_t start_index, ServerConfig& server);
        size_t parseLocationBlock(const std::vector<std::string>& lines, size_t start_index, RouteConfig& route);
        std::string trim(const std::string& str);
        std::vector<std::string> split(const std::string& str, char delimiter);
        bool isValidMethod(const std::string& method);
        bool validateBasicSyntax(const std::vector<std::string>& lines);
        void applyInheritanceAndNormalize(ServerConfig& server);

    public:
        ConfigParser();
        ~ConfigParser();

        bool parseConfig(const std::string& filename);
        const std::vector<ServerConfig>& getServers() const;
        // void printConfig() const;  // UNUSED - Commented out
};

#endif
