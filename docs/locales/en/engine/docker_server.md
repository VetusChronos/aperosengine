# Docker Server

We provide AperosEngine server Docker images using the GitHub container registry.

Images are built on each commit and available using the following tag scheme:

* `ghcr.io/vetuschronos/aperosengine:main` (latest build)
* `ghcr.io/vetuschronos/aperosengine:<tag>` (specific Git tag)
* `ghcr.io/vetuschronos/aperosengine:latest` (latest Git tag, which is the stable release)

See [here](https://github.com/vetuschronos/aperosengine/pkgs/container/aperosengine) for all available tags.

For a quick test you can easily run:

```shell
docker run ghcr.io/vetuschronos/aperosengine:main
```

To use it in a production environment, you should use volumes bound to the Docker host to persist data and modify the configuration:

```shell
docker create -v /home/aperosengine/data/:/var/lib/aperosengine/ -v /home/aperosengine/conf/:/etc/aperosengine/ ghcr.io/aperosengine/aperosengine:main
```

You may also want to use [Docker Compose](https://docs.docker.com/compose):

```yaml
---
version: "2"
services:
  aperosengine_server:
    image: ghcr.io/vetuschronos/aperosengine:main
    restart: always
    networks:
      - default
    volumes:
      - /home/aperosengine/data/:/var/lib/aperosengine/
      - /home/aperosengine/conf/:/etc/aperosengine/
    ports:
      - "30000:30000/udp"
      - "127.0.0.1:30000:30000/tcp"
```

Data will be written to `/home/aperosengine/data` on the host, and configuration will be read from `/home/aperosengine/conf/aperosengine.conf`.

**Note:** If you don't understand the previous commands please read the [official Docker documentation](https://docs.docker.com) before use.
