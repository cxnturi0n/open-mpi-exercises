FROM alpine:3.16.0

RUN apk update \
      && apk upgrade \
      && apk add --no-cache ncurses build-base libatomic gfortran git valgrind perl linux-headers openssh bash 

#### INSTALL OPENMPI ####
# Source is available at https://www.open-mpi.org/software/ompi/

# Build Options:
# These options are passed to the steps below
ARG OPENMPI_VERSION="4.1.4"
ARG OPENMPI_MAJOR_VERSION="v4.1"
ARG OPENMPI_CONFIGURE_OPTIONS
ARG OPENMPI_MAKE_OPTIONS="-j4"

# Download, build, and install OPENMPI
RUN mkdir /tmp/openmpi-src
WORKDIR /tmp/openmpi-src
RUN wget https://download.open-mpi.org/release/open-mpi/${OPENMPI_MAJOR_VERSION}/openmpi-${OPENMPI_VERSION}.tar.gz \
      && tar xfz openmpi-${OPENMPI_VERSION}.tar.gz
RUN cd openmpi-${OPENMPI_VERSION} && ./configure ${OPENMPI_CONFIGURE_OPTIONS}
RUN cd openmpi-${OPENMPI_VERSION} && make all ${OPENMPI_MAKE_OPTIONS}
RUN cd openmpi-${OPENMPI_VERSION} && make install
RUN rm -rf /tmp/openmpi-src

#### CLEAN UP ####
WORKDIR /
RUN rm -rf /tmp/*

#### ADD DEFAULT USER ####
ARG USER=mpi
ENV USER ${USER}
RUN adduser -D ${USER}

ENV USER_HOME /home/${USER}
RUN chown -R ${USER}:${USER} ${USER_HOME}

# #### CREATE WORKING DIRECTORY FOR USER ####
# ARG WORKDIR=/project
# ENV WORKDIR ${WORKDIR}
# ENV ENV=${WORKDIR}/.rc
# RUN mkdir ${WORKDIR}

# WORKDIR ${WORKDIR}

# ADD src src
# ADD include include
# ADD test test

# RUN git clone https://github.com/bats-core/bats-core.git 

# RUN chown -R ${USER}:${USER} ${WORKDIR}

# RUN echo 'export PS1="\[$(tput bold)\]\[\033[38;5;196m\]\u\[$(tput sgr0)\]\[\033[38;5;20m\]@\[$(tput sgr0)\]\[\033[38;5;27m\]\h\[$(tput sgr0)\]\[\033[38;5;33m\][\[$(tput sgr0)\]\[\033[38;5;45m\]\w\[$(tput sgr0)\]\[\033[38;5;33m\]]\[$(tput sgr0)\]\[$(tput bold)\]\[\033[38;5;46m\]>"' >> ${WORKDIR}/.rc

# USER ${USER}