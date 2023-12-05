FROM nvidia/cuda:12.3.0-runtime-ubuntu22.04

ARG REINSTALL_CMAKE_VERSION_FROM_SOURCE="3.28.0-rc5"
ARG NINJA_VERSION="1.11.1"
ARG LLVM_VERSION="17"

RUN apt-get update && \
    apt-get -y install \
    git \
    curl \
    wget \
    build-essential \
    libx11-dev \
    xserver-xorg-dev \
    xorg-dev \
    xcb \
    libxcb-xkb-dev \
    x11-xkb-utils \
    libx11-xcb-dev \
    libxkbcommon-x11-dev \
    pciutils \
    vulkan-tools \
    mesa-utils \
    libsdl2-dev \
    libsdl2-image-dev \
    && rm -rf /var/lib/apt/lists/*

COPY ./reinstall-cmake.sh /tmp/

RUN if [ "${REINSTALL_CMAKE_VERSION_FROM_SOURCE}" != "none" ]; then \
        chmod +x /tmp/reinstall-cmake.sh && /tmp/reinstall-cmake.sh ${REINSTALL_CMAKE_VERSION_FROM_SOURCE}; \
    fi \
    && rm -f /tmp/reinstall-cmake.sh

# Download and install Ninja
RUN wget https://github.com/ninja-build/ninja/archive/v${NINJA_VERSION}.tar.gz && \
    tar xf v${NINJA_VERSION}.tar.gz && \
    cd ninja-${NINJA_VERSION} && \
    cmake -Bbuild-cmake -H. -DCMAKE_BUILD_TYPE=Release && \
    cmake --build build-cmake && \
    mv build-cmake/ninja /usr/local/bin/ninja && \
    cd .. && \
    rm -rf v${NINJA_VERSION}.tar.gz ninja-${NINJA_VERSION}

RUN wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - && \
  echo "deb http://apt.llvm.org/jammy/ llvm-toolchain-jammy-${LLVM_VERSION} main" | tee -a /etc/apt/sources.list && \
  apt-get update && \
  apt-get install -y \
    clang-${LLVM_VERSION} \
    llvm-${LLVM_VERSION} \
    llvm-${LLVM_VERSION}-dev \
    llvm-${LLVM_VERSION}-runtime \
    libc++-${LLVM_VERSION}-dev \
    libc++abi-${LLVM_VERSION}-dev \
    libomp-${LLVM_VERSION}-dev \
    libomp5-${LLVM_VERSION} \
    lldb-${LLVM_VERSION} \
    liblldb-${LLVM_VERSION} \
    clang-tools-${LLVM_VERSION} 

RUN update-alternatives --install /usr/bin/clang clang /usr/bin/clang-${LLVM_VERSION}  100 && \
    update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-${LLVM_VERSION}  100

ARG USERNAME=vscode
ARG USER_UID=1000
ARG USER_GID=$USER_UID

# Create the user
RUN groupadd --gid $USER_GID $USERNAME \
    && useradd --uid $USER_UID --gid $USER_GID -m $USERNAME \
    && apt-get update \
    && apt-get install -y sudo libglew-dev \
    software-properties-common \
    && echo $USERNAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USERNAME \
    && chmod 0440 /etc/sudoers.d/$USERNAME

USER $USERNAME
ENV SHELL /bin/bash

# Needed to share GPU
ENV NVIDIA_DRIVER_CAPABILITIES=all
ENV NVIDIA_VISIBLE_DEVICES=all
