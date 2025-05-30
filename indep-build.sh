cd /tmp/

sudo rm -rf omnipkg-app/

git clone https://www.github.com/maibloom/omnipkg-app

cd omnipkg-app/

sudo chmod +x *

if sudo bash build.sh; then
  echo "Omnipkg has been installed successfully."
else
  echo "Omnipkg installation failed."
