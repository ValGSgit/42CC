#!/bin/sh

#-e flag tells the script to exit if anything fails
set -e

echo "Setting up NGINX..."

# Create necessary directories
mkdir -p /etc/ssl/private /etc/ssl/certs /run/nginx /var/log/nginx

# Generate SSL certificate with enhanced parameters for better browser compatibility
echo "Generating SSL certificate for ${DOMAIN_NAME}..."

# Create a config file for the certificate with proper extensions
cat > /tmp/cert.conf <<EOF
[req]
default_bits = 4096
prompt = no
default_md = sha256
req_extensions = v3_req
distinguished_name = dn

[dn]
C=AT
ST=Wien
L=Vienna
O=42 School
OU=Student Project
CN=${DOMAIN_NAME}

[v3_req]
basicConstraints = CA:FALSE
keyUsage = nonRepudiation, digitalSignature, keyEncipherment
subjectAltName = @alt_names

[alt_names]
DNS.1 = ${DOMAIN_NAME}
DNS.2 = www.${DOMAIN_NAME}
DNS.3 = localhost
IP.1 = 127.0.0.1
EOF

# Generate the certificate with the config file
# -x509 flag creates a self-signed X.509 certificate
# -nodes (no Data Encryption Standard) flag ensures the private key is not encrypted with a passphrase
# Most certificates use 2048-bit keys, so 4096-bit provides additional security
openssl req -x509 -nodes -days 365 -newkey rsa:4096 \
    -keyout /etc/ssl/private/nginx.key \
    -out /etc/ssl/certs/nginx.crt \
    -config /tmp/cert.conf \
    -extensions v3_req

# Clean up
rm /tmp/cert.conf

# Set proper permissions for SSL files
chmod 600 /etc/ssl/private/nginx.key
chmod 644 /etc/ssl/certs/nginx.crt

# Replace domain placeholder in nginx config
sed -i "s/DOMAIN_NAME_PLACEHOLDER/${DOMAIN_NAME}/g" /etc/nginx/nginx.conf

# Test nginx configuration
nginx -t

echo "NGINX setup completed successfully"

# $@ replaces the current shell process with whatever command was passed to the script
exec "$@"
