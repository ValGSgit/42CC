# Developer Documentation

This guide explains how to set up, build, and manage the Inception project as a dev.

## Prerequisites

### System Requirements
- Linux-based operating system (Debian, Ubuntu, or similar)
- Minimum 2GB RAM
- 10GB free disk space
- Internet connection for downloading packages

### Required Software

1. **Docker** (version 20.10 or higher)
   ```bash
   # Check version
   docker --version
   ```

2. **Docker Compose** (version 2.0 or higher)
   ```bash
   # Check version
   docker compose version
   ```

3. **Make**
   ```bash
   # Install if needed
   sudo apt install make
   ```

4. **Git**
   ```bash
   # Install if needed
   sudo apt install git
   ```

### User Permissions

Add your user to the docker group:
```bash
sudo usermod -aG docker $USER
```

Log out and log back in for changes to take effect.

## Environment Setup from Scratch

### 1. Clone the Repository

```bash
git clone <repository-url>
cd Inception
```

### 2. Configure the Domain

Edit `/etc/hosts` to add local domain resolution:

```bash
sudo sh -c 'echo "127.0.0.1    vagarcia.42.fr" >> /etc/hosts'
```

Verify:
```bash
grep vagarcia.42.fr /etc/hosts
```

### 3. Understand the Directory Structure

```
Inception/
├── Makefile              # Build automation
├── manage_secrets.sh     # Secret generation script
├── secrets/              # Password files (git-ignored)
│   ├── credentials.txt
│   ├── db_password.txt
│   ├── db_root_password.txt
│   └── wp_user_password.txt
└── srcs/
    ├── .env              # Environment variables
    ├── docker-compose.yml
    └── requirements/
        ├── nginx/
        │   ├── Dockerfile
        │   ├── conf/nginx.conf
        │   └── tools/nginx_setup.sh
        ├── wordpress/
        │   ├── Dockerfile
        │   ├── conf/www.conf
        │   └── tools/wordpress_setup.sh
        └── mariadb/
            ├── Dockerfile
            ├── conf/50-server.conf
            └── tools/mariadb_setup.sh
```

### 4. Configure Environment Variables

The `.env` file in `srcs/` contains non-sensitive configuration:

```bash
cat srcs/.env
```

### 5. Generate Secrets

Run the secret generation script:

```bash
./manage_secrets.sh regenerate
```

This creates random passwords in the `secrets/` directory.

## Building and Launching

### Using the Makefile

The Makefile automates the entire workflow:

```bash
# Complete setup: secrets + build + start
make all
# or simply
make

# Build Docker images only
make build

# Start containers (must build first)
make up

# Stop containers
make down

# View logs
make logs

# Check status
make status

# Show info
make info

# Clean containers and images
make clean

# Full cleanup (includes data)
make fclean

# Rebuild everything
make re
```

### Manual Build Process

If you want to understand what the Makefile does:

```bash
# 1. Create data directories
mkdir -p /home/vagarcia/data/wordpress
mkdir -p /home/vagarcia/data/mariadb

# 2. Generate secrets
./manage_secrets.sh regenerate

# 3. Build images
docker compose -f srcs/docker-compose.yml build

# 4. Start containers
docker compose -f srcs/docker-compose.yml up -d
```

## Managing Containers and Volumes

### Container Commands

```bash
# List running containers
docker ps

# List all containers (including stopped)
docker ps -a

# View container logs
docker logs nginx
docker logs wordpress
docker logs mariadb

# Follow logs in real-time
docker logs -f wordpress

# Execute commands in a container
docker exec -it wordpress sh
docker exec -it mariadb mysql -u root -p$(cat secrets/db_root_password.txt)

# Restart a specific container
docker restart wordpress

# Stop a container
docker stop nginx

# Remove a container
docker rm -f nginx
```

### Volume Commands

```bash
# List volumes
docker volume ls

# Inspect volume
docker volume inspect srcs_wordpress_data

# Check volume usage
docker system df -v
```

### Network Commands

```bash
# List networks
docker network ls

# Inspect network
docker network inspect srcs_inception_network

# See which containers are on the network
docker network inspect srcs_inception_network --format='{{range .Containers}}{{.Name}} {{end}}'
```

## Data Persistence

### Where Data Is Stored

The project uses bind mounts to store data on the host:

