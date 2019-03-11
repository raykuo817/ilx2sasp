#!/bin/bash

if [ ! -f /ilx2sasp/ilx2sasp.ini ]; then
    cp /usr/src/ilx2sasp/ilx2sasp.ini-sample /ilx2sasp/ilx2sasp.ini
    echo "Please set up the /ilx2sasp/ilx2sasp.ini first!"
else
    rm -f sync.dat
    rm -rf playlists
    php ./ilx2sasp.php
fi



