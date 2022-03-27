This solution is **DEPRECATED** because it's hacky, difficult to maintain and may be insecure - it requires you to disable SELinux. See [#2](https://github.com/gdubicki/centos-pam-with-update-motd/issues/2) for more info.

To have a dynamic MOTD using [gdubicki/dynamotd](https://github.com/gdubicki/dynamotd) please see [its installation instructions](https://github.com/gdubicki/dynamotd#installation) using standard PAM package and a built-in `mod_exec` module.

<br/><br/><br/>

# RPM with PAM for Centos 7 with a port of update-motd from Ubuntu

This repo contains the code for ☝️. And the RPM itself is under [releases](https://github.com/gdubicki/centos-pam-with-update-motd/releases).

You may want to read my article about this solution first: ["Dynamic MOTD on Centos 7, like on Ubuntu"](https://medium.com/@GregDubicki/dynamic-motd-on-centos-7-like-on-ubuntu-60d66e1b806d).

Or you may just scroll down and start using this. :)

## Installing from binary

If you trust me then do the following:

```
yum install https://github.com/gdubicki/centos-pam-with-update-motd/releases/download/1.1.8-1023.1/pam-1.1.8-1023.el7.x86_64.rpm
```

If not, then no hard feelings - PAM is a pretty critical part of GNU/Linux, so it is wise to be careful.
Please skip to the [Building your own binary](#building-your-own-binary) below for info how to build your own binary.

## Using

1. Delete the default static `/etc/motd`.
2. Update SSHD config in `/etc/ssh/sshd_config`:
```
# this is required
UsePAM yes

# you most probably don't want to show the default, static MOTD
PrintMotd no
Banner none
# you may also want to hide this for nicer output, although this is useful
PrintLastLog no
```
... & reload `sshd` service.
5. Add this line to your `/etc/pam.d/sshd`:
```
session    optional     pam_motd.so motd=/run/motd.dynamic
```
9. Add some scripts to generate your dynamic MOTD in `/etc/update-motd.d`.
10. Set SELinux to disabled or permissive. (See [#2](https://github.com/gdubicki/centos-pam-with-update-motd/issues/2) for more info.)

Test it by SSHing to your machine.

## Troubleshooting

Try to run the same command as PAM is to generate your dynamic MOTD: `run-parts-debian --lsbsysinit /etc/update-motd.d`
and check if it shows any output.

If yes then please open an issue in this repo, I will try (but not promise) to help.

If not then please check your scripts in `/etc/update-motd.d` for:
* permissions - they should be at least readable and executable by root (`0500`),
* file names - they have to have LSB init-compatible names, good: `10hello`, bad: `10_hello` (!),

If above conditions are not met then **they will fail to run without a warning**! Yeah, it's a pain but that's how it
works in Ubuntu too and we are using the same code for compatibility...

## Building your own binary

If you want to build the RPMs using code in this repo on your machine then please do the following:

### The easy way

Please see [travis.yml](./.travis.yml) for commands which you can reproduce on your localhost or in your CI/CD system.

### The hard way

Below is a full how-to for building each component of this app. 

#### Prerequisites

* Docker

#### 1. Clone this repo

Obviously.

#### 2. Clone the debianutils package with run-parts

Let's use the branch of the latest as of now LTS version of Ubuntu, 16.04 (Xenial Xerus):
```
cd debian-run-parts-for-centos/
git clone --single-branch --branch ubuntu/xenial https://git.launchpad.net/ubuntu/+source/debianutils
```

#### 3. Build the run-parts' build environment (OPTIONAL)

This step is **optional** as I have pushed `gdubicki/debian-run-parts-for-centos` Docker image to Dockerhub,
so you can just pull it in the next step.

```
# assuming that you still are in debian-run-parts-for-centos/ subdir
docker build --tag gdubicki/debian-run-parts-for-centos .
```

#### 4. Build run-parts

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

#### 5. Clone the Centos 7 PAM package repo

In this case `c7` is the branch of Centos 7:
```
# cd to update-motd subdirectory of this repo 
git clone --single-branch --branch c7 https://git.centos.org/rpms/pam.git 
```

#### 6. Build the PAM build environment (OPTIONAL)

This step is **optional** as I have pushed `gdubicki/centos-pam-with-update-motd` Docker image to Dockerhub,
so you can just pull it in the next step.
 
```
# assuming that you are are in update-motd/ subdir
docker build --tag gdubicki/centos-pam-with-update-motd .
```

#### 7. Copy run-parts to update-motd

```
# assuming that you still are in update-motd/ subdir
cp ../debian-run-parts-for-centos/bin/run-parts bin/run-parts-debian
```

#### 8. Build the PAM RPM

```
# assuming that you still are in update-motd/ subdir
docker run -it \
-v $(pwd)/bin/run-parts-debian:/root/rpmbuild/SOURCES/run-parts-debian \
-v $(pwd)/pam.spec:/root/rpmbuild/SPECS/pam.spec \
-v $(pwd)/pam-1.1.8-update-motd.patch:/root/rpmbuild/SOURCES/pam-1.1.8-update-motd.patch \
-v $(pwd)/pam/SOURCES:/root/rpmbuild/SOURCES \
-v $(pwd)/rpmbuild/RPMS:/root/rpmbuild/RPMS \
gdubicki/centos-pam-with-update-motd:latest \
/bin/bash -c "spectool --get-files -C /root/rpmbuild/SOURCES/ /root/rpmbuild/SPECS/pam.spec ; rpmbuild --define '_topdir /root/rpmbuild' -bb /root/rpmbuild/SPECS/pam.spec"
```

Created RPMs will be places in the `RPMS` directory.

## License

run-parts as a part of debianutils package is distributed under [GPL v2](./COPYING).

PAM license is attached in the [pam.LICENSE](./pam.LICENSE) file. 

update-motd patch has been written at [Canonical](https://canonical.com/) by [Dustin Kirkland](https://github.com/dustinkirkland), probably under [GPL v2](./COPYING). The patch has been downloaded from here: https://git.launchpad.net/ubuntu/+source/pam/tree/debian/patches-applied/update-motd

Centos PAM package is also licensed under [GPL v2](./COPYING).

So this code is also licensed under [GPL v2](./COPYING) ...right? 😅
