CPLEXFLAGS=-O3 -m64 -O -fPIC -fexceptions -DNDEBUG -DIL_STD -I/opt/cplex/cplex/include -I/opt/cplex/concert/include  -L/opt/cplex/cplex/lib/x86-64_sles10_4.1/static_pic -lilocplex -lcplex -L/opt/cplex/concert/lib/x86-64_sles10_4.1/static_pic -lconcert -lm -pthread -std=c++0x 

CFLAGS=-std=c++11 -static-libstdc++ -static-libgcc -Wall

all:p-mediana_cplex.o
	g++  p-mediana_cplex.o -o p-mediana_cplex.run $(CPLEXFLAGS) $(CFLAGS)


%.o: %.cpp %.hpp
	g++ -c $< -o $@ $(CFLAGS)

dieta.o: p-mediana_cplex.cpp
	g++ -c -o p-mediana_cplex.o p-mediana_cplex.cpp $(CPLEXFLAGS) $(CFLAGS)

clean:
	rm -f *.o

