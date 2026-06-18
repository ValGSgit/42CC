FROM alpine:3.22

RUN apk update && apk upgrade && \
    apk add --no-cache \
    php83 \
    php83-fpm \
    php83-mysqli \
    php83-json \
    php83-curl \
    php83-dom \
    php83-exif \
    php83-fileinfo \
    php83-mbstring \
    php83-openssl \
    php83-xml \
    php83-zip \
    php83-phar \
    php83-ctype \
    php83-iconv \
    php83-tokenizer \
    php83-xmlreader \
    php83-xmlwriter \
    php83-simplexml \
    mysql-client \
    wget && \
    rm -rf /var/cache/apk/*

COPY conf/www.conf /etc/php83/php-fpm.d/www.conf
COPY tools/wordpress_setup.sh /usr/local/bin/wordpress_setup.sh

RUN chmod +x /usr/local/bin/wordpress_setup.sh && \
    mkdir -p /var/www/html /var/log/php83 && \
    chown -R nobody:nobody /var/www/html /var/log/php83

WORKDIR /var/www/html

EXPOSE 9000

ENTRYPOINT ["/usr/local/bin/wordpress_setup.sh"]
CMD ["/usr/sbin/php-fpm83", "-F"]
# - -F: Runs php-fpm in foreground mode (prevents container from exiting)
