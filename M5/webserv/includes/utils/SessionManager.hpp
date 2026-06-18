#ifndef SESSION_MANAGER_HPP
#define SESSION_MANAGER_HPP

#include "../webserv.hpp"


//Test with curl -v -X POST -H "Content-Length: 0" http://localhost:8080/api/session/login 2>&1 | grep -E "(< HTTP|< Set-Cookie)"

struct SessionData {
	std::string session_id;
	time_t created_at;
	time_t last_accessed;
	time_t expires_at;
	std::map<std::string, std::string> data;

	SessionData() : created_at(0), last_accessed(0), expires_at(0) {}
};

class SessionManager {
	private:
		std::map<std::string, SessionData> _sessions;
		std::map<std::string, std::string> _username_to_session;
		int _session_timeout;

		std::string generateSessionId();

	public:
		SessionManager();
		~SessionManager();

	std::string createSession();
	SessionData* getSession(const std::string& session_id);
	void destroySession(const std::string& session_id);
	void destroyAllSessions();
	void cleanExpiredSessions();
		void setSessionTimeout(int seconds);
		size_t getActiveSessionCount() const;
		
		// Duplicate login prevention
		std::string getSessionByUsername(const std::string& username);
		void registerUsername(const std::string& session_id, const std::string& username);
};

#endif
