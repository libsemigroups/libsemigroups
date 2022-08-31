#!/bin/bash
set -e

CONTAINER_NAME="jamesdbmitchell/gap-docker-minimal:version-4.12.1"

# Pull the docker container
docker pull "$CONTAINER_NAME" 

# Start the docker container detached
ID=$(docker run --rm -i -d "$CONTAINER_NAME")

# Copy the libsemigroups directory into the container
docker cp . "$ID:/home/gap/libsemigroups"

# Run the ci/docker-gap.sh script in the running container
docker exec -i "$ID" libsemigroups/ci/run-gap-tests-in-docker-container.sh ; exit

# Attach to the container
docker attach "$ID"
