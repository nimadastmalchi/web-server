### Build/test container ###
# Define builder stage
FROM a2-team-please-adopt-me-nima:base as builder

# Share work directory
COPY . /usr/src/project
WORKDIR /usr/src/project/build

# Build and test
RUN cmake ..
RUN make
RUN ctest --output-on_failure
RUN make test


### Deploy container ###
# Define deploy stage
FROM ubuntu:jammy as deploy

COPY --from=builder /usr/src/project/build/bin/server .
COPY --from=builder /usr/src/project/src/server_config .
COPY --from=builder /usr/src/project/img img/
COPY --from=builder /usr/src/project/src/chess130 chess130/

EXPOSE 80

# Use ENTRYPOINT to specify the binary name
ENTRYPOINT ["./server"]

# Use CMD to specify arguments to ENTRYPOINT
CMD ["server_config"]
