FROM buildpack-deps

COPY fakesudo.c /tmp
RUN cc -o /usr/bin/sudo /tmp/fakesudo.c && \
    chmod 4755 /usr/bin/sudo && \
    rm /tmp/fakesudo.c
