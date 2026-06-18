FROM alpine:3.22

RUN apk update && apk upgrade && \
    apk add --no-cache nginx openssl && \
    rm -rf /var/cache/apk/* && \
    mkdir -p /var/log/nginx

COPY conf/nginx.conf /etc/nginx/nginx.conf
COPY tools/nginx_setup.sh /usr/local/bin/nginx_setup.sh

RUN chmod +x /usr/local/bin/nginx_setup.sh && \
    mkdir -p /var/www/html && \
    chown -R nginx:nginx /var/www/html && \
    chown -R nginx:nginx /var/log/nginx

EXPOSE 80 443

ENTRYPOINT ["/usr/local/bin/nginx_setup.sh"]
CMD ["nginx", "-g", "daemon off;"]

# - nginx: Starts the NGINX web server
# - -g "daemon off;": Runs NGINX in foreground mode (keeps container alive and allows Docker to manage the process)

