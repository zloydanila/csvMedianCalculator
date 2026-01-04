# syntax=docker/dockerfile:1

FROM ubuntu:24.04 AS build
RUN apt-get update && apt-get install -y --no-install-recommends \
  cmake ninja-build g++ git ca-certificates \
  libboost-program-options-dev \
  && rm -rf /var/lib/apt/lists/*
WORKDIR /src
COPY . .
RUN cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
RUN cmake --build build
RUN ctest --test-dir build --output-on-failure

FROM ubuntu:24.04 AS runtime
RUN apt-get update && apt-get install -y --no-install-recommends \
  ca-certificates \
  libboost-program-options1.83.0 \
  && rm -rf /var/lib/apt/lists/*
WORKDIR /app
COPY --from=build /src/build/csv_median_calculator /app/csv_median_calculator
COPY --from=build /src/examples /app/examples
CMD ["/app/csv_median_calculator", "-config", "/app/examples/config.toml"]
