# Centos 7 PAM with dynamic MOTD support (update-motd from Ubuntu)

☝

## Status️

It works! 🎉

## Installing

```
yum install https://github.com/gdubicki/centos-pam-with-update-motd/releases/download/1.1.8-1022.3/pam-1.1.8-1022.el7.x86_64.rpm
```

## Using

1. Delete the default `/etc/motd`.
2. Optionally add/change `PrintLastLog no` in your `/etc/ssh/sshd_config`, reload `sshd` service.
3. Add this line to your `/etc/pam.d/sshd`: `session    optional     pam_motd.so motd=/run/motd.dynamic`.
4. Add some scripts to generate your dynamic MOTD in `/etc/update-motd.d`.

Test it by SSHing to your machine.

## TODO

(Points 1. & 2. that were here are done!)

3. Add a build pipeline running in GitHub (Travis? Circle CI? Note to self: building in DockerHub
seems to not work, avoid.)

## Development

### Prerequisites

* Docker

### 1. Clone this repo

Obviously.

### 2. Clone the debianutils package with run-parts

Let's use the branch of the latest as of now LTS version of Ubuntu, 16.04 (Xenial Xerus):
```
cd debian-run-parts-for-centos/
git clone --single-branch --branch ubuntu/xenial https://git.launchpad.net/ubuntu/+source/debianutils
```

### 3. Build the run-parts' build environment

This step is **optional** as I have pushed `gdubicki/debian-run-parts-for-centos` Docker image to Dockerhub,
so you can just pull it in the next step.
 
```
# assuming that you still are in debian-run-parts-for-centos/ subdir
docker build --tag gdubicki/debian-run-parts-for-centos .
```

### 4. Build run-parts

(About that `autoreconf`: Debian sources contain files generated by autoconf 1.16 while
Centos 7 contains autoconf 1.13, which is incompatible with them. Therefore we need to
run autoheader, aclocal, automake, autopoint and libtoolize as required using this command.
Source: https://stackoverflow.com/a/33286344/2693875)
```
# assuming that you still are in debian-run-parts-for-centos/ subdir
docker run -it \
-v $(pwd)/bin:/root/bin \
-v $(pwd)/Makefile.am:/root/debianutils/Makefile.am \
gdubicki/debian-run-parts-for-centos:latest \
/bin/bash -c "cd /root/debianutils ; autoreconf -f -i ; ./configure ; make ; cp run-parts /root/bin/"
```

Created binary file will be placed in `bin` subdirectory.

### 5. Clone the Centos 7 PAM package repo

In this case `c7` is the branch of Centos 7:
```
# cd to update-motd subdirectory of this repo 
git clone --single-branch --branch c7 https://git.centos.org/rpms/pam.git 
```

### 6. Build the build environment

This step is **optional** as I have pushed `gdubicki/centos-pam-with-update-motd` Docker image to Dockerhub,
so you can just pull it in the next step.
 
```
# assuming that you are are in update-motd/ subdir
docker build --tag gdubicki/centos-pam-with-update-motd .
```

### 7. Copy run-parts to update-motd

```
# assuming that you still are in update-motd/ subdir
cp ../debian-run-parts-for-centos/bin/run-parts bin/run-parts-debian
```

### 8. Build the PAM RPM

```
# assuming that you still are in update-motd/ subdir
docker run -it \
-v $(pwd)/bin/run-parts-debian:/root/rpmbuild/SOURCES/run-parts-debian \
-v $(pwd)/pam.spec:/root/rpmbuild/SPECS/pam.spec \
-v $(pwd)/pam-1.1.8-update-motd.patch:/root/rpmbuild/SOURCES/pam-1.1.8-update-motd.patch \
-v $(pwd)/rpmbuild/RPMS:/root/rpmbuild/RPMS \
gdubicki/centos-pam-with-update-motd:latest \
/bin/bash -c "spectool --get-files -C /root/rpmbuild/SOURCES/ /root/rpmbuild/SPECS/pam.spec ; rpmbuild --define '_topdir /root/rpmbuild' -bb /root/rpmbuild/SPECS/pam.spec"
```

Created RPMs will be places in the `RPMS` directory.

## License

run-parts as a part of debianutils package is distributed under [GPL v2](./COPYING).

PAM license is attached in the [pam.LICENSE](./pam.LICENSE) file. 

update-motd patch has been written by Dustin Kirkland <kirkland@canonical.com>, probably under [GPL v2](./COPYING).
Downloaded from: https://git.launchpad.net/ubuntu/+source/pam/tree/debian/patches-applied/update-motd

Centos PAM package is also licensed under [GPL v2](./COPYING).

So this code is also licensed under [GPL v2](./COPYING) ...right? 😅
