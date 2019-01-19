FROM gcc:8.2
COPY . /usr/src/switch
WORKDIR /usr/src/switch
RUN ./make.sh