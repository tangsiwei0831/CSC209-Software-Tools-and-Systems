
all: classifier 

classifier: dectree.c classifier.c
	gcc -g -Wall -std=gnu99 -o classifier dectree.c classifier.c -lm

.PHONY: clean all

clean:	
	rm classifier
