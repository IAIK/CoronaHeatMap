FROM debian:11 AS BUILDER

ENV DEBIAN_FRONTEND noninteractive

# install dependencies
RUN apt-get update && \
    apt-get install -y build-essential cmake git

# copy
COPY . /build
WORKDIR /build

# compile:
RUN rm -rf SEAL build bin && \
    git clone https://github.com/microsoft/SEAL.git && \
    cd SEAL && \
    git checkout 3.6.6 && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make -j 4 && \
    cd ../.. && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make -j 4

# Run in slim docker
FROM debian:11-slim

COPY --from=BUILDER /build/bin /app

WORKDIR /app
CMD ["/bin/bash"]
