FROM brainboxdotcc/dpp:latest

WORKDIR /usr/src/capbot

COPY . .

WORKDIR /usr/src/capbot/build

RUN apt-get clean && apt-get update && apt-get upgrade

# locale is for my FormatWithCommas function
RUN apt-get install -y locales libboost-all-dev wget build-essential && \
    locale-gen en_US.UTF-8 && \
    update-locale
RUN apt-get install -y software-properties-common && \
    add-apt-repository ppa:ubuntu-toolchain-r/test

RUN apt-get install -y --reinstall ca-certificates && \
    mkdir /usr/local/share/ca-certificates/cacert.org && \
    wget -P /usr/local/share/ca-certificates/cacert.org http://www.cacert.org/certs/root.crt http://www.cacert.org/certs/class3.crt && \
    update-ca-certificates && \
    git config --global http.sslCAinfo /etc/ssl/certs/ca-certificates.crt

RUN apt-get -qq update; \
    apt-get install -qqy --no-install-recommends \
        gnupg2 wget ca-certificates apt-transport-https \
        autoconf automake cmake dpkg-dev file make patch libc6-dev

# Install LLVM
RUN echo "deb https://apt.llvm.org/focal/ llvm-toolchain-focal-14 main" \
        > /etc/apt/sources.list.d/llvm.list && \
    wget -qO /etc/apt/trusted.gpg.d/llvm.asc \
        https://apt.llvm.org/llvm-snapshot.gpg.key && \
    apt-get -qq update && \
    apt-get install -qqy -t llvm-toolchain-focal-14 clang-14 clang-tidy-14 clang-format-14 lld-14 libc++-14-dev libc++abi-14-dev && \
    for f in /usr/lib/llvm-14/bin/*; do ln -sf "$f" /usr/bin; done && \
    rm -rf /var/lib/apt/lists/*

ENV CC=clang-14
ENV CXX=clang++-14

ENV LANG en_US.UTF-8
ENV LANGUAGE en_US:en
ENV LC_ALL en_US.UTF-8

RUN cmake ..
RUN make -j$(nproc)

CMD sh /usr/src/capbot/docker.sh