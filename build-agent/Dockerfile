
FROM debian:buster

# General Tools
RUN apt-get update && apt-get install -y llvm clang libc++-7-dev libc++abi-dev git curl unzip python3 python3-pip
# Conan C++ package manager
RUN pip3 install conan
# SonarQube C++ build wrapper
RUN curl https://sonarcloud.io/static/cpp/build-wrapper-linux-x86.zip -O && unzip build-wrapper-linux-x86.zip -d /opt && rm build-wrapper-linux-x86.zip
# CMake (cmake from apt is too old)
RUN cd /tmp && curl https://github.com/Kitware/CMake/releases/download/v3.15.6/cmake-3.15.6.tar.gz -O -L && \
    tar -xf cmake-3.15.6.tar.gz && cd cmake-3.15.6 && CC=clang CXX=clang++ ./bootstrap && make install && rm -rf /tmp/cmake*
# Build requirements
RUN apt-get install -y clang-tidy clang-format libgl1-mesa-dev xsltproc pkg-config libfontenc-dev libice-dev libsm-dev libxaw7-dev libxcomposite-dev

ENV PATH="/opt/build-wrapper-linux-x86:${PATH}"
