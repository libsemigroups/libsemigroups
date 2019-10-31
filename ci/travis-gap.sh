#!/bin/bash
set -e

# DIGRAPHS_VERS=$(perl -lne '/"digraphs",\s*">=(\d+.\d+.\d+)/ && print $1' PackageInfo.g)
# GENSS_VERS=$(perl -lne '/"genss",\s*">=(\d+.\d+.\d+)/ && print $1' PackageInfo.g)
# ORB_VERS=$(perl -lne '/"orb",\s*">=(\d+.\d+.\d+)/ && print $1' PackageInfo.g)
# IO_VERS=$(perl -lne '/"io",\s*">=(\d+.\d+.\d+)/ && print $1' PackageInfo.g)

ci/travis-setup.sh

# Pull the docker container
docker pull gapsystem/gap-container 

# Start the docker container detached
ID=$(docker run --rm -i -t -d gapsystem/gap-container)

# Copy the libsemigroups directory into the container
docker cp . $ID:/home/gap/libsemigroups

# Run the ci/docker-gap.sh script in the running container
docker exec -it $ID libsemigroups/ci/docker-gap.sh ; exit

# Attach to the container
docker attach $ID
