#include <ilcplex/ilocplex.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string>
#include <sstream>

static const int N = 50;
static const int P = 5;

static int matrizGrafo[N][N];

static int matrix_custo[N][N];

int minDistance(int dist[], bool sptSet[]){

	int min = INT_MAX;
	int min_index = 0;

	for (int v = 0; v < N; v++){

		if (sptSet[v] == false && dist[v] <= min){

			min = dist[v], min_index = v;

		}

	}

	return min_index;

}

int dijkstra(int graph[N][N], int src, int dst){

	int dist[N];

	bool sptSet[N];

	for (int i = 0; i < N; i++){

		dist[i] = INT_MAX, sptSet[i] = false;

	}

	dist[src] = 0;

	for (int count = 0; count < N-1; count++){

		int u = minDistance(dist, sptSet);

		sptSet[u] = true;

		for (int v = 0; v < N; v++){

			if (!sptSet[v] && graph[u][v] && dist[u] != INT_MAX
					&& dist[u]+graph[u][v] < dist[v]){

				dist[v] = dist[u] + graph[u][v];

			}

		}

	}

	return dist[dst];

}

ILOSTLBEGIN

int main(int argc, char **argv){

	std::stringstream numGrafo;
	numGrafo << N;

	std::string str = "grafos/" + numGrafo.str();

	const char* path = str.c_str();

	FILE* arquivo = fopen(path, "r");

	if (arquivo == NULL){

		fprintf(stderr, "Arquivo do Grafo nao foi aberto!\n");
		exit(1);

	}

	for (int i = 0; i < N; i++){

		for (int j = 0; j < N; j++){

			int a = 0;
			fscanf(arquivo, "%d", &a);
			matrizGrafo[i][j] = a;

		}

	}

	// Calcular matriz de custo
	for (int i = 0; i < N; i++){

		for (int j = 0; j < N; j++){

			matrix_custo[i][j] = dijkstra(matrizGrafo, i, j);

		}

	}

	// Criando o ambiente
	IloEnv env;

	try {

		// Matriz de alocação das facilidades
		IloArray<IloIntVarArray> matrix_alocacao(env, N);

		for (int i = 0; i < N; i++){

			matrix_alocacao[i] = IloIntVarArray(env, N, 0, 1);

		}

		// Modelo
		IloModel modelo(env, "Problema P-Mediana");

		char var[15];

		for (int i=0; i < N; i++){

			for (int j = 0; j < N ; j++){

				sprintf(var, "m_alocacao[%d,%d]", i, j);
				matrix_alocacao[i][j].setName(var);
				modelo.add(matrix_alocacao[i][j]);

			}

		}

		// Inicializando o objeto cplex
		IloCplex cplex(modelo);

		// Função Objetivo
		IloExpr obj(env);

		for (int i = 0; i < N; i++){

			for (int j = 0; j < N; j++){

				obj = obj + (matrix_custo[i][j] * matrix_alocacao[i][j]);

			}

		}

		modelo.add(IloMinimize(env, obj));

		// Restrições

		// Garantir que cada nó i está alocado a apenas uma facilidade j
		for (int i = 0; i < N; i++){

			IloExpr rest_A(env);

			for (int j = 0; j < N; j++){

				rest_A = rest_A + matrix_alocacao[i][j];

			}

			modelo.add(rest_A == 1);

		}

		// Garantir que a quantidade de medianas é igual a 'p'.
		IloExpr rest_B(env);

		for (int i = 0; i < N; i++){

			rest_B = rest_B + matrix_alocacao[i][i];

		}

		modelo.add(rest_B == P);

		// Garantir que o no j só se conecte ao i se o i for uma mediana.
		IloExpr rest_D(env);

		for (int i = 0; i < N; i++){

			for (int j = 0; j < N; j++){

				if (i == j){

					continue;

				}

				modelo.add(matrix_alocacao[j][i] <= matrix_alocacao[i][i]);

			}

		}

		// Resolver o problema
		if (cplex.solve()){

			cout << "\nMenor Custo = " << cplex.getObjValue() << endl;

		}

		cout << "\nAlocação: \n" << endl;

		for (int i=0; i<N; i++){

			for (int j=0; j<N; j++){

				cout <<  bool(cplex.getValue(matrix_alocacao[i][j])) << " ";

			}

			cout << endl ;

		}


	} catch (const IloException& e) {

		cerr << "Exception caught: " << e << endl;

	} catch (...) {

		cerr << "Unknown exception caught!" << endl;

	}

	env.end();

	cout << "Fim" << endl;

	return 0;

}
