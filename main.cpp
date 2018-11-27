#include <iostream>
#include <stdlib.h>

#ifdef _WIN32
  #define CLEAR_CMD "cls"
#elif _WIN64
  #define CLEAR_CMD "cls"
#elif __linux__
  #define CLEAR_CMD "clear"
#endif

#include "avl_database.hpp"

using namespace std;

int main()
{
  AvlDatabase<int, int> tree("data.bin", "tree.bin");

  while (true) {
    system(CLEAR_CMD);
    cout << "Arvore AVL (guardada em disco):" << endl;
    tree.print(cout);

    cout << "1 - Inserir" << endl;
    cout << "2 - Remover" << endl;
    cout << "3 - Consulta" << endl << endl;
    cout << "Digite uma opcao: ";

    int option;
    cin >> option;

    int value;
    switch (option) {
      case 1:
        cout << "Digite o valor que deseja inserir: ";
        cin >> value;
        cout << endl;
        try {
          tree.add(value, value);
          cout << "Valor inserido com sucesso" ;
        } catch (invalid_argument e) {
          cout << "Valor ja existe na arvore";
        }
        break;
      case 2:
        cout << "Digite o valor que deseja remover: ";
        cin >> value;
        cout << endl;
        try {
          tree.remove(value);
          cout << "Valor removido com sucesso";
        } catch (invalid_argument e) {
          cout << "Valor nao existe na arvore";
        }
        break;
      case 3:
        cout << "Digite o valor que deseja consultar: ";
        cin >> value;
        cout << endl;
        int tree_value = tree.get(value);
        if (tree_value) {
          cout << "Valor encontrado: " << tree_value;
        } else {
          cout << "Valor nao encontrado na arvore";
        }
        break;
    }
    cout << endl << endl;
    cout << "Pressione qualquer tecla para continuar...";
    cin.sync();
    cin.ignore();
  }
}
