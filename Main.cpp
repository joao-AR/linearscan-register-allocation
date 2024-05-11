#include <iostream> 
#include <sstream>
#include <vector>
#include <list>
#include <map>

using namespace std;

struct Life_range_Node {
    int start;
    int end;
    int reg = -1;
    int interation_added;
    bool spill;
}; 

struct Life_range {
    map<int, Life_range_Node> nodes;
};

// Retorna um vetor de todos os nós que estão expirados
vector<int> get_expired_nodes( vector<int> Life_range_node, list<int> actives, map<int, Life_range_Node> nodes) {
    vector<int> expired;

    for(auto &active_node : actives){
        // [0] Inicio [1] Fim
        if(Life_range_node[0] >= nodes[active_node].end){
            expired.push_back(active_node);
        }
    }
    return expired;
}


int get_spill_node(int id_node, vector<int> Life_range_node, list<int> actives, map<int, Life_range_Node> nodes){
    // Primeiramente coloca o nó que queremos adicionar para spill
    int spill_node = id_node;
    int bigest_end = Life_range_node[1];
    int smallest_range = Life_range_node[1] - Life_range_node[0];
    
    //Usado para salvar em qual posição do ativos o registrador vitual foi salvo
    int posi_added;
    int actives_posi = 0;
    
    for(auto &active_node : actives){
        //Regra 1 (Selecionar o nó com o final mais distante)
        if(nodes[active_node].end > bigest_end){
            bigest_end = nodes[active_node].end;
            smallest_range = nodes[active_node].end - nodes[active_node].start;
            posi_added = actives_posi;
            spill_node = active_node;

        }else if(nodes[active_node].end == bigest_end){
            
            //Regra 2 desempate (Selecionar o com o intervalo de tempo de vida menor )
            if( (nodes[active_node].end - nodes[active_node].start) < smallest_range){
                bigest_end = nodes[active_node].end;
                smallest_range = nodes[active_node].end - nodes[active_node].start;
                posi_added = actives_posi;
                spill_node = active_node;

            }else if( (nodes[active_node].end - nodes[active_node].start) == smallest_range){

                //Regra 3 desempate (Selecionar nó mais recentemente adicionado)
                if(actives_posi > posi_added){
                    bigest_end = nodes[active_node].end;
                    smallest_range = nodes[active_node].end - nodes[active_node].start;
                    posi_added = actives_posi;
                    spill_node = active_node;
                }
            }
        }

        actives_posi++;
    }

    return spill_node;
}

int main() {
    string line;
    string token;

    int k;

    Life_range Lr;
    map<int, vector<int>> reg_map;
    list<int> actives; // lista de registradores ativos atualmente
    map<int, vector<int>> pair_k_spill;// Armazena o k(registradores ) e as iterações que resultaram em spill
    
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
                        first_node = true;
                    }else{
                        token_int = stoi(token);
                        reg_map[node].push_back(token_int);
                    }
                } 
            }
        }
    } 

    int qtd_reg;

    for(qtd_reg = k; qtd_reg >= 2; qtd_reg--){
        pair_k_spill[qtd_reg];
        cout << "K = " << qtd_reg << endl << endl;
        vector<pair<int, int>> registers; // Vetor de registradores que podem ou não podem ser utilizados
        
        
        for(int i = 0; i < qtd_reg; i++){ registers.push_back(make_pair(i,1)); }// Inicializar todos os registradores como vazios

        int iteration = 0; 
        for(auto &rm : reg_map){
            vector<int> &node_life_time = rm.second;
            vector<int> expired_nodes = get_expired_nodes(node_life_time, actives, Lr.nodes);
            
            if( expired_nodes.size() != 0){// Um ou mais nós expirados
                
                // Para cada nó expirado faça
                for(auto &expired_node : expired_nodes){
                    int reg_expired = Lr.nodes[expired_node].reg;
                    
                    for(int i = 0; i < registers.size(); i++){// Achando o registrador que está sendo utilizado pelo Nó expirado e liberando ele
                        if(registers[i].first == reg_expired){
                            registers[i].second = 1;
                            break;
                        }
                    }
                    actives.remove(expired_node); // Removendo o Nó expirado da lista de ativos
                }
                
            }else if(expired_nodes.size() == 0 && actives.size() == qtd_reg){ //Se não tiver nenhum expirado e o ativos está cheio fazer SPILL
                int spill_node = get_spill_node(rm.first, node_life_time, actives, Lr.nodes);
                int reg_spill = Lr.nodes[spill_node].reg;
                Lr.nodes[spill_node].spill = true;
                
                pair_k_spill[qtd_reg].push_back(iteration);

                if(spill_node != rm.first){ //Se o nó selecionado para spill não for o que queriamos adicionar
                    for(int i = 0; i < registers.size(); i++){ // Achando o registrador que está sendo utilizado pelo Nó mandado para spill e liberando ele
                        if(registers[i].first == reg_spill){
                            registers[i].second = 1;
                            break;
                        }
                    }
                    
                    //Só faz sentido remover do actives se o nó que for para spill não seja o que ia ser adicionado, pois, o nó q seria adicionado não estava no spill
                    actives.remove(spill_node);
                }
                
            }else{
                // cout << "Nenhum nó expirado ou mandado para spill" << endl;
            }

            for(auto &reg : registers){ //Alocar registrador (com o menor numero) para o registrador virtual
                if(reg.second == 1){ // Registrador Livre

                    Lr.nodes[rm.first].start = rm.second[0];
                    Lr.nodes[rm.first].end = rm.second[1];
                    Lr.nodes[rm.first].reg = reg.first;
                    reg.second = 0; // marcando como ultizado

                    // Adicionar o registrador em ativos
                    actives.push_back(rm.first);
                    break;
                }
            }

            iteration++;
        } 
        
        // Imprimir resultado de cada registrador virtual após o termino do linear scan
        for(auto &lr : Lr.nodes){
            cout << "vr" << lr.first << ": "; 
            if(lr.second.spill == true){
                cout << "SPILL" << endl;
            }else{
                cout << lr.second.reg << endl;;
            }
        }

        // Limpando tudo para a proxima iteração 
        Lr.nodes.clear();
        registers.clear();
        actives.clear();
        cout << "----------------------------------------" << endl;
    }
    
    cout << "----------------------------------------" << endl;
    
    // Itera sobre os elementos em ordem decrescente
    for(auto it = pair_k_spill.rbegin(); it != pair_k_spill.rend(); ++it){
        cout << "K = " << it->first << ": ";
        
        if(it->second.empty()){
            cout << "Successful Allocation";
        } else {
            cout << "SPILL on interation(s): ";
            for(auto spill_it = it->second.begin(); spill_it != it->second.end(); ++spill_it){
                cout << *spill_it;
                if(next(spill_it) != it->second.end()) {
                    cout << ", ";
                }
            }
        }
        
        if(next(it) != pair_k_spill.rend()) {
            cout << endl;
        }
    }

    return 0;
}