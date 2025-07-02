libknn.so: knn.c knn.h
	$(CC) -fPIC -shared knn.c -o libknn.so

test_knn: test_knn.c libknn.so
	$(CC) test_knn.c -L. -lknn -o test_knn -lm

clean:
	rm -f libknn.so test_knn
