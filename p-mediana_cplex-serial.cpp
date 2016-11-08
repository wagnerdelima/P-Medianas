using namespace std;

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string>
#include <sstream>

// Numero de vertices do grafo.
#define V 20
// Numero de medianas do problema.
#define MEDIANAS 5
// Numero de possiveis solucoes.
#define Q_SOLUCOES 15504

static int matrizSolucoes[Q_SOLUCOES][MEDIANAS];

static int valorMedianaSol[Q_SOLUCOES];

static int matrizGrafo[V][V];

static int matrizDistancias[V][V];

int minDistance(int dist[], bool sptSet[]){

	int min = INT_MAX, min_index;

	for (int v = 0; v < V; v++){

		if (sptSet[v] == false && dist[v] <= min){

			min = dist[v], min_index = v;

		}

	}

	return min_index;

}

int dijkstra(int graph[V][V], int src, int dst){

	int dist[V];

	bool sptSet[V];

	for (int i = 0; i < V; i++){

		dist[i] = INT_MAX, sptSet[i] = false;

	}

	dist[src] = 0;

	for (int count = 0; count < V-1; count++){

		int u = minDistance(dist, sptSet);

		sptSet[u] = true;

		for (int v = 0; v < V; v++){

			if (!sptSet[v] && graph[u][v] && dist[u] != INT_MAX
					&& dist[u]+graph[u][v] < dist[v]){

				dist[v] = dist[u] + graph[u][v];

			}

		}

	}

	return dist[dst];

}

int distanciaNos(int src, int dst){

	return matrizDistancias[src][dst];

}

void pMediana(int graph[V][V], int p){

	int melhorSolucao[p];

	int medianaAtual;

	for(int i = 0; i < p; i++){

		melhorSolucao[i] = i;

	}

	int melhorMediana = 0;

	for(int j = 0; j < p; j++){

		melhorSolucao[j] = matrizSolucoes[0][j];

	}

	for(int i = 0; i < V; i++){

		bool isParteSolucao = false;

		for (int j = 0; j < MEDIANAS; j++){

			if (i == melhorSolucao[j]){

				isParteSolucao = true;

				break;

			}

		}

		if(!isParteSolucao){

			int distSolucoes[p];

			for (int j = 0; j < p; j++){

				distSolucoes[j] = dijkstra(graph, melhorSolucao[j], i);

			}

			int menorValor = distSolucoes[0];

			for(int j = 1; j < p; j++){

				if(distSolucoes[j] < menorValor){

					menorValor = distSolucoes[j];

				}

			}

			melhorMediana = melhorMediana + menorValor;

		}

	}

	for(int i = 1; i < Q_SOLUCOES; i++){

		int solucaoAtual[p];

		for(int j = 0; j < p; j++){

			solucaoAtual[j] = matrizSolucoes[i][j];

		}

		medianaAtual = 0;

		for(int k = 0; k < V; k++){

			bool isParteSolucao = false;

			for(int j = 0; j < MEDIANAS; j++){

				if(k == solucaoAtual[j]){

					isParteSolucao = true;
					break;

				}

			}

			if(!isParteSolucao){

				int distSolucoes[p];

				for (int j = 0; j < p; j++){

					distSolucoes[j] = dijkstra(graph, solucaoAtual[j], k);

				}

				int menorValor = distSolucoes[0];

				for(int j = 1; j < p; j++){

					if(distSolucoes[j] < menorValor){

						menorValor = distSolucoes[j];

					}

				}

				medianaAtual = medianaAtual + menorValor;

			}

		}

		if(medianaAtual < melhorMediana){

			melhorMediana = medianaAtual;

			for (int l = 0; l < p; l++){

				melhorSolucao[l] = matrizSolucoes[i][l];

			}

		}

	}

	cout << "Melhor Solução: [ ";

	for (int l = 0; l < p; l++){

		cout << melhorSolucao[l] << " ";

	}

	cout << "]" << endl;

}

void carregarGrafo(){

	std::stringstream numGrafo;
	numGrafo << V;

	std::string str = "grafos/" + numGrafo.str();

	const char* path = str.c_str();

	FILE* arquivo = fopen(path, "r");

	if (arquivo == NULL){

		fprintf(stderr, "Arquivo do Grafo nao foi aberto!\n");
		exit(1);

	}

	for (int i = 0; i < V; i++){

		for (int j = 0; j < V; j++){

			int a = 0;
			fscanf(arquivo, "%d", &a);
			matrizGrafo[i][j] = a;

		}

	}

}

void carregarSolucoes(){

	if(MEDIANAS == 2){

		int num_matr = 0;

		for (int i = 0; i < (V - 1); i++){

			for (int j = (i + 1); j < (V); j++){

				matrizSolucoes[num_matr][0] = i;
				matrizSolucoes[num_matr][1] = j;

				num_matr++;

			}

		}

	} else if(MEDIANAS == 3){

		int num_matr = 0;

		for (int i = 0; i < (V - 2); i++){

			for (int j = (i + 1); j < (V - 1); j++){

				for (int k = (j + 1); k < (V); k++){

					matrizSolucoes[num_matr][0]  = i;
					matrizSolucoes[num_matr][1]  = j;
					matrizSolucoes[num_matr][2]  = k;

					num_matr++;

				}

			}

		}

	} else if(MEDIANAS == 5){

		int num_matr = 0;

		for (int i = 0; i < (V - 4); i++){

			for (int j = (i + 1); j < (V - 3); j++){

				for (int k = (j + 1); k < (V - 2); k++){

					for (int l = (k + 1); l < (V - 1); l++){

						for (int m = (l + 1); m < V; m++){

							matrizSolucoes[num_matr][0]  = i;
							matrizSolucoes[num_matr][1]  = j;
							matrizSolucoes[num_matr][2]  = k;
							matrizSolucoes[num_matr][3]  = l;
							matrizSolucoes[num_matr][4]  = m;

							num_matr++;

						}

					}

				}

			}

		}

	}else{

		cout << "Esse valor de mediana nao eh usado!" << endl;

	}

}

int main(){

	cout << "Carregando o Problema...\n" << endl;
	carregarGrafo();
	carregarSolucoes();

	for(int i = 0; i < V; i++){

		for(int j = 0; j < V; j++){

			matrizDistancias[i][j] = dijkstra(matrizGrafo, i, j);

		}

	}

	cout << "Resolvendo...\n" << endl;
	pMediana(matrizGrafo, MEDIANAS);

	int melhorS = INT_MAX;
	int indexMelhor = 0;

	cout << "\nFim." << endl;

	return 0;

}

/*
Look out of any window
Any morning, any evening, any day
Maybe the sun is shining
Birds are winging
Or rain is falling from a heavy sky...
*/
