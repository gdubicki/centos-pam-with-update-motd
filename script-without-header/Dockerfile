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

# util-linux extra packages
RUN yum clean metadata \
  && yum -y install \
    gettext \
    gettext-devel \
    libtool \
    ncurses \
    ncurses-devel \
  && yum clean all \
  && rm -rf /var/cache/yum

WORKDIR /root/
