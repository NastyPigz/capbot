FROM brainboxdotcc/dpp:latest

WORKDIR /usr/src/capbot

COPY . .

WORKDIR /usr/src/capbot/build

RUN apt-get clean && apt-get update && apt-get install -y locales libboost-all-dev wget && \
    locale-gen en_US.UTF-8 && \
    update-locale

RUN apt-get install -y --reinstall ca-certificates && \
    mkdir /usr/local/share/ca-certificates/cacert.org && \
    wget -P /usr/local/share/ca-certificates/cacert.org http://www.cacert.org/certs/root.crt http://www.cacert.org/certs/class3.crt && \
    update-ca-certificates && \
    git config --global http.sslCAinfo /etc/ssl/certs/ca-certificates.crt

ENV LANG en_US.UTF-8
ENV LANGUAGE en_US:en
ENV LC_ALL en_US.UTF-8

RUN cmake ..
RUN make -j$(nproc)

CMD sh /usr/src/capbot/docker.sh