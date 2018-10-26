#ifndef AVLDATABASE_H
#define AVLDATABASE_H

#include <stdexcept>
#include <ios>
#include <iostream>
#include <fstream>

/**
 * Struct for Node stored in a binary file
 */
typedef struct Node {
  char valid;
  int key;
  int data_index;
  int height;
  int left;
  int right;
} Node;

/**
 * Implementation of a database using AvlTree concepts and binary files
 * 
 * @tparam K the type of the key used to compare infos
 * @tparam T the type of the info stored
 */
template <typename K, typename T>
class AvlDatabase
{
  public:
    /** 
     * @todo write stuff here
    */
    AvlDatabase(std::string data_path, std::string tree_path) {
      data_file = std::fstream(data_path, std::ios_base::binary | std::ios_base::app);
      tree_file = std::fstream(tree_path, std::ios_base::binary | std::ios_base::app);

      // data_file.write((char*)&data, sizeof(data));
      // T data;
      // data_file.seekp(0);
      // data_file.read((char*)&data, sizeof(data));
      // std::cout << data.name << " " << data.ra;
    }

    /** 
     * @todo write stuff here
     * @todo remove invalid data and node blocks before writing to files
    */
    ~AvlDatabase() {
      // TODO: remove invalid data and node blocks before writing to files
      // ...
      
      data_file.close();
      tree_file.close();
    }

    void add (const K &key, const T &info) {
      add_recursive(key, info, 0);
    }

    /** 
     * @todo write stuff here
    */
    void remove(const K &key) {
      
    }

    bool has_info (const K &key, const T &info) {
      
    }

  private:
    const int tree_file_offset = sizeof(int);
    int data_count;

    // streampos last_data_pos;
    // streampos last_tree_pos;

    std::fstream data_file;
    std::fstream tree_file;

    int root_pos;

    /**
     * Adds data recursively on the tree
     * First current_pos passed is root_pos, if the tree is empty, this method
     * cannot be called
     */
    void add_recursive(const K &key, const T &info, int current_pos) {
      // If tree is empty, first insertion
      if (tree_file.tellg() == 0) {
        write_root_pos(0);
        int data_index = write_data(info);
        write_node(key, data_index);
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
          int data_index = write_data(info);
          int node_index = write_node(key, data_index);
          node.right = node_index;
        } else {
          add_recursive(key, info, node.right);
        }
      } else if (key < node.key) {
        // Insert to left
        if (node.left == -1) {
          int data_index = write_data(info);
          int node_index = write_node(key, data_index);
          node.left = node_index;
        } else {
          add_recursive(key, info, node.left);
        }
      }

      // TODO: Balance the tree
      // ...
    }

    
    /** 
     * Writes root position at the start of data_file
    */
    void write_root_pos(int pos) {
      data_file.seekp(0, std::ios_base::beg);
      data_file << pos;
      data_file.flush();
    }

    /**
     * Reads root position from the start of data_file and returns it
    */
    int read_root_pos() {
      this->data_file.seekg(0, std::ios_base::beg);
      int pos;
      this->data_file >> pos;
      return pos;
    }

    /**
     * Writes a data at the end of data_file and returns its index
    **/
    int write_data(const T &data) {
      data_file.seekp(0, std::ios_base::end);
      data_file << reinterpret_cast<char*>(&data);
      data_file.flush();

      return (this->data_file.tellg() / sizeof(T)) - 1;
    }

    /**
     * Writes a node at the end of tree_file and returns its index
     */
    int write_node(const K &key, int data_index) {
      Node* node_ptr = new Node();

      // Set node default attributes
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

      tree_file.seekg(tree_file_offset + pos * sizeof(T), std::ios_base::beg);
      tree_file >> node_chars;

      return *reinterpret_cast<Node*>(node_chars);
    }

};

#endif