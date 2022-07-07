FROM brainboxdotcc/dpp:latest

WORKDIR /usr/src/capbot

COPY . .

WORKDIR /usr/src/capbot/build

RUN apt-get clean && apt-get update && apt-get install -y locales libboost-all-dev && \
    locale-gen en_US.UTF-8 && \
    update-locale

ENV LANG en_US.UTF-8
ENV LANGUAGE en_US:en
ENV LC_ALL en_US.UTF-8

RUN cmake ..
RUN make -j$(nproc)

CMD sh /usr/src/capbot/docker.sh