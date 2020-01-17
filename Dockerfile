# images.Dockerfile

FROM ubuntu:18.04 as lit-builder
# Build image to compile lit program to convert md files to C++ files and html
RUN     apt-get update && \
        apt-get install --yes --no-install-recommends \
            make \
            gdc \
        && apt-get clean && rm -rf /var/lib/apt/lists/*
RUN     mkdir /tmp/Literate
COPY    Literate /tmp/Literate
        WORKDIR /tmp/Literate
RUN     make build
RUN     mkdir -p /tmp/fakeroot/lib && \
        mkdir -p /tmp/fakeroot/bin && \
        mkdir -p /tmp/fakeroot/tmp && \
        tar ch `ldd /tmp/Literate/bin/lit | grep -o '/.\+\.so[^ ]*' | sort | uniq` | (cd /tmp/fakeroot; tar x)


FROM ubuntu:18.04 as builder
# Build image to compile the simulations for the article
WORKDIR /tmp
COPY    main3.md   .
COPY    main2.md   .
COPY    random.h   .
COPY    random.cpp .
RUN     apt-get update && \
        apt-get install --yes --no-install-recommends \
        build-essential \
        libboost-all-dev \
        && apt-get clean && rm -rf /var/lib/apt/lists/*
COPY --from=lit-builder /tmp/Literate/bin/lit /usr/local/bin
COPY --from=lit-builder /tmp/fakeroot /tmp/fakeroot
RUN     cd /tmp/fakeroot ; tar c * | ( cd / ; tar xk ) || true
RUN     lit -t main3.md && \
        lit -t main2.md && \
        g++ -c random.cpp \
        && g++ random.o -o main3 main3.cpp \
        && g++ random.o -o main2 main2.cpp \
RUN \
        mkdir -p /tmp/fakeroot/lib && \
        mkdir -p /tmp/fakeroot/bin && \
        mkdir -p /tmp/fakeroot/tmp && \
        tar ch `ldd /bin/sh | grep -o '/.\+\.so[^ ]*' | sort | uniq` | (cd fakeroot; tar x) && \
        tar ch `ldd main3 | grep -o '/.\+\.so[^ ]*' | sort | uniq` | (cd fakeroot; tar x) && \
        cp /bin/sh fakeroot/bin


FROM scratch AS release
# Build the image for running the simulations, and fill it with the compiled programs
FROM scratch AS release
COPY --from=builder /tmp/fakeroot /
COPY --from=builder /tmp/main3 /tmp
COPY --from=builder /tmp/main2 /tmp
WORKDIR /tmp/
ENTRYPOINT /tmp/main3


