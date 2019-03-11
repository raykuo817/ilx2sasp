FROM php:7.2-cli
RUN apt-get update
RUN apt-get install -y iproute

RUN mkdir /music
RUN mkdir /ilx2sasp
RUN mkdir /usr/src/ilx2sasp

VOLUME ["/music", "/ilx2sasp"]

WORKDIR /usr/src/ilx2sasp
ADD ilx2sasp.php .
ADD ilx2sasp.ini-sample .
ADD ftp.php .
ADD PlistParser.inc .
ADD startup.sh .
RUN chmod 777 startup.sh

CMD ["./startup.sh"]





