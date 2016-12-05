FROM ubuntu

RUN apt-get update
RUN apt-get install -y \
		build-essential \
		ninja-build\
		python

VOLUME /app

WORKDIR /app
