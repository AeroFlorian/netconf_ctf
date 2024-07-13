# Build image =================================================================
FROM ubuntu:20.04 AS builder

LABEL maintainer="florian.salaun@nokia.com"

ARG DEBIAN_FRONTEND=noninteractive
ARG APT_KEY_DONT_WARN_ON_DANGEROUS_USAGE=1

ENV MAKEFLAGS=-j12

#RUN rm /etc/apt/sources.list \
#&& echo 'deb http://airphone-docker-local.artifactory-espoo1.int.net.nokia.com:80/artifactory/security-ubuntu-remote/ubuntu/ focal main restricted' >> /etc/apt/sources.list \
#&& echo 'deb http://airphone-docker-local.artifactory-espoo1.int.net.nokia.com:80/artifactory/security-ubuntu-remote/ubuntu/ focal-updates main restricted' >> /etc/apt/sources.list \
#&& echo 'deb http://airphone-docker-local.artifactory-espoo1.int.net.nokia.com:80/artifactory/security-ubuntu-remote/ubuntu/ focal universe' >> /etc/apt/sources.list \

#seems mirrors are quite bad, better to use official
RUN rm /etc/apt/sources.list \
&& echo 'deb http://archive.ubuntu.com/ubuntu focal main restricted' >> /etc/apt/sources.list \
&& echo 'deb http://archive.ubuntu.com/ubuntu focal-updates main restricted' >> /etc/apt/sources.list \
&& echo 'deb http://archive.ubuntu.com/ubuntu focal universe' >> /etc/apt/sources.list \
&& apt-get update && apt-get install -y \
      tmux \
      tcpdump \
      nmap \
      bison \
      strace \
      lsof \
      gdb \
      supervisor \
      net-tools \
      linuxptp \
      openssh-sftp-server \
      libxml2-utils \
      libxml2-dev \
      git \
      cmake \
      build-essential \
      vim \
      clang-format-9 \
      zip \
      # libyang
      libpcre3-dev \
      libpcre2-dev \
      pkg-config \
      # sysrepo
      libavl-dev \
      libev-dev \
      libcurl4-openssl-dev \
      libprotobuf-c-dev \
      protobuf-c-compiler \
      # netopeer2 \
      libssh-dev \
      libssl-dev \
      # bindings
      swig \
      python3-pip \
      wget \
      iputils-ping \
      iproute2 \
      openssh-client \
      vlan \
      valgrind \
      iptables \
      curl \
      libpam-modules \
      libpam0g-dev \
      bzip2 \
      libbz2-dev \
      lzma \
      lzma-dev \
      liblzma-dev \
      dnsutils \
      isc-dhcp-server \
      net-tools \
      vim.tiny \
      nano \
      openssh-client \
      iptraf \
      libjson-c-dev \
      flex \
      libcap-dev \
      gettext-base \
      libtool \
      gnulib \
      libncurses5-dev \
      libreadline-dev \
      gnutls-dev \
      libelf-dev \
      automake \
      libpcap-dev \
      openssl

RUN apt-get update && apt install software-properties-common -y &&\
    add-apt-repository ppa:deadsnakes/ppa -y &&\
    apt update && \
    apt install python3.10 -y

RUN pip3 install \
      cryptography==3.3.2 \
      rust \
      setuptools_rust \
      ncclient \
      pytest \
      pexpect \
      getmac \
      lxml \
      gcovr


RUN \
      echo "root:root" | chpasswd && \
      adduser --system ctf_netconf && \
      echo "ctf_netconf:netconf" | chpasswd


# use /opt/dev as working directory
RUN mkdir /opt/dev
WORKDIR /opt/dev

# libyang v2.1.148

RUN \
      cd /opt/dev && \
      git -c advice.detachedHead=false clone --branch v2.1.111 https://github.com/CESNET/libyang.git && cd libyang && \
      mkdir build && cd build && \
      cmake -DENABLE_LYD_PRIV=ON -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Debug .. && \
      make -j $(nproc) && \
      make install

# LibSSH 0.10.5 required by netopeer2
RUN \
      cd /opt/dev && \
      git -c advice.detachedHead=false clone --branch libssh-0.10.5 https://git.libssh.org/projects/libssh.git && cd libssh && \
      mkdir build && cd build && \
      cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Debug .. && \
      make -j $(nproc) && \
      make install

RUN find / -name *ssl*


# libnetconf2 3.0.8 --
RUN \
      cd /opt/dev && \
      git -c advice.detachedHead=false clone --branch v2.1.37 https://github.com/CESNET/libnetconf2.git && cd libnetconf2 && \
      mkdir build && cd build && \
      cmake  -DCMAKE_BUILD_TYPE:String="Release" -DCMAKE_INSTALL_PREFIX:PATH=/usr -DENABLE_BUILD_TESTS=OFF .. && \
      make -j $(nproc) && \
      make install

