#!/bin/bash

# Inception Project - Secure Secrets Generator
# This script generates and manages all secrets for the Inception project

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

SECRETS_DIR="secrets"

# Function to generate secure random password
generate_password() {
    local length=${1:-16}
    # Use openssl for encryption
    openssl rand -base64 $((length * 3 / 4 + 1)) | tr -d "=+/" | cut -c1-${length}
}

# Function to validate password requirements
validate_password() {
    local password=$1
    local min_length=${2:-8}
    
    if [ ${#password} -lt $min_length ]; then
        echo "Password too short (minimum $min_length characters)"
        return 1
    fi
    
    # Check for at least one letter and one number
    if ! echo "$password" | grep -q '[A-Za-z]' || ! echo "$password" | grep -q '[0-9]'; then
        echo "Password must contain at least one letter and one number"
        return 1
    fi
    
    return 0
}

# Function to create secret file
create_secret() {
    local secret_name=$1
    local secret_file="${SECRETS_DIR}/${secret_name}.txt"
    local password_length=${2:-16}
    
    if [ -f "$secret_file" ]; then
        echo -e "${YELLOW}⚠️  Secret file $secret_file already exists${NC}"
        read -p "Do you want to regenerate it? (y/N): " regenerate
        if [[ ! $regenerate =~ ^[Yy]$ ]]; then
            echo -e "${BLUE}Keeping existing secret${NC}"
            return
        fi
    fi
    
    echo -e "${BLUE}Generating secret: $secret_name${NC}"
    
    # Generate and validate password
    local attempts=0
    local max_attempts=5
    local password
    
    while [ $attempts -lt $max_attempts ]; do
        password=$(generate_password $password_length)
        if validate_password "$password" 8; then
            break
        fi
        attempts=$((attempts + 1))
    done
    
    if [ $attempts -eq $max_attempts ]; then
        echo -e "${RED}❌ Failed to generate valid password after $max_attempts attempts${NC}"
        return 1
    fi
    
    echo "$password" > "$secret_file"
    chmod 600 "$secret_file"
    echo -e "${GREEN}✅ Created $secret_file (${#password} chars)${NC}"
}

# Function to backup existing files
backup_files() {
    local timestamp=$(date +%Y%m%d_%H%M%S)
    
    if [ -d "$SECRETS_DIR" ]; then
        cp -r "$SECRETS_DIR" "${SECRETS_DIR}_backup_${timestamp}"
        echo -e "${BLUE} Backed up secrets directory${NC}"
    else
        echo -e "${YELLOW}  No secrets directory found to backup${NC}"
    fi
}

# Function to show current secret status
show_secret_status() {
    echo -e "${BLUE}Current Secret Status:${NC}"
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    
    local secrets=("db_root_password" "db_password" "credentials" "wp_user_password")
    
    for secret in "${secrets[@]}"; do
        local secret_file="${SECRETS_DIR}/${secret}.txt"
        if [ -f "$secret_file" ]; then
            local file_perm=$(stat -c %a "$secret_file" 2>/dev/null || echo "???")
            local file_size=$(wc -c < "$secret_file" 2>/dev/null || echo "0")
            echo -e "${GREEN}✅${NC} $secret (${file_size} chars, perm: $file_perm)"
        else
            echo -e "${RED}❌${NC} $secret (missing)"
        fi
    done
    echo
}

# Main function
main() {
    echo -e "${GREEN} Inception Project - Secure Secrets Manager${NC}"
    echo -e "${BLUE}═══════════════════════════════════════════════${NC}"
    echo
    
    # Parse flags
    local do_backup=false
    local command=""
    
    for arg in "$@"; do
        case "$arg" in
            --backup|-b)
                do_backup=true
                ;;
            *)
                if [ -z "$command" ]; then
                    command="$arg"
                fi
                ;;
        esac
    done
    
    # Default command if none provided
    command="${command:-status}"
    
    # Create secrets directory if it doesn't exist
    mkdir -p "$SECRETS_DIR"
    
    case "$command" in
        "init"|"generate")
            echo -e "${BLUE}Generating secure secrets...${NC}"
            
            if [ "$do_backup" = true ]; then
                backup_files
            fi
            
            create_secret "db_root_password" 20
            create_secret "db_password" 16  
            create_secret "credentials" 16
            create_secret "wp_user_password" 16
            
            echo -e "${GREEN}🎉 Secrets generation complete!${NC}"
            ;;
            
        "status"|"check")
            show_secret_status
            ;;
            
        "regenerate")
            echo -e "${YELLOW}⚠️  This will regenerate ALL secrets!${NC}"
            echo -e "${RED}WARNING: This will replace all existing secrets!${NC}"
            echo
                if [ "$do_backup" = true ]; then
                    backup_files
                fi
                
                # Regenerate secrets
                rm -f "${SECRETS_DIR}"/*.txt
                create_secret "db_root_password" 20
                create_secret "db_password" 16
                create_secret "credentials" 16  
                create_secret "wp_user_password" 16
            ;;
            
        "backup")
            backup_files
            echo -e "${GREEN}Backup completed${NC}"
            ;;
            
        "help"|"--help"|"-h")
            echo "Usage: $0 [command] [options]"
            echo ""
            echo "Commands:"
            echo "  init|generate  - Generate all secrets (creates new files or overwrites existing)"
            echo "  status|check   - Show current status of secrets"
            echo "  regenerate     - Regenerate all secrets (with confirmation prompt)"
            echo "  backup         - Create backup of existing secrets"
            echo "  help           - Show this help message"
            echo ""
            echo "Options:"
            echo "  --backup, -b   - Create backup before generating/regenerating secrets"
            echo ""
            echo "Examples:"
            echo "  $0 init                    # Generate secrets without backup"
            echo "  $0 init --backup           # Generate secrets with backup"
            echo "  $0 status                  # Check current status"
            echo "  $0 regenerate --backup     # Regenerate with backup"
            ;;
            
        *)
            echo -e "${RED}Unknown command: $command${NC}"
            echo "Use '$0 help' for usage information"
            exit 1
            ;;
    esac
}

# Check if running from correct directory
if [ ! -d "srcs" ]; then
    echo -e "${RED}Error: Must be run from Inception project root directory${NC}"
    echo -e "${BLUE}Current directory: $(pwd)${NC}"
    echo -e "${BLUE}Expected structure: ./srcs/docker-compose.yml${NC}"
    exit 1
fi

main "$@"