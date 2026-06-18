# WebServ Architecture Flow Diagram

## 1. STARTUP FLOW

```
main()
  │
  ├─> ConfigParser::parse("config.conf")
  │     │
  │     └─> Returns: std::vector<ServerConfig>
  │           ├─ ports: [8080, 8081, ...]
  │           ├─ server_name: "example.com"
  │           ├─ routes: map<path, RouteConfig>
  │           ├─ max_body_size: 1MB
  │           └─ error_pages: map<code, path>
  │
  ├─> ServerManager manager(configs)
  │     │
  │     └─> Constructor:
  │           ├─ Creates epoll instance (_epoll_fd)
  │           ├─ For each ServerConfig:
  │           │   └─ For each port:
  │           │       ├─ Create socket
  │           │       ├─ Bind to port
  │           │       ├─ Listen
  │           │       ├─ Set non-blocking
  │           │       └─ Add to epoll (EPOLLIN)
  │           │
  │           └─ Stores in _server_sockets:
  │                 ServerSocket {
  │                   fd: socket_fd,
  │                   port: 8080,
  │                   config: &ServerConfig
  │                 }
  │
  └─> manager.run()
        └─> [Goes to MAIN LOOP]
```

---

## 2. DATA STRUCTURES LINKAGE

```
┌─────────────────────────────────────────────────────────────┐
│                      ServerManager                          │
│  ┌───────────────────────────────────────────────────────┐  │
│  │ _configs: vector<ServerConfig>                        │  │
│  │   [0] ServerConfig {                                  │  │
│  │         ports: [8080, 8081]                          │  │
│  │         server_name: "example.com"                   │  │
│  │         routes: {                                    │  │
│  │           "/" -> RouteConfig {                       │  │
│  │             allowed_methods: [GET, POST]            │  │
│  │             root_directory: "./www"                 │  │
│  │             index_file: "index.html"                │  │
│  │             directory_listing: false                │  │
│  │             max_body_size: 1MB                      │  │
│  │           }                                          │  │
│  │           "/upload" -> RouteConfig {...}            │  │
│  │         }                                            │  │
│  │       }                                              │  │
│  │   [1] ServerConfig {...}                            │  │
│  └───────────────────────────────────────────────────────┘  │
│                                                             │
│  ┌───────────────────────────────────────────────────────┐  │
│  │ _server_sockets: vector<ServerSocket>                │  │
│  │   [0] ServerSocket {                                 │  │
│  │         fd: 3,                                       │  │
│  │         port: 8080,                                  │  │
│  │         config: &_configs[0] ────┐                   │  │
│  │       }                          │                   │  │
│  │   [1] ServerSocket {             │                   │  │
│  │         fd: 4,                   │                   │  │
│  │         port: 8081,              │                   │  │
│  │         config: &_configs[0] ────┤                   │  │
│  │       }                          ▼                   │  │
│  └──────────────────────────────────┼───────────────────┘  │
│                                     │                      │
│  ┌──────────────────────────────────┼───────────────────┐  │
│  │ _clients: map<fd, ClientConnection>                 │  │
│  │   [5] -> ClientConnection {                         │  │
│  │            fd: 5,                                    │  │
│  │            server_port: 8080, ───┘  (links back)    │  │
│  │            state: STATE_READING_HEADERS,            │  │
│  │            last_activity: timestamp,                │  │
│  │            buffer: "GET / HTTP/1.1\r\n...",         │  │
│  │            bytes_sent: 0,                           │  │
│  │            keep_alive: true                         │  │
│  │          }                                           │  │
│  │   [6] -> ClientConnection {...}                     │  │
│  └─────────────────────────────────────────────────────┘  │
│                                                             │
│  ┌───────────────────────────────────────────────────────┐  │
│  │ _epoll_fd: 7                                         │  │
│  │   Monitors:                                          │  │
│  │     fd 3 (server socket 8080) -> EPOLLIN            │  │
│  │     fd 4 (server socket 8081) -> EPOLLIN            │  │
│  │     fd 5 (client)             -> EPOLLIN | EPOLLOUT │  │
│  │     fd 6 (client)             -> EPOLLIN            │  │
│  └───────────────────────────────────────────────────────┘  │
│                                                             │
│  ┌───────────────────────────────────────────────────────┐  │
│  │ _handler: HttpHandler                                │  │
│  │   (Created per-request, temporary)                   │  │
│  └───────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
```

---

## 3. MAIN EVENT LOOP

