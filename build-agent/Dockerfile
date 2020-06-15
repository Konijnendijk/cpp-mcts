
FROM debian:buster

RUN apt-get update && apt-get install -y clang clang-tidy libqt5core5a libqt5widgets5 qtbase5-dev cmake ninja-build git curl unzip
RUN curl https://sonarcloud.io/static/cpp/build-wrapper-linux-x86.zip -O && unzip build-wrapper-linux-x86.zip -d /opt && rm build-wrapper-linux-x86.zip

ENV PATH="/opt/build-wrapper-linux-x86:${PATH}"