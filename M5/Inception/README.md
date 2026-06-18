# Inception

*This project has been created as part of the 42 curriculum by vagarcia.*

## Description

Inception is a system administration project that uses Docker to create a small infrastructure running multiple services. The goal is to set up a complete web application stack with NGINX, WordPress, and MariaDB, each running in its own container. This project teaches Docker containerization, networking, volume management, and good security practices.

The infrastructure includes:
- **NGINX**: Web server with TLS/SSL encryption
- **WordPress**: Content management system with PHP-FPM
- **MariaDB**: Database server for WordPress data

All services communicate through a custom Docker network and use volumes for data persistence.

## Instructions

## Set up .env with:
- DOMAIN_NAME=
- MYSQL_DATABASE=
- MYSQL_USER=
- MYSQL_ROOT_PASSWORD_FILE=/run/secrets/db_root_password
- MYSQL_PASSWORD_FILE=/run/secrets/db_password
- WP_ADMIN_USER=
- WP_ADMIN_PASSWORD_FILE=/run/secrets/credentials
- WP_ADMIN_EMAIL=
- WP_USER=
- WP_USER_PASSWORD_FILE=/run/secrets/wp_user_password
- WP_USER_EMAIL=


### Prerequisites

- A virtual machine running Linux (Debian or similar)
- Docker and Docker Compose installed
- `make` utility installed
- Root or sudo access for initial setup

### Installation & Setup

1. **Clone the repository**
   ```bash
   git clone <repository-url>
   cd Inception
   ```

2. **Configure the domain**
   
   Add the following line to `/etc/hosts`:
   ```bash
   sudo sh -c 'echo "127.0.0.1    vagarcia.42.fr" >> /etc/hosts'
   ```

3. **Generate secrets** (first time only)
   ```bash
   ./manage_secrets.sh regenerate
   ```

4. **Build and start the infrastructure**
   ```bash
   make
   ```

   This will:
   - Create data directories
   - Generate secure passwords
   - Build all Docker images
   - Start all containers

5. **Access the website**
   
   Open your browser and go to: `https://vagarcia.42.fr`
   
   WordPress admin panel: `https://vagarcia.42.fr/wp-admin`

### Available Commands

- `make` or `make all` - Build and start everything
- `make build` - Build Docker images only
- `make up` - Start containers
- `make down` - Stop containers
- `make logs` - View logs from all containers
- `make status` - Check container status
- `make clean` - Stop containers and remove them
- `make fclean` - Full cleanup including data
- `make re` - Rebuild everything from scratch

## Project Description

### Docker vs Virtual Machines

**Virtual Machines** run a complete operating system with its own kernel, which requires more resources and takes longer to start. Each VM is fully isolated but heavy.

**Docker containers** share the host OS kernel and only package the application and its dependencies. They're lightweight, start quickly, and use fewer resources. For this project, Docker is perfect because we need multiple services that can communicate with each other without the overhead of running multiple full OS instances.

### Secrets vs Environment Variables

**Environment variables** (`.env` file) store non-sensitive configuration like domain names, usernames, and database names. They're easy to manage and modify.

**Docker secrets** store sensitive data like passwords and API keys. They're mounted as files inside containers at `/run/secrets/` and are never exposed in logs or environment listings. This project uses secrets for all passwords, following security best practices.

### Docker Network vs Host Network

**Host network** mode would make containers use the host's network directly, which is simpler but removes network isolation and creates security risks.

**Docker bridge network** (used in this project) creates an isolated network where containers can communicate using service names as hostnames. This provides security through isolation while allowing controlled communication between containers.

### Docker Volumes vs Bind Mounts

**Docker volumes** are managed by Docker and stored in Docker's storage area. They're portable and easier to backup but less accessible from the host.

**Bind mounts** (used in this project) directly map host directories to containers. This makes it easy to access data from the host system (at `/home/vagarcia/data/`) and meets the subject requirement for data location.

### Design Choices

1. **Alpine Linux 3.22**: Lightweight base image for faster builds and smaller containers
2. **Service separation**: Each service (NGINX, WordPress, MariaDB) runs in its own container
3. **Security**: TLS/SSL encryption, Docker secrets, no hardcoded passwords
4. **Data persistence**: Bind mounts ensure data survives container restarts
5. **Automated setup**: Makefile and scripts handle the entire setup process

## Resources

### Documentation
- [Docker Documentation](https://docs.docker.com/)
- [Docker Compose Documentation](https://docs.docker.com/compose/)
- [NGINX Documentation](https://nginx.org/en/docs/)
- [WordPress Documentation](https://wordpress.org/support/)
- [MariaDB Documentation](https://mariadb.org/documentation/)
- [WP-CLI Documentation](https://wp-cli.org/)

### Tutorials & Articles
- [Docker Best Practices](https://docs.docker.com/develop/dev-best-practices/)
- [Dockerfile Best Practices](https://docs.docker.com/develop/develop-images/dockerfile_best-practices/)
- [Docker Security](https://docs.docker.com/engine/security/)

## License

This project is part of the 42 school curriculum and follows its academic guidelines.