# sysrepo -- 2.2.150
RUN \
      cd /opt/dev && \
      git -c advice.detachedHead=false clone --branch v2.2.105 https://github.com/sysrepo/sysrepo.git && cd sysrepo && \
      mkdir build && cd build && \
      cmake -DCMAKE_INSTALL_PREFIX=/usr -DREPO_PATH=/var/sysrepo/current -DCMAKE_BUILD_TYPE=Debug .. && \
      make -j $(nproc) && \
      make install

# netopeer 2 2.2.13
RUN \
      cd /opt/dev && git -c advice.detachedHead=false clone --branch v2.1.71 https://github.com/CESNET/Netopeer2.git && cd Netopeer2 && \
      mkdir build && cd build && \
      cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr .. && \
      make -j $(nproc) && \
      make install

RUN mkdir -p /opt/dev/challenges

ADD challenges/1_intro_get /opt/dev/challenges/1_intro_get

RUN \
      cd /opt/dev/challenges/1_intro_get/1_intro_get_sw && \
      mkdir build && cd build && \
      cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr .. && \
      make -j $(nproc) && \
      make install && rm -rf /opt/dev/challenges/1_intro_get/1_intro_get_sw

ADD challenges/2_get_filter /opt/dev/challenges/2_get_filter
RUN \
      cd /opt/dev/challenges/2_get_filter/2_get_filter_sw && \
      mkdir build && cd build && \
      cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr .. && \
      make -j $(nproc) && \
      make install && rm -rf /opt/dev/challenges/2_get_filter/2_get_filter_sw

ADD challenges/3_edit_config /opt/dev/challenges/3_edit_config
RUN \
      cd /opt/dev/challenges/3_edit_config/3_edit_config_sw && \
      mkdir build && cd build && \
      cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr .. && \
      make -j $(nproc) && \
      make install && rm -rf /opt/dev/challenges/3_edit_config/3_edit_config_sw

ADD challenges/4_rpc /opt/dev/challenges/4_rpc
RUN \
      cd /opt/dev/challenges/4_rpc/4_rpc_sw && \
      mkdir build && cd build && \
      cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr .. && \
      make -j $(nproc) && \
      make install && rm -rf /opt/dev/challenges/4_rpc/4_rpc_sw

ADD challenges/5_notif /opt/dev/challenges/5_notif
RUN \
      cd /opt/dev/challenges/5_notif/5_notif_sw && \
      mkdir build && cd build && \
      cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr .. && \
      make -j $(nproc) && \
      make install && rm -rf /opt/dev/challenges/5_notif/5_notif_sw

ADD challenges/6_get_out /opt/dev/challenges/6_get_out

RUN \
      cd /opt/dev/challenges/6_get_out/6_get_out_sw && \
      mkdir build && cd build && \
      cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr .. && \
      make -j $(nproc) && \
      make install && rm -rf /opt/dev/challenges/6_get_out/6_get_out_sw

ADD challenges/7_update /opt/dev/challenges/7_update

RUN \
      cd /opt/dev/challenges/7_update/7_update_sw && \
      mkdir build && cd build && \
      cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr .. && \
      make -j $(nproc) && \
      make install && rm -rf /opt/dev/challenges/7_update/7_update_sw

ADD challenges/8_set_full_object /opt/dev/challenges/8_set_full_object

RUN \
      cd /opt/dev/challenges/8_set_full_object/8_set_full_object_sw && \
      mkdir build && cd build && \
      cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr .. && \
      make -j $(nproc) && \
      make install && rm -rf /opt/dev/challenges/8_set_full_object/8_set_full_object_sw

ADD challenges/9_leafref /opt/dev/challenges/9_leafref

RUN \
      cd /opt/dev/challenges/9_leafref/9_leafref_sw && \
      mkdir build && cd build && \
      cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr .. && \
      make -j $(nproc) && \
      make install && rm -rf /opt/dev/challenges/9_leafref/9_leafref_sw

ADD challenges/10_delete /opt/dev/challenges/10_delete

RUN \
      cd /opt/dev/challenges/10_delete/10_delete_sw && \
      mkdir build && cd build && \
      cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr .. && \
      make -j $(nproc) && \
      make install && rm -rf /opt/dev/challenges/10_delete/10_delete_sw

ADD challenges/11_notif_schema /opt/dev/challenges/11_notif_schema

RUN \
      cd /opt/dev/challenges/11_notif_schema/11_notif_schema_sw && \
      mkdir build && cd build && \
      cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr .. && \
      make -j $(nproc) && \
      make install && rm -rf /opt/dev/challenges/11_notif_schema/11_notif_schema_sw

ADD challenges/12_nacm /opt/dev/challenges/12_nacm

RUN \
      cd /opt/dev/challenges/12_nacm/12_nacm_sw && \
      mkdir build && cd build && \
      cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr .. && \
      make -j $(nproc) && \
      make install && rm -rf /opt/dev/challenges/12_nacm/12_nacm_sw

ADD common /opt/dev/common

ENTRYPOINT [ "/opt/dev/common/entrypoint.py" ]