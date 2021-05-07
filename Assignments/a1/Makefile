# make is a program that reads a makefile such as this one to make it 
# easier to build programs.
#
# Since we haven't covered makefiles in class yet, this is a very simple
# Makefile.  You don't need to use it, but might find it helpful.
# You are welcome to add to it.  We will use our own Makefile to run tests.

all: classifier

classifier: knn.c classifier.c
	gcc -Wall -g -lm -std=gnu99 -o classifier classifier.c knn.c

test_loadimage: knn.c test_loadimage.c
	gcc -Wall -g -lm -std=gnu99 -o test_loadimage test_loadimage.c knn.c

datasets: datasets.tgz
	tar xvzf datasets.tgz

.PHONY: clean all 

clean:
	rm -rf *.o classifier test_loadimage
