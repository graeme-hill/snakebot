# to run pure c++ version
cd ~ && \
apt-get update && \
apt-get -y install vim build-essential git cmake libboost-all-dev && \
git clone https://bitbucket.org/graemekh/snakebot && \
cd ~/snakebot/nonode && \
mkdir build && \
cd build && \
cmake .. && \
make -j4
