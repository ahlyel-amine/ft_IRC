FROM debian:latest
RUN apt-get update
RUN apt-get -y install python3 python3-pip make g++
RUN apt-get update

WORKDIR /app
COPY src /app/src/
COPY test.py /app/
COPY Dockerfile /app/
COPY docker_ins.sh /app/
COPY inc /app/inc/
COPY Makefile /app

RUN make
EXPOSE 6666

# ENTRYPOINT ["./bin/ircserv", "6666", "password"]