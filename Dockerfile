FROM centos:7 as builder

RUN yum install -y epel-release && \
    yum install -y git gcc zeromq-devel hdf5-devel make &&\
    git clone --single-branch --branch use_hdf5 https://github.com/michele-brambilla/neventGenerator.git &&\
    cd neventGenerator && make zmqGenerator


FROM centos:7

RUN yum install -y glibc
COPY --from=builder /lib64/libzmq.so /lib64/
COPY --from=builder /lib64/libhdf5.so.8 /lib64/libhdf5.so.8
COPY --from=builder /lib64/libhdf5.so.8.0.1 /lib64/libhdf5.so.8.0.1
COPY --from=builder /lib64/libzmq.so.5 /lib64/libzmq.so.5
COPY --from=builder /lib64/libzmq.so.5.0.0 /lib64/libzmq.so.5.0.0
COPY --from=builder /lib64/libsodium.so.23 /lib64/libsodium.so.23
COPY --from=builder /lib64/libsodium.so.23.3.0 /lib64/libsodium.so.23.3.0
COPY --from=builder /lib64/libhdf5_hl.so.8 /lib64/libhdf5_hl.so.8 
COPY --from=builder /lib64/libhdf5_hl.so.8.0.1 /lib64/libhdf5_hl.so.8.0.1 
COPY --from=builder /lib64/libsz.so.2 /lib64/libsz.so.2 
COPY --from=builder /lib64/libsz.so.2.0.1 /lib64/libsz.so.2.0.1 
COPY --from=builder /lib64/libpgm-5.2.so.0 /lib64/libpgm-5.2.so.0
COPY --from=builder /lib64/libpgm-5.2.so.0.0.122 /lib64/libpgm-5.2.so.0.0.122
COPY --from=builder /lib64/libaec.so.0 /lib64/libaec.so.0
COPY --from=builder /lib64/libaec.so.0.0.10 libaec.so.0.0.10


#COPY --from=builder /lib64/libsodium* /lib64/
COPY --from=builder neventGenerator/zmqGenerator neventGenerator/zmqGenerator
COPY --from=builder neventGenerator/rita22012n006190.hdf neventGenerator/rita22012n006190.hdf
# COPY builder:
# COPY builder:
# COPY builder:
# COPY builder:
# COPY builder:
# COPY builder:
# COPY builder:
# COPY builder:
# COPY builder:

