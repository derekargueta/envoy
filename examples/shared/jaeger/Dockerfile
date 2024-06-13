FROM jaegertracing/all-in-one@sha256:d134df11440bce564c9279929e7baed5b9404c46c3953865ce0684dec07c93ef
HEALTHCHECK \
    --interval=1s \
    --timeout=1s \
    --start-period=1s \
    --retries=60 \
    CMD wget -q --header='Content-Type:application/json' -O - http://localhost:14269/health | grep "Server available"
