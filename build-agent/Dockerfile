
FROM debian:buster

RUN apt-get update && apt-get install -y clang clang-tidy libc++-7-dev libc++abi-dev libgl1-mesa-dev cmake git curl unzip python3 python3-pip xsltproc
RUN pip3 install conan
RUN curl https://sonarcloud.io/static/cpp/build-wrapper-linux-x86.zip -O && unzip build-wrapper-linux-x86.zip -d /opt && rm build-wrapper-linux-x86.zip

ENV PATH="/opt/build-wrapper-linux-x86:${PATH}"
