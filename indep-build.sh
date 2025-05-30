sudo rm -rf cd /tmp/omnipkg-app/

sudo pacman -S git --noconfirm

sudo git clone https://github.com/maibloom/omnipkg-app /tmp/

cd /tmp/omnipkg-app/

sudo chmod +x build.sh

sudo bash build.sh
