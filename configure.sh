#!/bin/bash -x
# Recommended configuration
if [ ! -e `dirname $0`/configure ]; then
    (cd `dirname $0` && sh bootstrap)
fi
`dirname $0`/configure --sysconfdir=/etc