**WordPress Files:**
- Container path: `/var/www/html`
- Host path: `/home/vagarcia/data/wordpress`
- Contains: WordPress core files, themes, plugins, uploads

**MariaDB Data:**
- Container path: `/var/lib/mysql`
- Host path: `/home/vagarcia/data/mariadb`
- Contains: Database files and tables

### How Data Persists

1. **Bind mounts** directly map host directories to container paths
2. When containers are removed, data remains on the host
3. New containers automatically use existing data
4. Data survives `make down` and `make clean`
5. Only `make fclean` removes the data

### Accessing Data

```bash
# View WordPress files
ls -la /home/vagarcia/data/wordpress

# View database files
ls -la /home/vagarcia/data/mariadb

# Check disk usage
du -sh /home/vagarcia/data/*
```

### Backing Up Data

```bash
# Stop containers
make down

# Create backup
sudo tar -czf backup-$(date +%Y%m%d).tar.gz /home/vagarcia/data

# Restart
make up
```

## Development Workflow

### Making Changes to Services

**1. Modify Dockerfile or configuration:**

```bash
# Edit files in srcs/requirements/
vim srcs/requirements/nginx/conf/nginx.conf
```

**2. Rebuild the specific service:**

```bash
docker compose -f srcs/docker-compose.yml build nginx
```

**3. Restart the service:**

```bash
docker compose -f srcs/docker-compose.yml up -d nginx
```

### Testing Changes

```bash
# Check if containers started successfully
docker ps

# View logs for errors
docker logs nginx

# Test the website
curl -k https://vagarcia.42.fr

# Check TLS configuration
openssl s_client -connect vagarcia.42.fr:443 -tls1_2
```

### Debugging

**Access container shell:**
```bash
docker exec -it wordpress sh
docker exec -it nginx sh
docker exec -it mariadb sh
```

**Inside WordPress container:**
```bash
# Check PHP-FPM status
ps aux | grep php-fpm

# Test database connection
mysql -h mariadb -u vagarcia -p$(cat /run/secrets/db_password)

# Check WordPress files
ls -la /var/www/html
```

**Inside NGINX container:**
```bash
# Test NGINX config
nginx -t

# Check SSL certificates
ls -la /etc/ssl/certs/nginx.crt
ls -la /etc/ssl/private/nginx.key
```

**Inside MariaDB container:**
```bash
# Connect to database
mysql -u root -p$(cat /run/secrets/db_root_password)

# Show databases
mysql -u root -p$(cat /run/secrets/db_root_password) -e "SHOW DATABASES;"
```

## Understanding the Build Process

### Docker Image Layers

Each Dockerfile command creates a layer:

```dockerfile
FROM alpine:3.22          # Base layer
RUN apk add nginx         # Package layer
COPY conf/nginx.conf      # Config layer
```

Layers are cached, so unchanged layers don't rebuild.

### Service Dependencies

Start order (defined in docker-compose.yml):
1. MariaDB starts first
2. WordPress starts after MariaDB
3. NGINX starts after WordPress

Note: `depends_on` only ensures start order, not readiness. WordPress waits for MariaDB to be ready using connection retry logic.

## Troubleshooting

### Build Failures

```bash
# Clean everything and rebuild
make fclean
make

# Check Docker disk space
docker system df

# Clean up old images
docker system prune -a
```

### Permission Issues

```bash
# Fix data directory permissions (if needed)
sudo chown -R $USER:$USER /home/vagarcia/data
```

### Network Issues

```bash
# Recreate network
docker network rm srcs_inception_network
docker network create srcs_inception_network
```

### Port Conflicts

```bash
# Check what's using port 443
sudo lsof -i :443

# Stop Apache if it's running
sudo systemctl stop apache2
sudo systemctl disable apache2
```

## Advanced Commands

```bash
# Force rebuild without cache
docker compose -f srcs/docker-compose.yml build --no-cache

# View resource usage
docker stats

# Remove all stopped containers
docker container prune

# Remove unused images
docker image prune -a

# Complete system cleanup
docker system prune -a --volumes
```

## Reference

- Makefile targets: See Makefile for all available commands
- Docker Compose file: `srcs/docker-compose.yml`
- Environment variables: `srcs/.env`
- Secrets: `secrets/` directory
- Data: `/home/vagarcia/data/`
