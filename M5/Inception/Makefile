COMPOSE_FILE = srcs/docker-compose.yml
DATA_DIR = /home/vagarcia/data
DOMAIN_NAME = vagarcia.42.fr
ENV_FILE = srcs/.env

all: check-env keep_your_secrets build up

build: check-env
	@echo "Creating data directories..."
	@mkdir -p $(DATA_DIR)/wordpress
	@mkdir -p $(DATA_DIR)/mariadb
	@docker compose -f $(COMPOSE_FILE) build

up: check-env
	@echo "Starting services..."
	@docker compose -f $(COMPOSE_FILE) up -d
	@echo "WordPress will be available at: https://$(DOMAIN_NAME)"
	@echo "Admin URL: https://$(DOMAIN_NAME)/wp-admin"

down: check-env
	@docker compose -f $(COMPOSE_FILE) down

clean:
	@docker compose -f $(COMPOSE_FILE) down --volumes --remove-orphans
	@docker system prune -af

fclean: clean
	@echo "Removing data directories..."
	@if [ -d "$(DATA_DIR)" ]; then \
		echo "Using Docker container to remove root-owned files..."; \
		docker run --rm -v $(DATA_DIR):/data alpine:3.22 sh -c "rm -rf /data/wordpress/* /data/wordpress/.*" 2>/dev/null || true; \
		docker run --rm -v $(DATA_DIR):/data alpine:3.22 sh -c "rm -rf /data/mariadb/* /data/mariadb/.*" 2>/dev/null || true; \
		echo "Removing empty directories..."; \
		rmdir $(DATA_DIR)/wordpress $(DATA_DIR)/mariadb 2>/dev/null || \
		docker run --rm -v $(DATA_DIR):/data alpine:3.22 sh -c "rmdir /data/wordpress /data/mariadb" 2>/dev/null || true; \
		rmdir $(DATA_DIR) 2>/dev/null || \
		docker run --rm -v $(DATA_DIR):/data alpine:3.22 sh -c "rmdir /data" 2>/dev/null || true; \
		echo "Data directories cleaned successfully"; \
	else \
		echo "No data directories found"; \
	fi
	@docker volume prune -f

fclean-force: clean
	@echo "Data cleanse full gas"
	@sudo rm -rf $(DATA_DIR)
	@docker volume prune -f

re: fclean all

logs:
	@docker compose -f $(COMPOSE_FILE) logs -f

logs-nginx:
	@docker compose -f $(COMPOSE_FILE) logs -f nginx

logs-wordpress:
	@docker compose -f $(COMPOSE_FILE) logs -f wordpress

logs-mariadb:
	@docker compose -f $(COMPOSE_FILE) logs -f mariadb

status:
	@docker compose -f $(COMPOSE_FILE) ps

info: check-env
	@echo "=== Inception Infrastructure Status ==="
	@echo "Domain: $(DOMAIN_NAME)"
	@echo "Data Directory: $(DATA_DIR)"
	@echo "WordPress URL: https://$(DOMAIN_NAME)"
	@echo "Admin URL: https://$(DOMAIN_NAME)/wp-admin"
	@echo ""
	@echo "Container Status:"
	@docker compose -f $(COMPOSE_FILE) ps
	@echo ""
	@echo "To view logs: make logs"
	@echo "To stop: make down"

keep_your_secrets:
	@echo "Initializing secure secrets..."
	@./manage_secrets.sh init

check-env:
		@echo "Checking for .env file..."
		@if [ ! -f "$(ENV_FILE)" ]; then \
			echo "❌ ERROR: .env file not found at $(ENV_FILE)"; \
			echo "Please create the .env file with all required variables."; \
			exit 1; \
		fi
		@echo "✅ .env file found"
		@echo "Validating required environment variables..."
		@missing_vars=""; \
		for var in DOMAIN_NAME MYSQL_DATABASE MYSQL_USER WP_ADMIN_USER WP_ADMIN_EMAIL WP_USER WP_USER_EMAIL; do \
			if ! grep -qE "^$$var=" "$(ENV_FILE)" || [ -z "$$(grep -E "^$$var=" "$(ENV_FILE)" | cut -d'=' -f2-)" ]; then \
				missing_vars="$$missing_vars $$var"; \
			fi; \
		done; \
		if [ -n "$$missing_vars" ]; then \
			echo "❌ ERROR: Required environment variables are missing or empty in .env file:"; \
			echo "Missing:$$missing_vars"; \
			exit 1; \
		fi
		@echo "✅ .env file is OK"


.PHONY: all build up down clean fclean re check_env logs logs-nginx logs-wordpress logs-mariadb status info keep_your_secrets
