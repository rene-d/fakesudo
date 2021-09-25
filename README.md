# fakesudo

fakesudo is a little replacement for sudo.

Sometimes, you have to run Docker images with a particular user id (probably the current one), especially if the Docker daemon is rootfull. You cannot use the regular [sudo](https://www.sudo.ws) program since this user id is not in `/etc/passwd`. fakesudo is the solution.

All options are silently ignored.

## Usage

Do not install sudo package. Instead add the following lines in the Dockerfile (gcc is required):

```dockerfile
COPY fakesudo.c /tmp
RUN cc -o /usr/bin/sudo /tmp/fakesudo.c && \
    chmod 4755 /usr/bin/sudo && \
    rm /tmp/fakesudo.c
```

## Example

```bash
docker build -t test .

docker run --rm -u $UID:$GID test sh -c 'sudo apt-get update && sudo apt-get install -qq --no-install-recommends moreutils && id | ts'
```
