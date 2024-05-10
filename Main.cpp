#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <string.h>
#include <map>
#include <algorithm>
#include "Life_time.hpp"

using namespace std;

int get_expired_node( vector<int> Life_range_node, list<int> actives, map<int, Life_range_Node> nodes ) {
    int expired = -1;
    
    for(auto &active_node : actives){
        // [0] Inicio [1] Fim
        cout << Life_range_node[0] << " <= " << nodes[active_node].end << endl;
        if(Life_range_node[0] >= nodes[active_node].end){
            expired = active_node;
            return expired;
        }
    }

    return expired;
}

int main() {
    string line;
    string token;

    int k;

    map<int, vector<int>> reg_map;
    list<int> actives; // lista de registradores ativos atualmente
    vector<pair<int, int>> registers; // vetor de registradores que podem ou não podem ser utilizados
    
    // vector<pair<int, int>> registers;// Guarda o par Nó e registro do nó
    Life_range Lr;
    
    // Loop para ler linha a linha da entrada padrão
    while (getline(cin, line)) {
        istringstream iss(line);

        if (line.find("K=") == 0) {//Linha K registradores 
            iss >> token; // Lê o primeiro token da linha
            k = stoi(token.substr(2));
        
        } else {// Linha Grafo de Interferencia
            // Usando um stringstream para dividir a string com base nos espaços
            istringstream iss(line);
            string token;
            bool first_node = false;
            int node;
            int token_int; 
            // Lendo cada palavra separada por espaço e armazenando nos respectivos vetores
            while (iss >> token) {
                if (token.compare("-->") != 0){

                    if(first_node == false){
                        node = stoi(token.substr(2));
                        cout << token.substr(2) << " :";
                        first_node = true;
                    }else{
                        token_int = stoi(token);
                        cout << " - " << token;
                        reg_map[node].push_back(token_int);
                    }
                } 
            }
            cout << endl;
        }
    } 
    
    // inicializar todos os registradores como vazios
    for(int i = 0; i < k; i++){
        registers.push_back(make_pair(i,1));
        // cout << registers[i].first << ", " << registers[i].second << endl;
        
    }

    for(auto &rm : reg_map){
        const vector<int> &node_life_time = rm.second;

        // TODO remover tempos de vidas expirados
        int expired_node = get_expired_node(node_life_time, actives, Lr.nodes);

        if(expired_node != -1){
            cout << "Nó expirado: " << expired_node << endl;
            //TODO remover 
            int reg_expired = Lr.nodes[expired_node].reg;

            // Achando o registrador que está sendo utilizado pelo Nó expirado e liberando ele
            for(int i = 0; i < registers.size(); i++){
                if(registers[i].first == reg_expired){
                    registers[i].second = 1;
                    break;
                }
            }
            cout << "Registrador Usado pelo nó expirado: " << reg_expired << endl; 
            // Removendo o Nó expirado da lista de ativos
            actives.remove(expired_node);
        }else{
            cout << "Nenhum nó expirado" << endl;
        }


        
        // cout << "inicial > " <<  actives.front() << endl;
        // Remover se o final do menor elemento dos actives for menor/igual o inicio do que queremos adicionar
        
        
        cout << "----------------" << endl;
        // TODO verificar se a quantidade de elementos no actives já está no maximo (k)

        /*
            para cada registrador, verificar se está ativo, se estiver adiciona
        */

        //Alocar registrador (com o menor numero) para o tempo de vida
        for(auto &reg : registers){
            cout << reg.first << " , " << reg.second << endl;

            if(reg.second == 1){ // Registrador Livre

                Lr.nodes[rm.first].start = rm.second[0];
                Lr.nodes[rm.first].end = rm.second[1];
                Lr.nodes[rm.first].reg = reg.first;
                reg.second = 0; // marcando como ultizado

                // // Adicionar o registrador em ativos
                actives.push_back(rm.first);
                break;
            }
        }
        
        
        // cout << node.first << ": " ;
        
        // for (auto &life : node_life_time){
        //     cout << " - " << life << " "; 
        // }
        cout << endl;
    }

    for(auto &lr : Lr.nodes){
        cout << "-> " << lr.first;
        cout << ": " << lr.second.start << ", " << lr.second.end << " r = " << lr.second.reg << endl; 
    }
    cout << endl;
    return 0;
}

