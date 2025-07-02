CC = gcc
CFLAGS = -Wall -Wextra -fPIC -I./knn -I./mf -I./graph
LDFLAGS = -lm

SRC = reco.c knn/knn.c mf/mf.c graph/pagerank.c 
OBJ = $(SRC:.c=.o)

TARGET = libreco.so

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -shared -o $(TARGET) $(SRC) $(LDFLAGS)

clean:
	rm -f $(TARGET) *.o *.so log.txt filtered.txt
	rm -f knn/*.o mf/*.o graph/*.o server test_reco 
