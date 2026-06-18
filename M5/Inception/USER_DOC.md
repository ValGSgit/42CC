# User Documentation

This guide explains how to use the Inception infrastructure as an end user or sys-admin.

## What Services Are Provided

The Inception stack provides a complete WordPress website with the following services:

### 1. **WordPress Website**
- Full-featured content management system
- Accessible at `https://vagarcia.42.fr`
- Supports posts, pages, media uploads, themes, and plugins
- Two user accounts: one administrator and one editor

### 2. **Admin Panel**
- WordPress administration interface
- Accessible at `https://vagarcia.42.fr/wp-admin`
- Manage content, users, settings, and appearance

### 3. **Database**
- MariaDB database storing all WordPress data
- Runs in the background (not directly accessible from outside)
- Data persists in `/home/vagarcia/data/mariadb`

### 4. **Web Server**
- NGINX handling all web traffic
- SSL/TLS encryption for secure connections
- Automatic HTTP to HTTPS redirection

## Starting and Stopping the Project

### Start Everything
```bash
cd /home/vagarcia/Desktop/Inception/Inception
make
```

This will start all services. Wait about 30 seconds for everything to initialize.

### Stop the Services
```bash
make down
```

This stops all containers but keeps your data safe.

### Restart After Stopping
```bash
make up
```

## Accessing the Website

### Main Website
1. Open your web browser
2. Go to: `https://vagarcia.42.fr`
3. You may see a security warning because we use a self-signed certificate - this is normal
4. Click "Advanced" and "Proceed to vagarcia.42.fr"

### Admin Panel
1. Go to: `https://vagarcia.42.fr/wp-admin`
2. Log in with admin credentials (see Credentials section below)
3. You can now manage the website

## Managing Credentials

### Where Credentials Are Stored

All passwords are stored in the `secrets/` directory:

```bash
cd /home/vagarcia/Desktop/Inception/Inception/secrets
```

Files:
- `credentials.txt` - WordPress admin password
- `wp_user_password.txt` - WordPress editor user password
- `db_password.txt` - Database password
- `db_root_password.txt` - Database root password

### Viewing Credentials

```bash
# WordPress admin password
cat secrets/credentials.txt

# Editor user password
cat secrets/wp_user_password.txt
```

### WordPress Users

**Administrator Account:**
- Username: `Superman`
- Password: Check `secrets/credentials.txt`
- Email: `super@vagarcia.42.fr`

**Editor Account:**
- Username: `vagarcia`
- Password: Check `secrets/wp_user_password.txt`
- Email: `user@vagarcia.42.fr`

### Changing Passwords

To generate new passwords:

```bash
./manage_secrets.sh regenerate
```

**Warning**: This will delete all existing data and create new passwords. Only use this if you want to completely reset the installation.

## Checking Services Status

### Quick Status Check
```bash
make status
```

This shows which containers are running.

### Expected Output
```
CONTAINER ID   IMAGE       ... STATUS         NAMES
...            nginx       ... Up X minutes   nginx
...            wordpress   ... Up X minutes   wordpress
...            mariadb     ... Up X minutes   mariadb
```

All three containers should show "Up" status.

### Detailed Information
```bash
make info
```

Shows:
- Domain name
- Container status
- Access URLs
- Data directory locations

### Viewing Logs

**All services:**
```bash
make logs
```

**Specific service:**
```bash
make logs-nginx
make logs-wordpress
make logs-mariadb
```

Press `Ctrl+C` to stop viewing logs.

## Troubleshooting

### Services Won't Start

1. Check if Docker is running:
   ```bash
   docker ps
   ```

2. Check for errors:
   ```bash
   make logs
   ```

3. Try restarting:
   ```bash
   make down
   make up
   ```

### Can't Access Website

1. Verify domain is configured:
   ```bash
   grep vagarcia.42.fr /etc/hosts
   ```
   Should show: `127.0.0.1    vagarcia.42.fr`

2. Check if NGINX is running:
   ```bash
   make status
   ```

3. Check NGINX logs:
   ```bash
   make logs-nginx
   ```

### WordPress Shows Setup Page

If you see the WordPress installation screen instead of the website, the database might not be properly initialized. Check:

```bash
make logs-wordpress
```

Look for "WordPress setup completed successfully" message.

### Database Connection Errors

1. Wait a bit longer - database initialization takes time
2. Check MariaDB logs:
   ```bash
   make logs-mariadb
   ```
3. Verify MariaDB is running:
   ```bash
   make status
   ```

## Data Backup

### Where Data Is Stored

- **WordPress files**: `/home/vagarcia/data/wordpress`
- **Database files**: `/home/vagarcia/data/mariadb`

### Creating a Backup

```bash
# Stop the services first
make down

# Backup data
sudo tar -czf inception-backup-$(date +%Y%m%d).tar.gz /home/vagarcia/data

# Restart services
make up
```

### Restoring from Backup

```bash
# Stop services
make down

# Remove old data
make fclean

# Extract backup
sudo tar -xzf inception-backup-YYYYMMDD.tar.gz -C /

# Restart
make up
```

## Complete Reset

To start fresh with new passwords and empty database:

```bash
make fclean
make
```

**Warning**: This deletes all data including posts, pages, and uploads!
