# Dockerfile - Raspberry Pi Pico (RP2040 / RP2350) C/C++ development environment
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive \
    PATH=/usr/bin:$PATH

# Install toolchain and build tools. this will be done as root
RUN apt-get update && apt-get install -y --no-install-recommends \
    git \
    cmake \
    build-essential \
    gcc-arm-none-eabi \
    gdb-multiarch \
    python3 \
    python3-pip \
    ninja-build \
    ccache \
    libusb-1.0-0-dev \
    pkg-config \
    libnewlib-arm-none-eabi \
    libstdc++-arm-none-eabi-newlib \
 && rm -rf /var/lib/apt/lists/*


# Create workspace as root (simpler for CI/CD)
RUN mkdir /root/pico
WORKDIR /root/pico
 
RUN git clone --depth=1 --recursive -b master https://github.com/raspberrypi/pico-sdk.git
ENV PICO_SDK_PATH=/root/pico/pico-sdk
RUN git clone --depth=1 -b master https://github.com/raspberrypi/pico-examples.git
# My branch has support to write directly to the frame buffer
RUN git clone --depth=1 -b direct_to_frame_buffer https://github.com/bozenalabs/pimoroni-pico.git --recursive

RUN python3 -m pip install --no-cache-dir --upgrade pip setuptools wheel
