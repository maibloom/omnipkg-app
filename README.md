# OmniPkg 
OmniPkg redefines package management by offering an innovative approach that includes a flexible installer, live application support, automated configurations, and installation procedures that adhere to the highest best practices—all accomplished with the simplicity of two elegant bash scripts.  

---

## Theory 

In Omnipkg, think of a package as a bag which is a ready-to-go kit for a specific task or setup.

* **What's in a bag?** Anything!
    * A configuration (like ```multilib-mirror``` package that configures Pacman's configuratios.).
    * A group of programs (like ```java-full``` package that installs JDK21, JRE21, Maven, and Gradel).
    * A collection of developer tools.
    * And so on...

* **How does it work?** Each bag has two simple scripts:
    * `install.sh`: Includes the work the bag has been intended to do.
    * `remove.sh`: Undos the work that has been happened. 

For example in the following case:

```
sudo omnipkg put install pypippark
```
omnipkg puts the ```pypippark``` bag down and starts running ```install.sh``` from the bag.

So, a "bag" is just a defined way to install or remove a particular setup using these scripts.

> [!NOTE]
> Packages are stored at [https://www.github.com/maibloom/omnipkg/main/tree/packages](https://www.github.com/maibloom/omnipkg/main/tree/packages)

---

## 🚀 Installation

Get OmniPkg with this command:

```
curl -sSL https://raw.githubusercontent.com/maibloom/omnipkg-app/main/indep-build.sh | bash
```

---


> [!IMPORTANT]
> OmniPkg packages might need you to manually handle some dependencies if your system isn't based on Arch Linux.
> It's a good idea to peek at a bag's install. sh script if you're not on an Arch-based system, just to see what it does.

> [!NOTE]
> Omnipkg will be universally available for most of the GNU/Linux distrobutions soon!

---

## Feedback & Creations

Help OmniPkg flourish! For ideas, bugs, or to contribute code and new "bags," feel free to share!
We warmly welcome all contributions and are excited to build this community with you.
Your input makes OmniPkg better for everyone!