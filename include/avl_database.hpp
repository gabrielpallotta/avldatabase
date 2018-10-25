#ifndef AVLDATABASE_H
#define AVLDATABASE_H

#include <stdexcept>
#include <ios>
#include <iostream>
#include <fstream>

typedef struct Node {
  int key;
  int data_index;
  int height;
  int left;
  int right;
} Node;

template <typename K, typename T>
class AvlDatabase
{
  public:
    AvlDatabase(std::string data_path, std::string tree_path) {
      this->root_pos = -1;
      this->data_count = data_file.tellg() / sizeof(T);

      data_file(data_path, std::ios_base::binary | std::ios_base::app);
      tree_file(tree_path, std::ios_base::binary | std::ios_base::app);

      // data_file.write((char*)&data, sizeof(data));
      // T data;
      // data_file.seekp(0);
      // data_file.read((char*)&data, sizeof(data));
      // std::cout << data.name << " " << data.ra;
    }

    void add (const K &key, const T &info) {
      add_data(info);
      add_data_to_tree(key, info, this->root_pos);
    }

    void remove (const K &key) {
      
    }

    bool has_info (const K &key, const T &info) {
      
    }

  private:
    int data_count;
    std::fstream data_file;
    std::fstream tree_file;

    int root_pos;

    void add_data_to_tree(const K &key, const T &info, int current_pos) {
      // Get current node
      Node node = read_node(current_pos);

      // Check current node key to find where to insert
      if (node.key == current_key) {
        throw std::bad_argument("Info already on tree");
      } else if (node.key > current_key) {
        // Insert to right
        if (node.right == -1) {
          int node_index = add_node(key, this.data_count - 1);
          node.right = node_index;
        } else {
          add_data_to_tree(key, info, node.right)
        }
      } else if (node.key < current_key) {
        // Insert to left
        if (node.left == -1) {
          int node_index = add_node(key, this.data_count - 1);
          node.left = node_index;
        } else {
          add_data_to_tree(key, info, node.left)
        }
      }

      // TODO: Balance the tree
      // ...
    }

    void add_data(const T &data) {
      // TODO: seekp end of file
      // data_file.seekp(ios::end);
      data_file.write((char*)&data);
      data_file.write((char*)&data);
      this->data_count++;
    }

    void add_node(const K &key, int data_index) {
      Node node;
      node.key = key;
      node.data_index = data_index;
      node.height = 0;
      node.left = -1;
      node.right = -1;

      // TODO: find where to insert node
      // ...

      tree_file.write((char*)&node)

      // TODO: return where the node was inserted
      // return ...
    }

    Node read_node(streampos pos) {
      Node node;
      tree_file.seekp(pos);
      tree_file.read(&node.key, sizeof(int));
      tree_file.read(&node.data_index, sizeof(int));
      tree_file.read(&node.height, sizeof(int));
      tree_file.read(&node.left, sizeof(int));
      tree_file.read(&node.right, sizeof(int))
      return node;;
    }

};

#endif