// graph.h
#ifndef GRAPH_H
#define GRAPH_H

#define MAX_NAME 50
#define MAX_NODES 100
#define MAX_NEIGHBORS 4

typedef struct Edge {
    int destination;          // Índice do nó de destino
    char street[MAX_NAME];    // Nome da rua
    double distance;          // Distância até o destino
} Edge;

typedef struct Node {
    char id;                  // Identificação do ponto (A, B, C, ...)
    double x, y;              // Coordenadas no plano cartesiano
    char street1[MAX_NAME];   // Nome de uma das ruas do cruzamento
    char street2[MAX_NAME];   // Nome da outra rua do cruzamento
    int num_neighbors;        // Número de vizinhos
    Edge neighbors[MAX_NEIGHBORS]; // Lista de vizinhos
} Node;

// Funções para manipular o grafo
int load_graph_from_text(const char* pontos_file, const char* vizinhos_file, Node* nodes, int* num_nodes);
int save_graph_to_binary(const char* binary_file, Node* nodes, int num_nodes);
int load_graph_from_binary(const char* binary_file, Node* nodes, int* num_nodes);

// Funções para encontrar índices de nós
int find_node_index(Node* nodes, int num_nodes, char id);

// Funções para Dijkstra
typedef struct {
    int previous;
    double distance;
} DijkstraNode;

int dijkstra(Node* nodes, int num_nodes, int start, int end, DijkstraNode* dj_nodes);

#endif // GRAPH_H
