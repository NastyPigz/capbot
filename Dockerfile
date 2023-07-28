FROM brainboxdotcc/dpp:latest

WORKDIR /usr/src/capbot

COPY . .

WORKDIR /usr/src/capbot/build

RUN apt-get clean && apt-get update && apt-get upgrade
RUN apt-get install -y software-properties-common && \
    add-apt-repository ppa:ubuntu-toolchain-r/test
# RUN add-apt-repository 'deb http://mirrors.kernel.org/ubuntu hirsute main universe'

# locale is for my FormatWithCommas function
RUN apt-get install -y locales libboost-all-dev wget build-essential && \
    locale-gen en_US.UTF-8 && \
    update-locale

# RUN apt-get install -y gcc-11 g++-11 libc++-dev libc++abi-dev

RUN wget https://apt.llvm.org/llvm.sh && \
    chmod +x llvm.sh && \
    ./llvm.sh 14 all

RUN apt-get install -y --reinstall ca-certificates && \
    mkdir /usr/local/share/ca-certificates/cacert.org && \
    wget -P /usr/local/share/ca-certificates/cacert.org http://www.cacert.org/certs/root.crt http://www.cacert.org/certs/class3.crt && \
    update-ca-certificates && \
    git config --global http.sslCAinfo /etc/ssl/certs/ca-certificates.crt

ENV CC=clang-14
ENV CXX=clang++-14
# ENV CXXFLAGS=-stdlib=libstdc++
# ENV LDFLAGS=-stdlib=libstdc++
ENV LANG en_US.UTF-8
ENV LANGUAGE en_US:en
ENV LC_ALL en_US.UTF-8

RUN cmake ..
# RUN clang++-14 -v && sleep 2 && sleep 5
# CMD g++ -v
RUN make -j$(nproc)

# CMD g++ -v

CMD sh /usr/src/capbot/docker.sh