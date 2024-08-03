FROM debian:latest
RUN apt-get update
RUN apt-get -y install make g++ libcurl4-openssl-dev

WORKDIR /app
COPY . /app/

RUN make
