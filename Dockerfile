FROM centos:7

# basic build tools
RUN yum clean metadata \
  && yum -y update \
  && yum -y install \
    gcc \
    make \
    wget \
    unzip \
    rpm-build \
    rpmdevtools

# add PAM sources
RUN mkdir /root/rpmbuild
COPY . /root/rpmbuild/

# install build dependencies specific to PAM
RUN spectool --get-files -C /root/rpmbuild/SOURCES/ /root/rpmbuild/SPECS/pam.spec
RUN yum-builddep -y /root/rpmbuild/SPECS/pam.spec

# build PAM
ENTRYPOINT rpmbuild --define "_topdir /root/rpmbuild" -bb /root/rpmbuild/SPECS/pam.spec
