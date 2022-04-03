FROM debian

RUN apt-get update && apt-get -y install cmake make g++ cppcheck googletest
