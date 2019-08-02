tzarc/nanosrv
===

Simple wrapper around boost.beast's async server. Static binary. Static site.

Usage
===

    FROM busybox:latest as builder

    COPY ./ /www

    RUN find /www -type d -exec chmod 0700 '{}' + \
        && find /www -type f -exec chmod 0600 '{}' +

    FROM tzarc/nanosrv:latest

    ENV PUID=5374 \
        PGID=5374 \
        BIND_ADDRESS=0.0.0.0 \
        BIND_PORT=11253 \
        DOC_ROOT=/www \
        NUM_THREADS=8

    COPY --chown=$PUID:$PGID --from=builder /www /www
