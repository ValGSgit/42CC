#!/bin/sh

set -e

echo "Starting WordPress setup..."

# Wait for MariaDB to be ready with better connection handling
echo "Waiting for MariaDB to be ready..."
max_attempts=30
attempt=0
# -h for user, -P for port, -u and -p for username/password from the env vars
# -e "SELECT 1" is a simple SQL instruction to test if db is responsive
until mysql -h mariadb -P 3306 -u"${MYSQL_USER}" -p"$(cat ${MYSQL_PASSWORD_FILE})" -e "SELECT 1" >/dev/null 2>&1; do
    attempt=$((attempt + 1))
    if [ $attempt -eq $max_attempts ]; then
        echo "ERROR: MariaDB is not responding after $max_attempts attempts"
        exit 1
    fi
    echo "MariaDB is not ready yet, waiting... (attempt $attempt/$max_attempts)"
    sleep 5
done
echo "MariaDB is ready!"

# Download WordPress if not exists
if [ ! -f /var/www/html/wp-config.php ]; then
    echo "Setting up WordPress..."
    
    # Download WordPress
    echo "Downloading WordPress..."
    wget --timeout=30 --tries=3 https://wordpress.org/latest.tar.gz -O /tmp/wordpress.tar.gz
    tar -xzf /tmp/wordpress.tar.gz -C /tmp/
    cp -R /tmp/wordpress/* /var/www/html/
    rm -rf /tmp/wordpress /tmp/wordpress.tar.gz
    
    # Download WP-CLI
    echo "Downloading WP-CLI..."
    wget --timeout=30 --tries=3 https://raw.githubusercontent.com/wp-cli/builds/gh-pages/phar/wp-cli.phar -O /usr/local/bin/wp
    chmod +x /usr/local/bin/wp
    
    # Create wp-config.php with improved configuration for data persistence
    echo "Creating WordPress configuration..."
    php83 /usr/local/bin/wp config create \
        --dbname="${MYSQL_DATABASE}" \
        --dbuser="${MYSQL_USER}" \
        --dbpass="$(cat ${MYSQL_PASSWORD_FILE})" \
        --dbhost=mariadb:3306 \
        --path=/var/www/html \
        --allow-root \
        --extra-php <<'PHP'
define('WP_DEBUG', false);
define('WP_DEBUG_LOG', false);
define('WP_DEBUG_DISPLAY', false);
define('FORCE_SSL_ADMIN', true);
define('WP_HOME', 'https://${DOMAIN_NAME}');
define('WP_SITEURL', 'https://${DOMAIN_NAME}');
define('WP_CACHE', false);
define('DISABLE_WP_CRON', false);
define('AUTOMATIC_UPDATER_DISABLED', true);
define('WP_AUTO_UPDATE_CORE', false);
define('FS_METHOD', 'direct');
if (isset($_SERVER['HTTP_X_FORWARDED_PROTO']) && $_SERVER['HTTP_X_FORWARDED_PROTO'] === 'https') {
    $_SERVER['HTTPS'] = 'on';
}
PHP
    
    # Replace ${DOMAIN_NAME} placeholder in wp-config.php
    sed -i "s/\${DOMAIN_NAME}/${DOMAIN_NAME}/g" /var/www/html/wp-config.php
    
    # Install WordPress
    echo "Installing WordPress..."
    php83 /usr/local/bin/wp core install \
        --url="https://${DOMAIN_NAME}" \
        --title="Inception WordPress" \
        --admin_user="${WP_ADMIN_USER}" \
        --admin_password="$(cat ${WP_ADMIN_PASSWORD_FILE})" \
        --admin_email="${WP_ADMIN_EMAIL}" \
        --path=/var/www/html \
        --allow-root
    
    # Create additional user
    echo "Creating additional WordPress user..."
    php83 /usr/local/bin/wp user create "${WP_USER}" "${WP_USER_EMAIL}" \
        --user_pass="$(cat ${WP_USER_PASSWORD_FILE})" \
        --role=editor \
        --path=/var/www/html \
        --allow-root
    
    # Update WordPress URLs to use HTTPS
    php83 /usr/local/bin/wp option update home "https://${DOMAIN_NAME}" --path=/var/www/html --allow-root
    php83 /usr/local/bin/wp option update siteurl "https://${DOMAIN_NAME}" --path=/var/www/html --allow-root
    
    # Flush rewrite rules
    php83 /usr/local/bin/wp rewrite flush --path=/var/www/html --allow-root
    
    # Set proper permissions
    echo "Setting file permissions..."
    chown -R nobody:nobody /var/www/html
    find /var/www/html -type d -exec chmod 755 {} \;
    find /var/www/html -type f -exec chmod 644 {} \;
    chmod 600 /var/www/html/wp-config.php
    
    echo "WordPress setup completed successfully!"
    echo "Admin credentials:"
    echo "  URL: https://${DOMAIN_NAME}/wp-admin"
    echo "  Username: ${WP_ADMIN_USER}"
    echo "  Password: $(cat ${WP_ADMIN_PASSWORD_FILE})"
    echo ""
    echo "Regular User credentials:"
    echo "  Username: ${WP_USER}"
    echo "  Email: ${WP_USER_EMAIL}"
    echo "  Password: $(cat ${WP_USER_PASSWORD_FILE})"
else
    echo "WordPress already configured, skipping setup"
fi

echo "Starting PHP-FPM..."
exec "$@"
