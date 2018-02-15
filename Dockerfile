FROM archlinux/base:latest

WORKDIR /usr/src/app

COPY package*.json ./

RUN pacman -Syyu --noconfirm
RUN pacman-db-upgrade
RUN pacman -S --noconfirm nodejs npm base-devel python2

# get js packages
RUN npm install

# bundle source
COPY . .

# compile native modules
RUN node-gyp configure build

EXPOSE 5000

CMD [ "npm", "start" ]