#!/bin/bash
set -e

# Image name
IMAGE_NAME="showbox-dev:latest"

# Build the image ensuring freshness
echo "Cleaning up old images and building fresh (Debian 13 Trixie)..."
# Using --pull to ensure we have the latest base image
# Using --no-cache is safer but slower. For iterative dev, we can omit it if we trust the cache.
# We will trust cache for now to speed up tests.
docker build --pull -t "$IMAGE_NAME" -f src/dev.Dockerfile .

# Get current user UID and GID
USER_ID=$(id -u)
GROUP_ID=$(id -g)

# Prepare arguments for GUI support (X11 and DBus forwarding)
DOCKER_ARGS=""
if [ -n "$DISPLAY" ]; then
	echo "Enabling X11 and DBus forwarding..."
	DOCKER_ARGS="$DOCKER_ARGS -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix"

	# DBus support for theme detection
	if [ -n "$DBUS_SESSION_BUS_ADDRESS" ]; then
		# Map the bus path if it is a unix path
		DBUS_PATH=$(echo $DBUS_SESSION_BUS_ADDRESS | sed 's/unix:path=//')
		if [ -S "$DBUS_PATH" ]; then
			DOCKER_ARGS="$DOCKER_ARGS -v $DBUS_PATH:$DBUS_PATH -e DBUS_SESSION_BUS_ADDRESS=$DBUS_SESSION_BUS_ADDRESS"
		fi
	fi
fi

# Determine if we should use TTY
INTERACTIVE_FLAGS="-i"
if [ -t 0 ]; then
	INTERACTIVE_FLAGS="-it"
fi

echo "Starting development container..."
# echo "Mounting $(pwd) to /app"

# Determine command to run
if [ $# -eq 0 ]; then
	set -- bash
fi

# Run the container
docker run --rm $INTERACTIVE_FLAGS \
	-v "$(pwd):/app" \
	-u "$USER_ID:$GROUP_ID" \
	-e HOME=/tmp \
	$DOCKER_ARGS \
	"$IMAGE_NAME" \
	"$@"
