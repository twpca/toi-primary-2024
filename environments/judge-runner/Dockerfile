FROM debian:bullseye
SHELL ["bash", "-l", "-c"]

RUN sed -i.bak -e "s%http://deb.debian.org/debian %http://ftp.jp.debian.org/debian %g" /etc/apt/sources.list
RUN apt-get update
RUN apt-get install -y curl zip unzip apt-transport-https ca-certificates software-properties-common
RUN apt-get install -y gcc g++ make openjdk-11-jdk libboost-all-dev \
    python3 python3-distutils python3-setuptools python3-pip python3-dev \
    ssh-client --no-install-recommends
RUN apt-get install -y libcap-dev git --no-install-recommends
RUN pip3 install wheel
RUN pip3 install jsonschema pyyaml toml pypdf2

# kotlin
RUN curl -s "https://get.sdkman.io" | bash
# https://stackoverflow.com/questions/20635472
RUN echo '[[ -s "/root/.sdkman/bin/sdkman-init.sh" ]] && source "/root/.sdkman/bin/sdkman-init.sh"' >> /root/.bash_profile
RUN sdk install kotlin

# isolate
RUN git clone https://github.com/Nekosyndrome/hoj-isolate.git
RUN cd hoj-isolate && make isolate && make install
RUN rm -rf hoj-isolate

VOLUME /workdir
WORKDIR /workdir
