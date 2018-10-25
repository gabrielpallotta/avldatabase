#ifndef AVLDATABASE_H
#define AVLDATABASE_H

#include <ios>
#include <iostream>
#include <fstream>

typedef struct Node {
  int key;
  int index;
  int left;
  int right;
} Node;

template <typename K, typename T>
class AvlDatabase
{
  public:
    std::fstream data_file;
    std::fstream tree_file;

    AvlDatabase(std::string data_path, std::string tree_path) {
      std::cout << sizeof(T);

      data_file(data_path, std::ios_base::binary | std::ios_base::app);
      tree_file(tree_path, std::ios_base::binary | std::ios_base::app);
      // data_file.write((char*)&data, sizeof(data));
      // T data;
      // data_file.seekp(0);
      // data_file.read((char*)&data, sizeof(data));
      // std::cout << data.name << " " << data.ra;
    }

    void add (const K &key, const T &info) {
      tree_file.seekp()
      tree_file.seekp()
    }

    void remove (const K &key, const T &info) {
      
    }

    bool has_info (const K &key, const T &info) {
      
    }

  private:
    Node read_node(streampos pos) {
      Node node;
      tree_file.seekp(pos);
      tree_file.read(node.key, sizeof(int));
      tree_file.read(node.index, sizeof(int));
      tree_file.read(node.left, sizeof(int));
      tree_file.read(node.right, sizeof(int))
      return node;;
    }

};

#endif