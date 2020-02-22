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

# debianutils require also this
RUN yum clean metadata \
  && yum -y update \
  && yum -y install \
    automake \
  && yum clean all \
  && rm -rf /var/cache/yum

# add debianutils original sources
RUN mkdir /root/debianutils
COPY debianutils /root/debianutils/

WORKDIR /root/debianutils
