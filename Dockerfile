FROM node:7.2.1

RUN apt-get update
RUN apt-get install -y \
		build-essential \
		ninja-build\
		python \
    gdb

VOLUME /app

WORKDIR /app
