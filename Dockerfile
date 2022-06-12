FROM brainboxdotcc/dpp:latest

WORKDIR /usr/src/capbot

COPY . .

WORKDIR /usr/src/capbot/build

RUN cmake ..
RUN make -j$(nproc)

CMD sh /usr/src/capbot/docker.sh