FROM jaegertracing/all-in-one@sha256:f7897513501bb9c593c71d5fca11e20891e4942e25f877e9cead78711f43a0be
HEALTHCHECK \
    --interval=1s \
    --timeout=1s \
    --start-period=1s \
    --retries=60 \
    CMD wget -q --header='Content-Type:application/json' -O - http://localhost:14269/health | grep "Server available"
