// find_path.c
#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <locale.h> // Adicionado para suportar localização

#define MAX_PATH 100

// Função para calcular o produto vetorial de dois vetores (V1 x V2)
double cross_product(double v1x, double v1y, double v2x, double v2y) {
    return (v1x * v2y) - (v1y * v2x);
}

// Função para determinar a direção da virada
// Retorna 1 para esquerda, -1 para direita, 0 para seguir em frente
int determine_turn_direction(Node* nodes, int previous, int current, int next) {
    // Vetor do atual para próximo (BC)
    double bcx = nodes[next].x - nodes[current].x;
    double bcy = nodes[next].y - nodes[current].y;
    // Vetor do atual para anterior (BA)
    double bax = nodes[previous].x - nodes[current].x;
    double bay = nodes[previous].y - nodes[current].y;
    
    // Calcular produto vetorial: BC x BA
    double cross = cross_product(bcx, bcy, bax, bay);
    
    if (cross > 1e-6) {
        return 1; // Esquerda
    } else if (cross < -1e-6) {
        return -1; // Direita
    } else {
        return 0; // Seguir em frente
    }
}

// Função para obter o nome da rua no cruzamento, excluindo a rua atual
const char* get_cross_street(Node* node, const char* current_street) {
    if (strcmp(node->street1, current_street) != 0) {
        return node->street1;
    } else {
        return node->street2;
    }
}

int main(int argc, char* argv[]) {
    // Configurar a localização para suportar caracteres especiais
    setlocale(LC_ALL, "");

    if(argc != 2) {
        printf("Uso: %s grafo.bin\n", argv[0]);
        return 1;
    }

    const char* binary_file = argv[1];
    Node nodes[MAX_NODES];
    int num_nodes = 0;

    if(load_graph_from_binary(binary_file, nodes, &num_nodes) != 0) {
        printf("Erro ao carregar o grafo do arquivo binário.\n");
        return 1;
    }

    // Exibir todos os pontos disponíveis
    printf("Pontos disponíveis:\n");
    for(int i = 0; i < num_nodes; i++) {
        printf("%c: %s com %s\n", nodes[i].id, nodes[i].street1, nodes[i].street2);
    }

    // Solicitar origem e destino
    char origem_id, destino_id;
    printf("Digite o ID do ponto de origem: ");
    scanf(" %c", &origem_id);
    printf("Digite o ID do ponto de destino: ");
    scanf(" %c", &destino_id);

    int origem = find_node_index(nodes, num_nodes, origem_id);
    int destino = find_node_index(nodes, num_nodes, destino_id);

    if(origem == -1 || destino == -1) {
        printf("Origem ou destino inválido.\n");
        return 1;
    }

    // Executar Dijkstra
    DijkstraNode dj_nodes[MAX_NODES];
    if(dijkstra(nodes, num_nodes, origem, destino, dj_nodes) != 0) {
        printf("Não existe um caminho do ponto %c até o ponto %c.\n", origem_id, destino_id);
        return 0;
    }

    // Reconstruir o caminho
    int path[MAX_PATH];
    int path_length = 0;
    int current = destino;
    while(current != -1) {
        path[path_length++] = current;
        current = dj_nodes[current].previous;
    }

    // Inverter o caminho
    for(int i = 0; i < path_length / 2; i++) {
        int temp = path[i];
        path[i] = path[path_length - i -1];
        path[path_length - i -1] = temp;
    }

    // Emitir o relatório
    printf("Para realizar o percurso entre o ponto %c (%s com %s) e o ponto %c (%s com %s), faça os seguintes movimentos:\n",
           nodes[origem].id, nodes[origem].street1, nodes[origem].street2,
           nodes[destino].id, nodes[destino].street1, nodes[destino].street2);

    int step = 1;
    for(int i = 0; i < path_length -1; i++) {
        int from = path[i];
        int to = path[i+1];
        // Encontrar a rua que conecta from a to
        char street[MAX_NAME] = "";
        for(int j = 0; j < nodes[from].num_neighbors; j++) {
            if(nodes[from].neighbors[j].destination == to) {
                strcpy(street, nodes[from].neighbors[j].street);
                break;
            }
        }

        // Obter a rua de cruzamento no próximo nó
        const char* cross_street = get_cross_street(&nodes[to], street);

        // Instrução de movimento
        printf("(%d) Siga em frente pela %s até o cruzamento com a %s.\n", step++, street, cross_street);

        // Se não for o último movimento, adicionar a instrução de virada
        if(i < path_length -2) {
            int next = path[i+2];
            int turn = determine_turn_direction(nodes, from, to, next);
            printf("(%d) No cruzamento da %s com a %s, vire à %s.\n",
                   step++, street, cross_street,
                   (turn == 1) ? "esquerda" : (turn == -1) ? "direita" : "frente");
        }
    }

    return 0;
}
