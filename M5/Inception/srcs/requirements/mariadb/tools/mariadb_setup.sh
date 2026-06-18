#!/bin/sh

set -e

echo "Starting MariaDB setup..."

# Ensure proper ownership and permissions
chown -R mysql:mysql /var/lib/mysql /run/mysqld /var/log/mysql
chmod 755 /var/lib/mysql /run/mysqld /var/log/mysql

# Initialize database if not exists
if [ ! -d "/var/lib/mysql/mysql" ]; then
    echo "Initializing MariaDB database..."
    
    # Install database (mysql will start a test db by default so we can skip that)
    mysql_install_db --user=mysql --datadir=/var/lib/mysql --skip-test-db
    
    ROOT_PASSWORD=$(cat "${MYSQL_ROOT_PASSWORD_FILE}")
    USER_PASSWORD=$(cat "${MYSQL_PASSWORD_FILE}")
    
    # Validate passwords are not empty
    if [ -z "$ROOT_PASSWORD" ] || [ -z "$USER_PASSWORD" ]; then
        echo "ERROR: Password files are empty"
        exit 1
    fi
    
    echo "Configuring MariaDB with initial data..."
    
    # Use mysqld --bootstrap for initial setup (safer than starting daemon)
    # FLUSH PRIVILEGES ensures that privilege changes take effect immediately by reloading the grant tables from the mysql database.
    # By default, MariaDB/MySQL installations create anonymous users with empty usernames.
    # These users allow anyone to connect to the database without providing credentials
    mysqld --user=mysql --bootstrap << EOF_SQL
USE mysql;
FLUSH PRIVILEGES;

-- Set root password and secure installation
ALTER USER 'root'@'localhost' IDENTIFIED BY '${ROOT_PASSWORD}';
DELETE FROM mysql.user WHERE User='';
DELETE FROM mysql.user WHERE User='root' AND Host NOT IN ('localhost', '127.0.0.1', '::1');
DROP DATABASE IF EXISTS test;
DELETE FROM mysql.db WHERE Db='test' OR Db='test\\_%';

-- Create database and user for WordPress
CREATE DATABASE IF NOT EXISTS \`${MYSQL_DATABASE}\`;
CREATE USER IF NOT EXISTS '${MYSQL_USER}'@'%' IDENTIFIED BY '${USER_PASSWORD}';
GRANT ALL PRIVILEGES ON \`${MYSQL_DATABASE}\`.* TO '${MYSQL_USER}'@'%';
FLUSH PRIVILEGES;
EOF_SQL
    
    echo "MariaDB initialized successfully"
else
    echo "MariaDB database already exists, skipping initialization"
fi

echo "Starting MariaDB server..."
#The exec command replaces the shell process with mysqld, making it PID 1 so Docker can manage it properly.
exec "$@"
