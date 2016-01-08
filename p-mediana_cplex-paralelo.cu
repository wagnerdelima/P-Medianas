
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <cuda.h>

#include <thrust\host_vector.h>
#include <thrust\device_vector.h>
#include <thrust\sort.h>
#include <thrust\extrema.h>

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string>
#include <sstream>
#include <book.h>

using namespace std;

// Numero de vertices do grafo.
#define V 65
// Numero de medianas do problema.
#define MEDIANAS 5
// Numero de possiveis solucoes.
#define Q_SOLUCOES 8259888

#define FLAG 0

static int matrizSolucoes[Q_SOLUCOES * MEDIANAS];

static int valorMedianaSol[Q_SOLUCOES];

static int matrizGrafo[V][V];

static int matrizDistancias[V * V];

__host__ int minDistance(const int dist[], const bool sptSet[]){

	int min = INT_MAX, min_index;

	for (int v = 0; v < V; v++){

		if (sptSet[v] == false && dist[v] <= min){

			min = dist[v], min_index = v;

		}

	}

	return min_index;

}

__host__ int dijkstra(int graph[V][V], int src, int dst){

	int distancia[V];

	bool sptSet[V];

	for (int i = 0; i < V; i++){
    
		distancia[i] = INT_MAX, sptSet[i] = false;
	
	}

	distancia[src] = 0;

	for (int count = 0; count < V-1; count++){

		int u = minDistance(distancia, sptSet);

		sptSet[u] = true;

		for (int v = 0; v < V; v++){

			if (!sptSet[v] && graph[u][v] && distancia[u] != INT_MAX
            		&& distancia[u]+graph[u][v] < distancia[v]){

				distancia[v] = distancia[u] + graph[u][v];

			}

		}

	}

	return distancia[dst];

}

__host__ int distanciaNos(int src, int dst){
	
	return matrizDistancias[src * V + dst];

}

__global__ void checar(int medianas, int i, int matrizSolucoes[Q_SOLUCOES * MEDIANAS], int *solucaoAtual){
		
	int tx = threadIdx.x;
	
	if(tx < medianas){
		
		solucaoAtual[tx] = matrizSolucoes[i * medianas + tx];

	}

}


__host__ void pMediana(int graph[V][V]){

	int *solucaoAtual = (int*)malloc(sizeof(int) * MEDIANAS), 
		*hSolucao, *hMatriz;
	int *atual, *menor, *Solucoes;

	cudaMalloc((void**)&hSolucao, sizeof(int) * MEDIANAS);
	cudaMalloc((void**)&hMatriz, sizeof(int) * Q_SOLUCOES * MEDIANAS);
	cudaMemcpy(hMatriz, matrizSolucoes, sizeof(int) * Q_SOLUCOES * MEDIANAS, cudaMemcpyHostToDevice);

	for(int i = 0; i < Q_SOLUCOES; i++){
		
		checar<<<1, MEDIANAS>>>(MEDIANAS, i, hMatriz, hSolucao);

		cudaMemcpy(solucaoAtual, hSolucao, sizeof(int) * MEDIANAS, cudaMemcpyDeviceToHost);

		int medianaAtual = 0;

		for(int k = 0; k < V; k++){

			bool isParteSolucao = false;

			for(int j = 0; j < MEDIANAS; j++){
				
				if(k == solucaoAtual[j]){

					isParteSolucao = true;
					break;

				}

			}

		
			if(!isParteSolucao){

				int distSolucoes[MEDIANAS];

				for (int j = 0; j < MEDIANAS; j++){

					distSolucoes[j] = distanciaNos(solucaoAtual[j], k);

				}

				int menorValor = distSolucoes[0];

				for(int j = 1; j < MEDIANAS; j++){

					if(distSolucoes[j] < menorValor){

						menorValor = distSolucoes[j];

					}

				}

				medianaAtual = medianaAtual + menorValor;

			}

		}

		valorMedianaSol[i] = medianaAtual;

	}

	free(solucaoAtual);
	cudaFree(hSolucao);
	cudaFree(hMatriz);

}

__host__ void carregarGrafo(){

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

__host__ void carregarSolucoes(){

	if(MEDIANAS == 2){

		int num_matr = 0;

		for (int i = 0; i < (V - 1); i++){

			for (int j = (i + 1); j < (V); j++){

				matrizSolucoes[num_matr * MEDIANAS + 0] = i;
				matrizSolucoes[num_matr * MEDIANAS + 1] = j;

				num_matr++;

			}

		 }

	 } else if(MEDIANAS == 3){

		int num_matr = 0;

		for (int i = 0; i < (V - 2); i++){

			for (int j = (i + 1); j < (V - 1); j++){

				for (int k = (j + 1); k < (V); k++){

					matrizSolucoes[num_matr * MEDIANAS + 0] = i;
					matrizSolucoes[num_matr * MEDIANAS + 1] = j;
					matrizSolucoes[num_matr * MEDIANAS + 2] = k;

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

							matrizSolucoes[num_matr * MEDIANAS + 0] = i;
							matrizSolucoes[num_matr * MEDIANAS + 1] = j;
							matrizSolucoes[num_matr * MEDIANAS + 2] = k;
							matrizSolucoes[num_matr * MEDIANAS + 3] = l;
							matrizSolucoes[num_matr * MEDIANAS + 4] = m;

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
	
	cudaEvent_t start, stop;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);

	cudaEventRecord(start);

	carregarGrafo();
    carregarSolucoes();
	
	
	for(int i = 0; i < V; i++){
		
		for(int j = 0; j < V; j++){
			
			matrizDistancias[i * V + j] = dijkstra(matrizGrafo, i, j);

		}

	}

    cout << "Resolvendo...\n" << endl;

	pMediana(matrizGrafo);
		
	thrust::device_ptr<int> dp = thrust::device_pointer_cast(valorMedianaSol);
	
	int *pos = thrust::min_element(valorMedianaSol, valorMedianaSol + Q_SOLUCOES);

	unsigned int indexMelhor = thrust::distance(dp, (thrust::device_ptr<int>)pos);

	cudaEventRecord(stop);
	cudaEventSynchronize(stop);	
	
	for(int i = 0; i < MEDIANAS; i++){
	
		cout << matrizSolucoes[indexMelhor * MEDIANAS + i] << " ";

	}

	cout  << endl;

	float milliseconds = 0;

	cudaEventElapsedTime(&milliseconds, start, stop);

	cout << "Processing time: " << milliseconds / 1000.0 << " s" << endl;

	cout << endl;
	
	cudaEventDestroy(start);
	cudaEventDestroy(stop);

    return 0;

}
