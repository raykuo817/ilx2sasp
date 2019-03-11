#!/bin/bash

TAG=1.0.0
NAME=ilx2sasp
USER=raykuo
PARAM=''

case $1 in
    build)
	sudo docker build -t $USER/$NAME .
	;;

    del)
	# remove all stopped containers
	sudo docker rm -f $(sudo docker ps -a -q)

	# remove all untagged images
	sudo docker rmi -f $(sudo docker images | grep "^<none>" | awk '{print $3}')

	# remove all images
	if [ ${2:-''} == 'all' ]; then
	    sudo docker rmi -f $(sudo docker images | awk '{print $3}')
	fi
	;;

    run)
	if [ ${2:-''} == '' ]; then
	    sudo docker run -d $PARAM $USER/$NAME
	fi
	if [ ${2:-''} == 'bash' ]; then
	    sudo docker run -i -t $PARAM $USER/$NAME bash
	fi
	;;

    save)
	sudo docker save -o ~/$NAME.tar $USER/$NAME
	sudo chmod 777 ~/$NAME.tar
	;;

    stop)
	# stop containers
	if [ ${2:-''} == '' ]; then
	    sudo docker stop $(sudo docker ps | awk -v VAR_FN=$USER/$NAME '{if ($2==VAR_FN) {printf $1}}')
	fi

	# stop all containers
	if [ ${2:-''} == 'all' ]; then
	    sudo docker stop $(sudo docker ps -a -q)
	fi
	;;

   tag)
	sudo docker tag $USER/$NAME $USER/$NAME:$TAG
	;;

    *)
#	echo 'd [build | run | run [bash] | save | del [all] | stop [all]]'
	while true; do
	    echo '1) build image'
	    echo '2) run image (background)'
	    echo '3) run image (bash)'
	    echo '4) save image to HOME directory'
	    echo '5) del stopped containers and untagged images'
	    echo '6) del all'
	    echo '7) stop container'
	    echo '8) stop all'
	    echo '9) tag version' $TAG
	    echo 'i) images list'
	    read -p 'Choise (Ctrl-C to Exit): ' input

	    case $input in
		1) ./d.sh build ;;
		2) ./d.sh run ;;
		3) ./d.sh run bash ;;
		4) ./d.sh save ;;
		5) ./d.sh del ;;
		6) ./d.sh del all ;;
		7) ./d.sh stop ;;
		8) ./d.sh stop all ;;
		9) ./d.sh tag ;;
		i) sudo docker images ;;
	    esac
	done
	;;
esac


