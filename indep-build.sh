#!/bin/bash
set -e

cd /tmp
sudo rm -rf omnipkg-app

git clone https://github.com/maibloom/omnipkg-app.git
cd omnipkg-app || { echo "Failed to enter directory"; exit 1; }

sudo chmod +x *

if sudo bash build.sh; then
  echo "Omnipkg has been installed successfully."
else
  echo "Omnipkg installation failed."
fi
