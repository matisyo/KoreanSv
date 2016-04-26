echo Buscando replicas
	for j in `seq 1 168`;
    do
    	

		targets=($(grep -o " $j$" Id-1_14-01-2016.txt))	
    	#echo $targets
    	length=${#targets[@]}
    	
    	if [ "$length" = "1" ]; then 
    		echo hola > basura.txt
    	else
    		echo Perdiste
    	fi

    done
   echo Fin