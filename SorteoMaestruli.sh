cont=0
sumador=1
CANT_SORTEOS=168
while IFS="," read Fecha Adjudicador Tipo
do
	#A la variable fecha le remplazo / por - 
	#Otro ejemplo seria Fecha (variable ) // (toda aparicion sino / sola) / (simbolo a rmplazar) /(separador)-(lo que pongo)
	bar=${Fecha////-}
	cont=$[cont+sumador] 
	nombre=$(printf "Id-%d_$bar.txt" "$cont")
	
	lista=$()
	for j in `seq 1 $CANT_SORTEOS`;
    do
    	lista[$j]=$j;
    done

	#inicializo los sorteos
    sorteosRestantes=$CANT_SORTEOS
    #TIRO RANDOM
    rand=$(( ( RANDOM % $sorteosRestantes )  + 1 ))
    
	#Reseteo el file
	echo Numero de orden 1 le corresponde el numero de sorteo ${lista[$rand]} > sorteos/$nombre    
	#Lista poooop como la gran bestia que se hace añicos
	lista[$rand]=${lista[$sorteosRestantes] }
	sorteosRestantes=$[sorteosRestantes-sumador]
	for j in `seq 2 $CANT_SORTEOS`;
    do
    	
    	#TIRO RANDOM
    	rand=$(( ( RANDOM % $sorteosRestantes )  + 1 ))

    	#REEMPPLAZAR SORTEOS POR EL PROCDIR
    	echo Numero de orden $j le corresponde el numero de sorteo ${lista[$rand]} >> sorteos/$nombre    
    	#Lista poooop como la gran bestia que se hace añicos
    	
    	lista[$rand]=${lista[$sorteosRestantes]} 
		sorteosRestantes=$[sorteosRestantes-sumador]
    done
    
    
done < FechasAdj.csv