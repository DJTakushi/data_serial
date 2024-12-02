################################################################################
# build stage
################################################################################
FROM alpine:3.20.3 AS build

RUN apk update && apk add --no-cache build-base
RUN apk add cmake=3.29.3-r0
RUN apk add git=2.45.2-r0
RUN apk add boost1.84-dev=1.84.0-r1
RUN apk add openssl-dev=3.3.2-r1
RUN apk add curl-dev=8.11.0-r2
RUN apk add util-linux-dev=2.40.1-r1
RUN apk add mosquitto-dev=2.0.18-r0

# azure iot sdk
WORKDIR /
RUN git clone https://github.com/Azure/azure-iot-sdk-c.git --branch 1.14.0
RUN mkdir -p /azure-iot-sdk-c/cmake
WORKDIR /azure-iot-sdk-c/cmake
RUN git submodule init && git submodule update
RUN cmake -Duse_edge_modules=ON ..
RUN make install

# nlohmann
WORKDIR /
RUN git clone https://github.com/nlohmann/json.git --branch v3.11.3
RUN cp -r /json/include/* /usr/include/

# build
ADD . /data_serial/
RUN rm -rf /data_serial/embedded_common/build/*
RUN rm -rf /data_serial/build/*

WORKDIR /data_serial/build
ARG AZURE_ROUTES=1
RUN cmake -DCMAKE_BUILD_TYPE=Release -DAZURE_ROUTES=1 .. && cmake --build .

################################################################################
# final image
################################################################################

FROM alpine:3.20.3

RUN apk update && apk add --no-cache libstdc++
RUN apk add boost1.84=1.84.0-r1
RUN apk add mosquitto=2.0.18-r0
RUN apk add curl=8.11.0-r2
RUN apk add libuuid=2.40.1-r1

RUN addgroup -S shs && adduser -S shs -G shs
USER shs

COPY --from=build /data_serial/build/app/app ./app
COPY --from=build /data_serial/data_serial_config.json ./data_serial_config.json

# NOTE : entrypoint should be overridden in deployment manifest to AZURE_ROUTES
ENTRYPOINT ["./app", "-cMQTT"]