FROM alpine:3.22

#iproute2 is a package for ip, ss, and routing tools
RUN apk update && apk upgrade && \
    apk add --no-cache mariadb mariadb-client iproute2 && \ 
    rm -rf /var/cache/apk/*
#installing packages separately would create multiple layers.

COPY conf/50-server.conf /etc/my.cnf.d/50-server.conf
COPY tools/mariadb_setup.sh /usr/local/bin/mariadb_setup.sh

RUN chmod +x /usr/local/bin/mariadb_setup.sh && \
    mkdir -p /run/mysqld /var/log/mysql /var/lib/mysql && \
    chown -R mysql:mysql /run/mysqld /var/log/mysql /var/lib/mysql && \
    rm -f /etc/my.cnf.d/mariadb-server.cnf

VOLUME ["/var/lib/mysql"]

EXPOSE 3306

ENTRYPOINT ["/usr/local/bin/mariadb_setup.sh"]
CMD ["mysqld", "--user=mysql", "--port=3306", "--bind-address=0.0.0.0"]
# - mysqld: Starts the MySQL/MariaDB daemon (database server)
# - --user=mysql: Runs the database process as the 'mysql' system user for security
# - --port=3306: Binds the database service to port 3306 (default MySQL/MariaDB port)
# - --bind-address=0.0.0.0: Allows connections from any network interface (enables container networking)