```
ServerManager::run()
  │
  └─> while (g_running) {
        │
        ├─> epoll_wait(_epoll_fd, events, MAX_CONNECTIONS, 1000)
        │     │
        │     └─> Returns events ready for I/O
        │
        └─> for each event {
              │
              ├─> Is it a server socket? (check _server_sockets)
              │     │
              │     YES─> acceptConnection(server_fd)
              │             │
              │             ├─> accept() new client
              │             ├─> Set non-blocking
              │             ├─> Find ServerConfig by server_fd
              │             ├─> Create ClientConnection {
              │             │     fd: client_fd,
              │             │     server_port: server->port,
              │             │     state: STATE_READING_HEADERS
              │             │   }
              │             ├─> Store in _clients[client_fd]
              │             └─> epoll_ctl ADD client_fd (EPOLLIN)
              │
              └─> NO ─> It's a client socket
                    │
                    ├─> event & EPOLLIN?
                    │     │
                    │     YES─> handleClient(client_fd)
                    │             └─> [Goes to CLIENT HANDLING]
                    │
                    ├─> event & EPOLLOUT?
                    │     │
                    │     YES─> handleClient(client_fd)
                    │             └─> [Goes to RESPONSE WRITING]
                    │
                    └─> event & (EPOLLHUP | EPOLLERR)?
                          │
                          YES─> closeConnection(client_fd)
                                  ├─> epoll_ctl DEL client_fd
                                  ├─> close(client_fd)
                                  └─> _clients.erase(client_fd)
        }
      }
```

---

## 4. CLIENT REQUEST HANDLING FLOW

```
handleClient(client_fd)
  │
  ├─> Get ClientConnection& client = _clients[client_fd]
  │
  ├─> STATE_READING_HEADERS or STATE_READING_BODY?
  │     │
  │     YES─> recv(client_fd, buffer)
  │           │
  │           ├─> Append to client.buffer
  │           │
  │           ├─> STATE_READING_HEADERS?
  │           │     │
  │           │     └─> Found "\r\n\r\n"?
  │           │           │
  │           │           YES─> HttpRequest::parse(client.buffer)
  │           │                 │
  │           │                 ├─> Parse method, path, headers
  │           │                 ├─> Has Content-Length?
  │           │                 │     │
  │           │                 │     YES─> client.state = STATE_READING_BODY
  │           │                 │     NO──> client.state = STATE_PROCESSING
  │           │                 │
  │           │                 └─> Return HttpRequest
  │           │
  │           └─> STATE_READING_BODY?
  │                 │
  │                 └─> Received all body?
  │                       │
  │                       YES─> client.state = STATE_PROCESSING
  │
  └─> STATE_PROCESSING?
        │
        YES─> processRequest(client_fd)
              │
              ├─> Get ServerConfig* config by client.server_port
              │
              ├─> Create HttpHandler(client_fd, config)
              │     │
              │     └─> HttpHandler {
              │           _client_fd: 5,
              │           _config: &ServerConfig,
              │           _request: HttpRequest (parsed),
              │           _response: HttpResponse (empty)
              │         }
              │
              ├─> handler.handleRequest()
              │     │
              │     └─> [Goes to REQUEST PROCESSING]
              │
              ├─> Get response string
              │
              ├─> Store in client.buffer
              │
              ├─> client.state = STATE_WRITING_RESPONSE
              │
              └─> epoll_ctl MOD client_fd (EPOLLIN | EPOLLOUT)
```

---

## 5. REQUEST PROCESSING FLOW

```
HttpHandler::handleRequest()
  │
  ├─> Validate request
  │     ├─> Check method allowed
  │     ├─> Check body size
  │     └─> Check path
  │
  ├─> Find matching route in _config->routes
  │     │
  │     └─> RouteConfig& route = _config->routes["/upload"]
  │
  ├─> Handle redirection?
  │     │
  │     YES─> Return 301/302 redirect
  │
  ├─> Is CGI request? (extension matches .php, .py, etc.)
  │     │
  │     YES─> CgiHandler cgi(_request, route)
  │           │
  │           └─> cgi.execute()
  │                 │
  │                 ├─> fork()
  │                 ├─> Set environment variables
  │                 ├─> execve(script_path)
  │                 ├─> Read output
  │                 └─> Return CGI response
  │
  ├─> METHOD_GET?
  │     │
  │     └─> Is directory?
  │           │
  │           ├─> YES─> Has index file?
  │           │         │
  │           │         ├─> YES─> Serve index file
  │           │         └─> NO──> Directory listing enabled?
  │           │                   │
  │           │                   ├─> YES─> Generate directory listing
  │           │                   └─> NO──> 403 Forbidden
  │           │
  │           └─> NO──> Serve static file
  │                     │
  │                     ├─> Open file
  │                     ├─> Read content
  │                     ├─> Set Content-Type
  │                     └─> Return 200 OK
  │
  ├─> METHOD_POST?
  │     │
  │     └─> Is upload?
  │           │
  │           ├─> Parse multipart/form-data
  │           ├─> Extract file
  │           ├─> Save to upload_path
  │           └─> Return 201 Created
  │
  ├─> METHOD_DELETE?
  │     │
  │     └─> Delete file
  │           │
  │           ├─> Check permissions
  │           ├─> unlink(file_path)
  │           └─> Return 204 No Content
  │
  └─> Build HttpResponse
        │
        ├─> Set status code
        ├─> Set headers (Content-Type, Content-Length, etc.)
        ├─> Set body
        └─> Return response string
```

