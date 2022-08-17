FROM ubuntu 

# Install build dependencies.
RUN apt-get update && apt-get install -y g++ make libpthread-stubs0-dev libssl-dev git git-lfs

# Clone the repo.
CMD cd /usr/src
RUN git clone https://github.com/abc579/simple-git-lfs-server.git

WORKDIR /usr/src/simple-git-lfs-server