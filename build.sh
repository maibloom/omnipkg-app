#!/bin/bash

# sudo pacman -Sy

# sudo pacman -S git gcc unzip --noconfirm

# 1. Compile the program
echo "Attempting to build omnipkg..."
if gcc *.c -o omnipkg; then
    echo "omnipkg has been built successfully."

    # 2. Install the compiled program
    echo "Attempting to install omnipkg..."
    # Create directory if it doesn't exist
    sudo mkdir -p /usr/local/bin/

    if sudo mv omnipkg /usr/local/bin/omnipkg; then
        echo "omnipkg moved to /usr/local/bin/omnipkg."
        if sudo chmod +x /usr/local/bin/omnipkg; then
            echo "Permissions set for /usr/local/bin/omnipkg."
            echo "omnipkg installed successfully."
            echo "Now, you can use Omni Package." # Modified success message
        else
            echo "Failed to set permissions for /usr/local/bin/omnipkg."
        fi
    else
        echo "Failed to move omnipkg to /usr/local/bin/. It might not exist or there was a permission issue."
    fi
else
    echo "Failed to build omnipkg. Please check for compilation errors."
    # Do not proceed if build fails
fi
