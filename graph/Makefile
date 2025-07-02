libpagerank.so: pagerank.c pagerank.h
	$(CC) -fPIC -shared pagerank.c -o libpagerank.so

test_pagerank: test_pagerank.c libpagerank.so
	$(CC) test_pagerank.c -L. -lpagerank -o test_pagerank

test_pagerank_all: test_pagerank_all.c libpagerank.so libeval.so
	$(CC) test_pagerank_all.c -L. -lpagerank -leval -o test_pagerank_all -lm

libeval.so: eval.c eval.h
	$(CC) -fPIC -shared eval.c -o libeval.so -lm

clean:
	rm -f libpagerank.so libeval.so test_pagerank test_pagerank_all
