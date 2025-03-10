OBJS = main.o Accommodation.o  Constructor.o Iterator.o Node.o ProblemInstance.o Reader.o Route.o Solution.o Team.o
CFLAGS = -std=c++1y -fmax-errors=1 -O0 -g -ggdb #-fsanitize=address 

ARGS=Instance_d3_b20_a5_d3_s129.dat 318 0.05 0.01 100000


all: $(OBJS)
	g++ -o MDIIP $(OBJS) $(CFLAGS) #-static-libasan

%.o: %.cpp
	g++ -c $(CFLAGS) $< -o $@

clean:
	rm -f MDIIP *.o

exe:
	./MDIIP ${ARGS}

