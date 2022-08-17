FROM ubuntu 

WORKDIR /usr/src/simple-git-lfs-server

COPY . .

# Install build dependencies.
RUN apt-get update && apt-get install -y g++ make libpthread-stubs0-dev libssl-dev git git-lfs

EXPOSE 8080

RUN make -j4
