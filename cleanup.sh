#!/bin/sh

find . -not -path '*/.git/*' -type d -exec chmod 0700 '{}' +
find . -not -path '*/.git/*' -type f -exec chmod 0600 '{}' +
find . -not -path '*/.git/*' -type f -name '*.sh' -exec chmod 0700 '{}' +

find . -not -path '*/.git/*' -type f -exec dos2unix '{}' +