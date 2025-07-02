CC=gcc
CFLAGS=-fPIC -Wall -Wextra -I./knn -I./mf -I./graph
LDFLAGS=-shared -lm

SOURCES=reco.c knn/knn.c mf/mf.c graph/pagerank.c
OUTPUT=libreco.so

all: $(OUTPUT)

$(OUTPUT): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(OUTPUT) $(LDFLAGS)

clean:
	rm -f libmf.so test_mf
