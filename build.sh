#!/bin/bash

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

            # 3. Now, proceed with Yay installation if omnipkg was installed
            echo "Proceeding with the installation of Yay..."
            # Ensure system is up to date and install dependencies
            sudo pacman -Syu --noconfirm base-devel git --needed # --noconfirm for automation, --needed to only install missing
            if [ $? -eq 0 ]; then # Check if pacman command was successful
                # Clone, build, and install yay
                # It's better to do this in a temporary directory or handle cleanup
                git clone https://aur.archlinux.org/yay.git /tmp/yay # Clone to a temp location
                cd /tmp/yay
                if makepkg -si --noconfirm; then # --noconfirm for makepkg
                    echo "Yay build and installation process initiated."
                    # Verify Yay installation
                    if yay --version; then
                        echo "Installation of Yay was successful. Now, you can use Omni Package."
                    else
                        echo "Installation of Yay was NOT successful. please try reinstalling it manually."
                    fi
                else
                    echo "Failed to build or install Yay."
                fi
                cd - # Go back to previous directory
                # sudo rm -rf /tmp/yay # Optional: cleanup
            else
                echo "Failed to update system or install base-devel/git. Aborting Yay installation."
            fi
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

