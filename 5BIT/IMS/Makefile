# Autor: Jakub Sadilek (xsadil07)
# Autor: Adam Sedlacek (xsedla1e)

###########################################################
#		VESKERE EXPERIMENTY JSOU SE SEED PRO MERLINA      #
###########################################################

make:
	@g++ hrabosi.cpp -o hrabosi -O3

exp1:
	# experiment expanze ve 3-5 letech
	./hrabosi -h 40 -s 1575921419

exp2:
	# experiment hubeni pomoci Stutox II - slabe (napr. dest)
	./hrabosi -h 40 -s 1575929758 -t 36,12,25 -e 50,33,33

exp3:
	# experiment hubeni pomoci Stutox II - silne (cca 80 % ucinnost)
	./hrabosi -h 40 -s 1575930663 -t 15,30 -e 80,80

clean:
	@rm -f hrabosi
