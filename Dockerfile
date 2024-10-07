ARG DOCKER_IMAGE=alpine:3.19
FROM $DOCKER_IMAGE AS dev

ENV LUAJIT_VERSION v2.1

RUN apk add --no-cache git build-base cmake curl-dev zlib-dev zstd-dev \
		sqlite-dev postgresql-dev hiredis-dev leveldb-dev \
		gmp-dev jsoncpp-dev ninja ca-certificates

WORKDIR /usr/src/
RUN git clone --recursive https://github.com/jupp0r/prometheus-cpp && \
		cd prometheus-cpp && \
		cmake -B build \
			-DCMAKE_INSTALL_PREFIX=/usr/local \
			-DCMAKE_BUILD_TYPE=Release \
			-DENABLE_TESTING=0 \
			-GNinja && \
		cmake --build build && \
		cmake --install build && \
	cd /usr/src/ && \
	git clone --recursive https://github.com/libspatialindex/libspatialindex && \
		cd libspatialindex && \
		cmake -B build \
			-DCMAKE_INSTALL_PREFIX=/usr/local && \
		cmake --build build && \
		cmake --install build && \
	cd /usr/src/ && \
	git clone --recursive https://luajit.org/git/luajit.git -b ${LUAJIT_VERSION} && \
		cd luajit && \
		make amalg && make install && \
	cd /usr/src/

FROM dev as builder

COPY .git /usr/src/aperosvoxel/.git
COPY CMakeLists.txt /usr/src/aperosvoxel/CMakeLists.txt
COPY README.md /usr/src/aperosvoxel/README.md
COPY aperosengine.conf.example /usr/src/aperosvoxel/aperosengine.conf.example
COPY builtin /usr/src/aperosvoxel/builtin
COPY cmake /usr/src/aperosvoxel/cmake
COPY doc /usr/src/aperosvoxel/doc
COPY fonts /usr/src/aperosvoxel/fonts
COPY lib /usr/src/aperosvoxel/lib
COPY misc /usr/src/aperosvoxel/misc
COPY po /usr/src/aperosvoxel/po
COPY src /usr/src/aperosvoxel/src
COPY irr /usr/src/aperosvoxel/irr
COPY textures /usr/src/aperosvoxel/textures

WORKDIR /usr/src/aperosvoxel
RUN cmake -B build \
		-DCMAKE_INSTALL_PREFIX=/usr/local \
		-DCMAKE_BUILD_TYPE=Release \
		-DBUILD_SERVER=TRUE \
		-DENABLE_PROMETHEUS=TRUE \
		-DBUILD_UNITTESTS=FALSE -DBUILD_BENCHMARKS=FALSE \
		-DBUILD_CLIENT=FALSE \
		-GNinja && \
	cmake --build build && \
	cmake --install build

FROM $DOCKER_IMAGE AS runtime

RUN apk add --no-cache curl gmp libstdc++ libgcc libpq jsoncpp zstd-libs \
				sqlite-libs postgresql hiredis leveldb && \
	adduser -D aperosvoxel --uid 30000 -h /var/lib/aperosvoxel && \
	chown -R aperosvoxel:aperosvoxel /var/lib/aperosvoxel

WORKDIR /var/lib/aperosvoxel

COPY --from=builder /usr/local/share/aperosvoxel /usr/local/share/aperosvoxel
COPY --from=builder /usr/local/bin/aperosvoxelserver /usr/local/bin/aperosvoxelserver
COPY --from=builder /usr/local/share/doc/aperosvoxel/aperosengine.conf.example /etc/aperosvoxel/aperosengine.conf
COPY --from=builder /usr/local/lib/libspatialindex* /usr/local/lib/
COPY --from=builder /usr/local/lib/libluajit* /usr/local/lib/
USER aperosvoxel:aperosvoxel

EXPOSE 30000/udp 30000/tcp
VOLUME /var/lib/aperosvoxel/ /etc/aperosvoxel/

ENTRYPOINT ["/usr/local/bin/aperosvoxelserver"]
CMD ["--config", "/etc/aperosvoxel/aperosengine.conf"]
