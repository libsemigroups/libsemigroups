#!/bin/bash
set -e

# Pull the docker container
docker pull gapsystem/gap-container 

# Start the docker container detached
ID=$(docker run --rm -i -d gapsystem/gap-container)

# Copy the libsemigroups directory into the container
docker cp . $ID:/home/gap/libsemigroups

# Run the ci/docker-gap.sh script in the running container
docker exec -i $ID libsemigroups/ci/docker-gap.sh ; exit

# Attach to the container
docker attach $ID
