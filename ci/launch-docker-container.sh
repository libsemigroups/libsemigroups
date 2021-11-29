#!/bin/bash
set -e

if [ "$GH_ACTIONS_ABI" != "32" ] && [ "$GH_ACTIONS_ABI" != "64" ]; then
  echo -e "Error, unrecognised GH_ACTIONS_ABI, expected \"32\", or \"64\", got \"$GH_ACTIONS_ABI\""
  exit 1
elif [ "$GH_ACTIONS_ARCH" != "x86" ] && [ "$GH_ACTIONS_ARCH" != "arm" ]; then
  echo -e "Error, unrecognised GH_ACTIONS_ARCH, expected \"x86\", or \"arm\", got \"$GH_ACTIONS_ARCH\""
  exit 1
elif [ "$GH_ACTIONS_TEST_PROG" == "" ]; then
  echo -e "Error, environment variable GH_ACTIONS_TEST_PROG expected"
  exit 1
elif [ "$GH_ACTIONS_TEST_TAGS" == "" ]; then
  echo -e "Error, environment variable GH_ACTIONS_TEST_TAGS expected"
  exit 1
fi

PREFIX="/home/libsemigroups/ci/"
CONTAINER_NAME="libsemigroups/libsemigroups-docker-$GH_ACTIONS_ARCH-$GH_ACTIONS_ABI-base:main"

# Pull the docker container
docker pull "$CONTAINER_NAME" 

# Start the docker container detached
ID=$(docker run --rm -i -d -e GH_ACTIONS_TEST_PROG -e GH_ACTIONS_TEST_TAGS "$CONTAINER_NAME")

# Copy the libsemigroups directory into the container
docker cp . "$ID:/home/libsemigroups"

# Run the $GH_ACTIONS_SCRIPT in the running container
docker exec -i "$ID" "$PREFIX/run-tests-in-docker-container.sh" ; exit

# Attach to the container
docker attach "$ID"
