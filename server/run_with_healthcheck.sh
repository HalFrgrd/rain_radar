#!/usr/bin/env bash
set -euo pipefail

source api_secrets.py

TMP_OUTPUT=$(mktemp)

# Send start ping
curl -fsS "$HEALTHCHECK_URL/start" > /dev/null

# Ensure temp file is cleaned up
trap 'rm -f "$TMP_OUTPUT"' EXIT

# On error, send last lines of the log to Healthchecks
trap 'curl -fsS --data-urlencode "d=$(tail -n 20 "$TMP_OUTPUT")" "$HEALTHCHECK_URL/fail" > /dev/null' ERR

cd /home/hfrigaard/inky_frame/server

# Run the command, capture all output
/home/hfrigaard/.local/bin/uv run main.py --deploy --clean-up &> "$TMP_OUTPUT"

# If successful, send success ping
curl -fsS "$HEALTHCHECK_URL" > /dev/null
