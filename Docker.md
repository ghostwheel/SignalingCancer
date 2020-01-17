@code_type Dockerfile .Dockerfile
@comment_type # %s


# Compiling

The original code it provided as Markdown files. `main2.md` and `main3.md`. These can be converted to C++ using the converter [Literate](https://github.com/zyedidia/Literate). (This program itself is also provided - see [section Reproducability](#Reproducability).

## Markdown to C++

To convert to C++, simply do
```sh
lit main2.md
lit main3.md
```

This will create both the C++ files and html documentation.

## Producing the executables 
To compile, simply do
```sh
g++ -o main2 main2.cpp
g++ -o main3 main3.cpp
```


# Reproducability

For readers to be able to reproduce our results, we use the [docker](http://docker.com)
platform. We supply two "images" that can be run on the reader's
machine: one that can run the programs described, and one on which it
can be edited and compiled.

## Run program on machine that has docker installed
If you are able to run docker locally or remotely, download the image,
and do the following:

```sh
docker load -i docker.cancer-release.tgz
```

Then you can run the 3D simulation using:
### Run program on machine that has docker installed +=
```sh
docker run -it --rm mlachmann/cancer/release:0.1 main3
```
And the 2D simulation with:
### Run program on machine that has docker installed +=
```sh
docker run -it --rm mlachmann/cancer/release:0.1 main2
```

## Run program on machine on which docker is not installed

We provide a CDROM iso image that can run docker. Download the iso
image, and run it. You need to give it access to the image files.

For example, with `qemu` one can currently do the following:

```sh
qemu-system-x86_64 -boot d -cdrom boot2docker.iso  -m 2048 -drive file=fat:rw:DIR,cache=unsafe -redir tcp:2222::22 &
```

where `DIR` is the directory on which the images are located. Then
wait for the machine to boot, and do

### Run program on machine on which docker is not installed +=
```sh
ssh localhost -p 2222 -l docker
```
password is `tcuser`.
### Run program on machine on which docker is not installed +=
```sh
sudo mount /dev/sda1 /mnt
```

Then proceed as above with the image files located at `/mnt`.

## Compiling the Programs

It is possible to compile the programs locally. You need the `boost`
library and a C++ compiler such as `g++`.

### Compile using docker

As above you can use a local docker installation, or the provided
`iso` file. Once docker is running do


```sh
docker load -i compile_file.tar
```

### Compile using docker +=
```sh
docker run --rm RUN_IMAGE /bin/bash
```

This will provide a `shell` prompt in the directory of the source
files in a docker container that has all the tools needed to compile
the files. Simply run `make`.

## Rebuild docker images

The following `Dockerfile` was used to build the docker images.

It consists of three parts:
### images.Dockerfile

```Dockerfile

FROM ubuntu:18.04 as lit-builder
@{Build image to compile lit program to convert md files to C++ files and html}

FROM ubuntu:18.04 as builder
@{Build image to compile the simulations for the article}

FROM scratch AS release
@{Build the image for running the simulations, and fill it with the compiled programs}
```

### Build image to compile lit program to convert md files to C++ files and html

Converting markdown (`.md`) files to C++ and html is done using the
program [lit](http://literate.zbyedidia.webfactional.com). We use a
[branch](https://github.com/robertkovacs/Literate) that converts markdown.
This program is written in D. We construct an ubuntu
image with the D compiler `gcd`.

```Dockerfile
RUN         apt-get update                                \
        &&  apt-get install --yes --no-install-recommends \
                    make                                  \
                    gdc dub                                   \
        && apt-get clean && rm -rf /var/lib/apt/lists/*
COPY    Literate /tmp/Literate
WORKDIR /tmp/Literate
```

Once everything is in place, we compile `lit`. 
##### Build image to compile lit program to convert md files to C++ files and html +=
```Dockerfile
RUN     make build
```
Then we copy the program and all libraries it needs under the
directory `/tmp/fakeroot`. The program `ldd` tells us which libraries
are needed, and the files are copied using `tar`.
##### Build image to compile lit program to convert md files to C++ files and html +=        
```Dockerfile
RUN        mkdir -p /tmp/fakeroot/lib  \
        && mkdir -p /tmp/fakeroot/bin  \
        && mkdir -p /tmp/fakeroot/tmp  \
        && tar ch `ldd /tmp/Literate/bin/lit | grep -o '/.\+\.so[^ ]*' | sort | uniq` | (cd /tmp/fakeroot; tar x)
```

### Build image to compile the simulations for the article
To build the programs for the simulations we construct an ubuntu image
with C and C++ compilers, and the boost library.
```Dockerfile
WORKDIR /tmp
COPY    main3.md main2.md random.h random.cpp  ./
RUN        apt-get update                                \
        && apt-get install --yes --no-install-recommends \
                   g++                                  \
                   libboost-dev                          \
        && apt-get clean                                 \
        && rm -rf /var/lib/apt/lists/*
```
We then copy the `lit` programs and all required libraries from the
`lit-builder` image.
#### Build image to compile the simulations for the article +=
```Dockerfile
COPY --from=lit-builder /tmp/Literate/bin/lit /usr/local/bin
COPY --from=lit-builder /tmp/fakeroot /tmp/fakeroot
RUN     cd /tmp/fakeroot ; tar c * | ( cd / ; tar xk ) || true
```
Now we compile the simulations.
```Dockerfile
RUN        lit -t main3.md                 \
        && lit -t main2.md                 \
        && g++ -c random.cpp               \
        && g++ random.o -o main3 main3.cpp \
        && g++ random.o -o main2 main2.cpp
```
Finally we copy the excutables and all required libraries to a
directory called `/tmp/fakeroot`.
##### Build image to compile the simulations for the article +=
```Dockerfile
RUN \
           mkdir -p /tmp/fakeroot/lib  \
        && mkdir -p /tmp/fakeroot/bin  \
        && mkdir -p /tmp/fakeroot/tmp  \
        && tar ch `ldd /bin/sh | grep -o '/.\+\.so[^ ]*' | sort | uniq` | (cd fakeroot; tar x)  \
        && tar ch `ldd main3 | grep -o '/.\+\.so[^ ]*' | sort | uniq` | (cd fakeroot; tar x)  \
        && cp /bin/sh fakeroot/bin
```


  
### Build the image for running the simulations, and fill it with the compiled programs
The final image just contain the excutables and libraries, copied over
        from the `builder` image.
```Dockerfile
COPY --from=builder /tmp/fakeroot /
COPY --from=builder /tmp/main3 /tmp
COPY --from=builder /tmp/main2 /tmp
WORKDIR /tmp/
ENTRYPOINT /tmp/main3
```

### dot.dockerignore
When docker builds, it checks if files in the directory changed.
By creating the file `.dockerignore` you can tell docker to ignore some files when
building images.
Below, we tell docker to ignore changes all files, except those listed.
```sh
*
!main3.md
!main2.md
!random.cpp
!random.h
!Literate
```

