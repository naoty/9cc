FROM ubuntu
RUN apt-get update && \
  apt-get -y install gcc make binutils libc6-dev git vim --no-install-recommends && \
  rm -rf /var/lib/apt/lists/*
WORKDIR /9cc
COPY . /9cc
