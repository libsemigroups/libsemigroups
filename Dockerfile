FROM ubuntu:bionic

MAINTAINER Murray Whyte <mw231@st-andrews.ac.uk>

ENV DEBIAN_FRONTEND noninteractive

ENV LIBSEMIGROUPS_VERSION latest

RUN apt-get update -qq \
  && apt-get install -y \
  automake \
  autoconf \
  build-essential \
  curl \
  git \
  libtool \
  m4 \
  pkg-config \
  sudo 

RUN adduser --quiet --shell /bin/bash --gecos "libsemigroups user,101,," --disabled-password libsemigroups \
    && adduser libsemigroups sudo \
    && chown -R libsemigroups:libsemigroups /home/libsemigroups/ \
    && echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers \
    && cd /home/libsemigroups \
    && touch .sudo_as_admin_successful

RUN git clone https://github.com/libsemigroups/libsemigroups \
    && cd libsemigroups/extern \
    && curl -L -O https://github.com/fmtlib/fmt/archive/5.3.0.tar.gz \
    && tar -xzf 5.3.0.tar.gz \
    && rm -f 5.3.0.tar.gz \
    && cd .. \
    && ./autogen.sh \
    && ./configure \
    && make -j4 \
    && sudo make install \
    && cd / \
    && rm -rf libsemigroups

USER libsemigroups

ENV HOME /home/libsemigroups

WORKDIR home/libsemigroups

