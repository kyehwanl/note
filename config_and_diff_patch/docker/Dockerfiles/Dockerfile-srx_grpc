############################################################
# Dockerfile to build gRPC enabled NIST-BGP-SRx container 
# Based on CentOS 7
############################################################
FROM centos:7
MAINTAINER "Kyehwan Lee".
ENV container docker


################## BEGIN INSTALLATION ######################
RUN yum -y install epel-release 
RUN yum -y install git golang unzip gcc automake autoconf libtool psmisc readline-devel patch
RUN yum -y install wget libconfig libconfig-devel openssl openssl-devel libcrypto.so.* less gcc uthash-devel

WORKDIR /root
RUN go get -u github.com/golang/protobuf/protoc-gen-go
RUN go get -u google.golang.org/grpc

WORKDIR /tmp
RUN wget https://github.com/protocolbuffers/protobuf/releases/download/v3.6.1/protoc-3.6.1-linux-x86_64.zip
RUN unzip protoc-3.6.1-linux-x86_64.zip
RUN cp bin/protoc /bin/ && cp -rf include/* /usr/include/

RUN git clone https://gitlab.nist.gov/gitlab/kyehwanl/srx_grpc.git /root/go/src/srx_grpc_v6

WORKDIR /root/go/src/srx_grpc_v6
RUN ./configure --prefix=/usr && make all


EXPOSE 2605 179 17900 17901 323 50000
CMD ["sleep", "infinity"]


############# DOCKER RUN command example #####################################
# docker run -ti \
#       -p 179:179 -p 17900:17900 -p 17901:17901 -p 2605:2605 -p 323:323 \
#       -v $PWD/examples/:/etc/ \
#       <docker_image> [command]
##############################################################################
