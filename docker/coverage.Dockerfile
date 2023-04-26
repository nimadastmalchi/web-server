### Build/test container ###
# Define builder stage
FROM a2-team-please-adopt-me-nima:base as builder

# Share work directory
COPY . /usr/src/project
WORKDIR /usr/src/project/build

# Build and generate test coverage
RUN cmake -DCMAKE_BUILD_TYPE=Coverage ..
RUN make
RUN make coverage