# nginx reverse-proxy image
FROM nginx:1.25

# SSL directory (certs mounted via the `ssl` volume at runtime)
RUN mkdir -p /etc/nginx/conf

# nginx config — rendered from the template by the official image entrypoint
# (envsubst fills in the ${...PORT} vars passed in via compose). The template
# must live directly under /etc/nginx/templates (NOT a conf.d/ subdir) so the
# stock entrypoint renders it to /etc/nginx/conf.d/default.conf and it replaces
# the image's default server block.
COPY conf/nginx.conf /etc/nginx/templates/default.conf.template
