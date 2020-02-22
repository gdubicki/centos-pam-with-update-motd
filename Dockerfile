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

# add PAM original sources
RUN mkdir /root/rpmbuild
COPY SOURCES /root/rpmbuild/SOURCES
COPY SPECS /root/rpmbuild/SPECS

# install build dependencies specific to PAM
RUN spectool --get-files -C /root/rpmbuild/SOURCES/ /root/rpmbuild/SPECS/pam.spec
RUN yum-builddep -y /root/rpmbuild/SPECS/pam.spec
