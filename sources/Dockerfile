FROM ubuntu:16.04
MAINTAINER Kazuma Arino <hogeika2@gmail.com>

# Install git, wget, bc and dependencies
RUN apt-get update && apt-get install -y \
  git \
  iproute2 \
  wget \
  python3.5 \
  python3-pip \
  python3-dev
  
# Install tensorflow and basics
# ENV TF_PYTHON_URL https://storage.googleapis.com/tensorflow/linux/gpu/tensorflow_gpu-1.1.0-cp35-cp35m-linux_x86_64.whl
# RUN pip3 install $TF_PYTHON_URL \

RUN pip3 install tensorflow \
    jupyter \
    pandas \
    scikit-learn \
    matplotlib \
    scikit-image \
    pillow \
    tqdm

# Install Keras and its dependencies
RUN pip3 install h5py \
    keras


RUN useradd docker -u 1001 -s /bin/bash -m
USER docker

# Set working directory
WORKDIR /work

ENTRYPOINT ["/bin/bash"]

