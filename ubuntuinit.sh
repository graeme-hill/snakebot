# to run nodejs version with C++ addon
cd ~ && \
apt-get update && \
apt-get -y install vim build-essential git python2.7 && \
ln -s /usr/bin/python2.7 /usr/bin/python && \
wget https://nodejs.org/dist/v9.6.1/node-v9.6.1-linux-x64.tar.xz && \
tar -xf node-v9.6.1-linux-x64.tar.xz && \
rm node-v9.6.1-linux-x64.tar.xz && \
mv node-v9.6.1-linux-x64 ~/node && \
echo "PATH=\$PATH:~/node/bin" >> ~/.bashrc && \
PATH=$PATH:~/node/bin && \
git clone https://bitbucket.org/graemekh/snakebot && \
cd ~/snakebot && \
npm install -g mocha node-gyp && \
npm install && \
node-gyp configure && \
node-gyp build


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
