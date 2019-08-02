FROM tzarc/alpine-dev-base:latest as builder

RUN sudo apk --no-cache add gnupg1

ENV TINI_VERSION v0.18.0
ADD https://github.com/krallin/tini/releases/download/${TINI_VERSION}/tini-static-muslc-amd64 /tini
ADD https://github.com/krallin/tini/releases/download/${TINI_VERSION}/tini-static-muslc-amd64.asc /tini.asc
RUN sudo chmod 755 /tini \
    && sudo chmod 644 /tini.asc \
    && sudo chown root:root /tini* \
    && gpg --batch --keyserver hkp://p80.pool.sks-keyservers.net:80 --recv-keys 595E85A6B1B4779EA4DAAEC70B588DFF0527A9B7 \
    && gpg --batch --verify /tini.asc /tini \
    && sudo strip -s /tini \
    && sudo chmod 0700 /tini

COPY --chown=devel:devel src/ /home/devel

RUN g++ -std=c++17 -static -g -O0 -o nanosrv main.cpp -lstdc++fs \
    && strip -s nanosrv \
    && chmod 0700 nanosrv

FROM scratch

COPY --chown=0:0 --from=builder /tini /tini
COPY --chown=0:0 --from=builder /home/devel/nanosrv /nanosrv

CMD ["/tini", "--", "/nanosrv"]