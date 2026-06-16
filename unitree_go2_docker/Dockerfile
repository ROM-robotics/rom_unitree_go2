FROM ubuntu:22.04

# Prevent interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install build dependencies
RUN apt-get update && apt-get install -y \
    git \
    cmake \
    g++ \
    vim \
    tmux tmuxinator \
    build-essential \
    libyaml-cpp-dev \
    libeigen3-dev \
    libboost-all-dev \
    libspdlog-dev \
    libfmt-dev \
    && rm -rf /var/lib/apt/lists/*

# Create user mr_robot with sudo privileges
RUN useradd -m -s /bin/bash mr_robot \
    && apt-get update && apt-get install -y sudo \
    && rm -rf /var/lib/apt/lists/* \
    && echo "mr_robot ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers

USER mr_robot
WORKDIR /home/mr_robot

# Clone unitree_sdk2 at tag 2.0.2 into mr_robot's home
RUN git clone --depth 1 --branch 2.0.2 \
    https://github.com/ROM-robotics/unitree_sdk2.git

# Build and install unitree_sdk2 to /opt/unitree_robotics
WORKDIR /home/mr_robot/unitree_sdk2
RUN mkdir build && cd build \
    && cmake .. -DCMAKE_INSTALL_PREFIX=/opt/unitree_robotics \
    && make -j$(nproc) \
    && sudo make install

# Register installed shared libraries with the dynamic linker
RUN sudo bash -c 'echo "/opt/unitree_robotics/lib" > /etc/ld.so.conf.d/unitree.conf && ldconfig'

# Export unitree_sdk2 cmake config path so find_package() works
ENV CMAKE_PREFIX_PATH=/opt/unitree_robotics


# Make unitree headers/libs available to gcc/g++ without explicit CMake find_package
ENV CPATH=/opt/unitree_robotics/include:/opt/unitree_robotics/include/ddscxx
ENV LIBRARY_PATH=/opt/unitree_robotics/lib
ENV LD_LIBRARY_PATH=/opt/unitree_robotics/lib

# Mount point for rom_developers workspace
RUN sudo mkdir -p /home/mr_robot/rom_developers \
    && sudo chown mr_robot:mr_robot /home/mr_robot/rom_developers
WORKDIR /home/mr_robot/rom_developers

# Entrypoint: copies SDK examples into mounted workspace on first run
COPY --chown=mr_robot:mr_robot unitree_go2/entrypoint.sh /entrypoint.sh
RUN sudo chmod +x /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]
CMD ["/bin/bash"]
