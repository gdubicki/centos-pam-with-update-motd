# Centos PAM with (Ubuntu's) update-motd

Status: WIP

## Building build envioronment
 
```
docker build --tag gdubicki/centos-pam-with-update-motd .
```


## Building the PAM RPM

```
docker run -it -v $(pwd)/update-motd/pam.spec:/root/rpmbuild/SPECS/pam.spec -v $(pwd)/update-motd/pam-1.1.8-update-motd.patch:/root/rpmbuild/SOURCES/pam-1.1.8-update-motd.patch gdubicki/centos-pam-with-update-motd:latest rpmbuild --define "_topdir /root/rpmbuild" -bb /root/rpmbuild/SPECS/pam.spec
```
