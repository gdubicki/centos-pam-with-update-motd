# Centos PAM with (Ubuntu's) update-motd

☝

## Status️

Working! ...almost, see the first TODO below.

## TODO

1. We need to replace `run-parts` script from `crontabs` package with our own
as that one prints the scripts' names before their output, like this:

```
gdubicki@mac ~ $ ssh foo-server
/etc/update-motd.d/10_hello_world:

Hello World
```

2. Get the latest PAM release from Centos git instead of keeping a copy here.

3. Add a build pipeline running in GitHub (Travis? Circle CI? Note to self: building in DockerHub
seems to not work, avoid.)

## Installing

```
yum install https://github.com/gdubicki/centos-pam-with-update-motd/releases/download/1.1.8-1022.1/pam-1.1.8-1022.el7.x86_64.rpm
```

## Using

1. Delete the default `/etc/motd`.
2. Optionally add/change `PrintLastLog no` in your `/etc/ssh/sshd_config`, reload `sshd` service.
3. Add this line to your `/etc/pam.d/sshd`: `session    optional     pam_motd.so motd=/run/motd.dynamic`.
4. Add some scripts to generate your dynamic MOTD in `/etc/update-motd.d`.

Test it by SSHing to your machine.

## Development

### Prerequisites

* Docker

### 1. Clone this repo

Obviously.

### 2. Creating the build environment

This step is **optional** as I have pushed `gdubicki/centos-pam-with-update-motd` Docker image to Dockerhub,
so you can just pull it in the next step.
 
```
docker build --tag gdubicki/centos-pam-with-update-motd .
```

### 3. Building the PAM RPM

```
docker run -it -v $(pwd)/update-motd/pam.spec:/root/rpmbuild/SPECS/pam.spec -v $(pwd)/update-motd/pam-1.1.8-update-motd.patch:/root/rpmbuild/SOURCES/pam-1.1.8-update-motd.patch gdubicki/centos-pam-with-update-motd:latest rpmbuild --define "_topdir /root/rpmbuild" -bb /root/rpmbuild/SPECS/pam.spec
```

Created RPMs will be places in the `RPMS` directory.