---

## 6. RESPONSE WRITING FLOW

```
handleClient(client_fd) [with EPOLLOUT event]
  │
  ├─> ClientConnection& client = _clients[client_fd]
  │
  ├─> STATE_WRITING_RESPONSE?
  │     │
  │     YES─> send(client_fd, client.buffer + client.bytes_sent)
  │           │
  │           ├─> Update client.bytes_sent
  │           │
  │           ├─> All sent?
  │           │     │
  │           │     YES─> client.keep_alive?
  │           │           │
  │           │           ├─> YES─> Reset client
  │           │           │         ├─> client.state = STATE_READING_HEADERS
  │           │           │         ├─> client.buffer.clear()
  │           │           │         ├─> client.bytes_sent = 0
  │           │           │         └─> epoll_ctl MOD (EPOLLIN only)
  │           │           │
  │           │           └─> NO──> closeConnection(client_fd)
  │           │
  │           └─> Not all sent?
  │                 │
  │                 └─> Keep EPOLLOUT, will continue next event
```

---

## 7. ERROR HANDLING FLOW

```
Any error occurs
  │
  ├─> HttpResponse::buildErrorResponse(code, config)
  │     │
  │     ├─> Check if custom error page exists
  │     │     │
  │     │     └─> config->error_pages[404] = "/error_pages/404.html"
  │     │
  │     ├─> Custom page exists?
  │     │     │
  │     │     ├─> YES─> Read custom HTML
  │     │     └─> NO──> Use default template
  │     │
  │     └─> Build response
  │           ├─> Status line: "HTTP/1.1 404 Not Found"
  │           ├─> Headers
  │           └─> Body (error page HTML)
  │
  └─> Return error response to client
```

---

## 8. TIMEOUT HANDLING

```
ServerManager::checkTimeouts() [called periodically]
  │
  └─> For each client in _clients {
        │
        ├─> time(NULL) - client.last_activity > CONNECTION_TIMEOUT?
        │     │
        │     YES─> closeConnection(client_fd)
        │             │
        │             ├─> Send 408 Request Timeout
        │             ├─> epoll_ctl DEL
        │             ├─> close(fd)
        │             └─> _clients.erase(fd)
      }
```

---

## 9. COMPLETE REQUEST LIFECYCLE EXAMPLE

```
Client connects to port 8080
  │
  1. ACCEPT
     ├─> ServerSocket (fd=3, port=8080, config=&_configs[0])
     ├─> accept() -> client_fd = 5
     ├─> ClientConnection created
     └─> epoll monitors fd=5 for EPOLLIN
  
  2. CLIENT SENDS: "GET /upload/ HTTP/1.1\r\nHost: localhost\r\n\r\n"
     ├─> EPOLLIN event on fd=5
     ├─> recv() -> append to client.buffer
     ├─> HttpRequest::parse()
     │     ├─> method = METHOD_GET
     │     ├─> path = "/upload/"
     │     └─> headers["Host"] = "localhost"
     └─> state = STATE_PROCESSING
  
  3. PROCESS REQUEST
     ├─> Find config by client.server_port (8080)
     ├─> Match route: _configs[0].routes["/upload"]
     │     ├─> allowed_methods: [GET, POST, DELETE]
     │     ├─> root_directory: "./www/uploads"
     │     ├─> directory_listing: true
     │     └─> max_body_size: 5MB
     ├─> HttpHandler::handleRequest()
     │     ├─> Check if directory
     │     ├─> No index file
     │     ├─> Generate directory listing HTML
     │     └─> Build 200 OK response
     └─> state = STATE_WRITING_RESPONSE
  
  4. SEND RESPONSE
     ├─> EPOLLOUT event on fd=5
     ├─> send() response
     ├─> All bytes sent
     ├─> keep_alive = true
     └─> Reset to STATE_READING_HEADERS
  
  5. CLIENT SENDS: "POST /upload HTTP/1.1\r\nContent-Length: 1024\r\n\r\n..."
     ├─> state = STATE_READING_BODY
     ├─> Receive all 1024 bytes
     ├─> Parse multipart form data
     ├─> Save file to ./www/uploads/
     ├─> Build 201 Created response
     └─> Send response
  
  6. CLIENT CLOSES or TIMEOUT
     └─> closeConnection(fd=5)
           ├─> epoll_ctl DEL
           ├─> close(5)
           └─> _clients.erase(5)
```
