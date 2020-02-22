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
    rpmdevtools \
  && yum clean all \
  && rm -rf /var/cache/yum

# add PAM original sources
RUN mkdir /root/rpmbuild
COPY pam /root/rpmbuild/

# install build dependencies specific to PAM
RUN yum-builddep -y /root/rpmbuild/SPECS/pam.spec
