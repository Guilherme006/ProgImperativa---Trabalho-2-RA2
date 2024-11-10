// graph.c
#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Função para encontrar o índice de um nó pelo ID
int find_node_index(Node* nodes, int num_nodes, char id) {
    for(int i = 0; i < num_nodes; i++) {
        if(nodes[i].id == id) return i;
    }
    return -1;
}

// Função para carregar o grafo a partir dos arquivos de texto
int load_graph_from_text(const char* pontos_file, const char* vizinhos_file, Node* nodes, int* num_nodes) {
    FILE *fp_pontos = fopen(pontos_file, "r");
    if(!fp_pontos) {
        printf("Erro ao abrir o arquivo %s\n", pontos_file);
        return -1;
    }

    // Leitura de pontos.txt
    char line[256];
    *num_nodes = 0;
    while(fgets(line, sizeof(line), fp_pontos)) {
        if(*num_nodes >= MAX_NODES) {
            printf("Número máximo de nós atingido.\n");
            fclose(fp_pontos);
            return -1;
        }
        // Remover nova linha
        line[strcspn(line, "\n")] = 0;
        nodes[*num_nodes].id = line[0];
        
        // Coordenadas
        if(!fgets(line, sizeof(line), fp_pontos)) { break; }
        sscanf(line, "%lf %lf", &nodes[*num_nodes].x, &nodes[*num_nodes].y);
        
        // Rua 1
        if(!fgets(line, sizeof(line), fp_pontos)) { break; }
        line[strcspn(line, "\n")] = 0;
        strcpy(nodes[*num_nodes].street1, line);
        
        // Rua 2
        if(!fgets(line, sizeof(line), fp_pontos)) { break; }
        line[strcspn(line, "\n")] = 0;
        strcpy(nodes[*num_nodes].street2, line);
        
        nodes[*num_nodes].num_neighbors = 0;
        (*num_nodes)++;
    }
    fclose(fp_pontos);

    // Leitura de vizinhos.txt
    FILE *fp_vizinhos = fopen(vizinhos_file, "r");
    if(!fp_vizinhos) {
        printf("Erro ao abrir o arquivo %s\n", vizinhos_file);
        return -1;
    }

    while(fgets(line, sizeof(line), fp_vizinhos)) {
        if(*num_nodes >= MAX_NODES) { break; }
        char origem, destino, rua[MAX_NAME];
        // Origem
        origem = line[0];
        // Destino
        if(!fgets(line, sizeof(line), fp_vizinhos)) { break; }
        destino = line[0];
        // Rua
        if(!fgets(line, sizeof(line), fp_vizinhos)) { break; }
        line[strcspn(line, "\n")] = 0;
        strcpy(rua, line);
        
        int index_origem = find_node_index(nodes, *num_nodes, origem);
        int index_destino = find_node_index(nodes, *num_nodes, destino);
        if(index_origem == -1 || index_destino == -1) {
            printf("Origem ou destino inválidos: %c -> %c\n", origem, destino);
            continue;
        }
        
        // Calcular distância
        double dx = nodes[index_origem].x - nodes[index_destino].x;
        double dy = nodes[index_origem].y - nodes[index_destino].y;
        double distance = sqrt(dx*dx + dy*dy);
        
        // Adicionar ao vizinho
        if(nodes[index_origem].num_neighbors >= MAX_NEIGHBORS) {
            printf("Número máximo de vizinhos para o nó %c atingido.\n", origem);
        } else {
            nodes[index_origem].neighbors[nodes[index_origem].num_neighbors].destination = index_destino;
            strcpy(nodes[index_origem].neighbors[nodes[index_origem].num_neighbors].street, rua);
            nodes[index_origem].neighbors[nodes[index_origem].num_neighbors].distance = distance;
            nodes[index_origem].num_neighbors++;
        }
    }
    fclose(fp_vizinhos);
    return 0;
}

// Função para salvar o grafo em um arquivo binário
int save_graph_to_binary(const char* binary_file, Node* nodes, int num_nodes) {
    FILE *fp_bin = fopen(binary_file, "wb");
    if(!fp_bin) {
        printf("Erro ao abrir o arquivo binário %s para escrita.\n", binary_file);
        return -1;
    }
    // Escrever número de nós
    fwrite(&num_nodes, sizeof(int), 1, fp_bin);
    // Escrever cada nó
    for(int i = 0; i < num_nodes; i++) {
        fwrite(&nodes[i], sizeof(Node), 1, fp_bin);
    }
    fclose(fp_bin);
    return 0;
}

// Função para carregar o grafo de um arquivo binário
int load_graph_from_binary(const char* binary_file, Node* nodes, int* num_nodes) {
    FILE *fp_bin = fopen(binary_file, "rb");
    if(!fp_bin) {
        printf("Erro ao abrir o arquivo binário %s para leitura.\n", binary_file);
        return -1;
    }
    // Ler número de nós
    fread(num_nodes, sizeof(int), 1, fp_bin);
    // Ler cada nó
    for(int i = 0; i < *num_nodes; i++) {
        fread(&nodes[i], sizeof(Node), 1, fp_bin);
    }
    fclose(fp_bin);
    return 0;
}

// Implementação do Algoritmo de Dijkstra
int dijkstra(Node* nodes, int num_nodes, int start, int end, DijkstraNode* dj_nodes) {
    int visited[num_nodes];
    for(int i = 0; i < num_nodes; i++) {
        dj_nodes[i].distance = INFINITY;
        dj_nodes[i].previous = -1;
        visited[i] = 0;
    }
    dj_nodes[start].distance = 0.0;

    for(int i = 0; i < num_nodes; i++) {
        // Encontrar o nó não visitado com a menor distância
        double min_distance = INFINITY;
        int u = -1;
        for(int j = 0; j < num_nodes; j++) {
            if(!visited[j] && dj_nodes[j].distance < min_distance) {
                min_distance = dj_nodes[j].distance;
                u = j;
            }
        }
        if(u == -1) break; // Todos os nós restantes são inacessíveis

        if(u == end) break; // Chegou ao destino

        visited[u] = 1;

        // Atualizar as distâncias dos vizinhos
        for(int v = 0; v < nodes[u].num_neighbors; v++) {
            int dest = nodes[u].neighbors[v].destination;
            double alt = dj_nodes[u].distance + nodes[u].neighbors[v].distance;
            if(alt < dj_nodes[dest].distance) {
                dj_nodes[dest].distance = alt;
                dj_nodes[dest].previous = u;
            }
        }
    }

    // Verificar se o destino foi alcançado
    if(dj_nodes[end].distance == INFINITY) {
        return -1; // Não existe caminho
    }
    return 0;
}
