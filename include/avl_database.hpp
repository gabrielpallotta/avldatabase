#ifndef AVLDATABASE_H
#define AVLDATABASE_H

#include <stdexcept>
#include <ios>
#include <iostream>
#include <fstream>

typedef struct Node {
  char valid;
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
      this->data_file = std::fstream(data_path, std::ios_base::binary | std::ios_base::app);
      this->tree_file = std::fstream(tree_path, std::ios_base::binary | std::ios_base::app);

      // data_file.write((char*)&data, sizeof(data));
      // T data;
      // data_file.seekp(0);
      // data_file.read((char*)&data, sizeof(data));
      // std::cout << data.name << " " << data.ra;
    }

    ~AvlDatabase() {
      // TODO: remove invalid data and node blocks
      // ...
      
      this->data_file.close();
      this->tree_file.close();
    }

    void add (const K &key, const T &info) {
      add_recursive(key, info, 0);
    }

    void remove (const K &key) {
      
    }

    bool has_info (const K &key, const T &info) {
      
    }

  private:
    int data_count;

    // streampos last_data_pos;
    // streampos last_tree_pos;

    std::fstream data_file;
    std::fstream tree_file;

    int root_pos;

    /**
     * Adds data recursively on the tree
     */
    void add_recursive(const K &key, const T &info, int current_pos) {
      // If tree is empty
      if (tree_file.tellg() == 0) {
        int data_index = add_data(info);
        int node_index = add_node(key, data_index);
        return;
      }

      // Get current node
      Node node = read_node(current_pos);

      // Check current node key to find where to insert
      if (key == node.key) {
        throw std::invalid_argument("Info already on tree");
      } else if (key > node.key) {
        // Insert to right
        if (node.right == -1) {
          int data_index = add_data(info);
          int node_index = add_node(key, data_index);
          node.right = node_index;
        } else {
          add_recursive(key, info, node.right);
        }
      } else if (key < node.key) {
        // Insert to left
        if (node.left == -1) {
          int data_index = add_data(info);
          int node_index = add_node(key, data_index);
          node.left = node_index;
        } else {
          add_recursive(key, info, node.left);
        }
      }

      // TODO: Balance the tree
      // ...
    }

    /**
     * Adds a data to data_file and returns its index
    **/
    int add_data(const T &data) {
      this->data_file.seekp(0, std::ios_base::end);
      this->data_file << reinterpret_cast<char*>(&data);
      this->data_file.flush();

      return (this->data_file.tellg() / sizeof(T)) - 1;
    }

    /**
     * Adds a node to tree_file and returns its index
     */
    int add_node(const K &key, int data_index) {
      Node* node_ptr = new Node();

      // Set node attributes
      node_ptr->valid = 1;
      node_ptr->key = key;
      node_ptr->data_index = data_index;
      node_ptr->height = 0;
      node_ptr->left = -1;
      node_ptr->right = -1;
      
      this->tree_file.seekp(0, std::ios_base::end);
      this->tree_file << reinterpret_cast<char*>(node_ptr);
      this->tree_file.flush();

      return (this->tree_file.tellg() / sizeof(T)) - 1;
    }

    /**
     * Reads node from tree_file at specified position 
     */
    Node read_node(int pos) {
      char* node_chars;

      this->tree_file.seekg(pos * sizeof(T), std::ios_base::beg);
      this->tree_file >> node_chars;

      return *reinterpret_cast<Node*>(node_chars);
    }

};

#endif