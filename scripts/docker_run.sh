#!/usr/bin/env bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

# running in network host mode to access postgres DB on machine.
sudo docker run -v "$SCRIPT_DIR"/..:/app editlite-builder

