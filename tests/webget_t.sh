#!/bin/bash

WEB_HASH=`./apps/webget 127.0.0.1 /nph-hasher/xyzzy | tee /dev/stderr | tail -n 1`
CORRECT_HASH="7SmXqWkrLKzVBCEalbSPqBcvs11Pw263K7x4Wv3JckI"

echo HERE1:"$WEB_HASH"

if [ "${WEB_HASH}" != "${CORRECT_HASH}" ]; then
    echo ERROR: webget returned output that did not match the test\'s expectations
    exit 1
fi
exit 0
