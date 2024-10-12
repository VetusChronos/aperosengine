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

COPY .git /usr/src/aperosengine/.git
COPY CMakeLists.txt /usr/src/aperosengine/CMakeLists.txt
COPY README.md /usr/src/aperosengine/README.md
COPY aperosengine.conf.example /usr/src/aperosengine/aperosengine.conf.example
COPY builtin /usr/src/aperosengine/builtin
COPY cmake /usr/src/aperosengine/cmake
COPY doc /usr/src/aperosengine/docs
COPY fonts /usr/src/aperosengine/fonts
COPY lib /usr/src/aperosengine/lib
COPY misc /usr/src/aperosengine/misc
COPY po /usr/src/aperosengine/po
COPY src /usr/src/aperosengine/src
COPY irr /usr/src/aperosengine/irr
COPY textures /usr/src/aperosengine/textures

WORKDIR /usr/src/aperosengine
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
	adduser -D aperosengine --uid 30000 -h /var/lib/aperosengine && \
	chown -R aperosengine:aperosengine /var/lib/aperosengine

WORKDIR /var/lib/aperosengine

COPY --from=builder /usr/local/share/aperosengine /usr/local/share/aperosengine
COPY --from=builder /usr/local/bin/aperosengineserver /usr/local/bin/aperosengineserver
COPY --from=builder /usr/local/share/doc/aperosengine/aperosengine.conf.example /etc/aperosengine/aperosengine.conf
COPY --from=builder /usr/local/lib/libspatialindex* /usr/local/lib/
COPY --from=builder /usr/local/lib/libluajit* /usr/local/lib/
USER aperosengine:aperosengine

EXPOSE 30000/udp 30000/tcp
VOLUME /var/lib/aperosengine/ /etc/aperosengine/

ENTRYPOINT ["/usr/local/bin/aperosengineserver"]
CMD ["--config", "/etc/aperosengine/aperosengine.conf"]
