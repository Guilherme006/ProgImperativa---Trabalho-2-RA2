// build_graph.c
#include "graph.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    if(argc != 4) {
        printf("Uso: %s pontos.txt vizinhos.txt grafo.bin\n", argv[0]);
        return 1;
    }

    const char* pontos_file = argv[1];
    const char* vizinhos_file = argv[2];
    const char* binary_file = argv[3];

    Node nodes[MAX_NODES];
    int num_nodes = 0;

    if(load_graph_from_text(pontos_file, vizinhos_file, nodes, &num_nodes) != 0) {
        printf("Erro ao carregar o grafo a partir dos arquivos de texto.\n");
        return 1;
    }

    if(save_graph_to_binary(binary_file, nodes, num_nodes) != 0) {
        printf("Erro ao salvar o grafo no arquivo binário.\n");
        return 1;
    }

    printf("Grafo salvo com sucesso no arquivo %s.\n", binary_file);
    return 0;
}
