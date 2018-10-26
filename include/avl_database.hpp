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
      // ...
      
      data_file.close();
      tree_file.close();
    }

    /** 
     * @todo write stuff here
    */
    void add(const K &key, const T &info) {      
      // If tree is empty, first insertion
      if (is_empty()) {
        write_root_pos(0);
        int data_index = write_data(info);
        write_node(key, data_index);
      } else {
        add_recursive(key, info, read_root_pos());
      }
    }

    /** 
     * @todo write stuff here
    */
    void remove(const K &key) {
      
    }

    /** 
     * @todo write stuff here
    */
    T get_info(const K &key) {
      return get_info_recursive(key, read_root_pos());
    }

    /**
     * Check if the tree is empty
     * @return true if the tree is empty
     * @return false if the tree is not empty
     */
    bool is_empty() {
      return tree_file.tellg() == 0;
    }


  private:
    const int tree_file_offset = sizeof(int);

    std::fstream data_file;
    std::fstream tree_file;

    /**
     * Adds data recursively on the tree
     * First current_pos passed is root_pos, if the tree is empty, this method
     * cannot be called
     */
    void add_recursive(const K &key, const T &info, int current_pos) {
      // Get current node
      Node node = read_node(current_pos);

      // Check current node key to find where to insert
      if (key == node.key) {
        throw std::invalid_argument("Info already on tree");
      } else if (key > node.key) {
        // Insert to right
        if (node.right == -1) {
          node.right = write_data_node(key, info);
        } else {
          add_recursive(key, info, node.right);
        }
      } else if (key < node.key) {
        // Insert to left
        if (node.left == -1) {
          node.left = write_data_node(key, info);
        } else {
          add_recursive(key, info, node.left);
        }
      }

      // TODO: Balance the tree
      // ...
    }

    /**
     * Gets info recursively from the tree 
     */
    T get_info_recursive(const K &key, int current_pos) {
      if (current_pos == -1) {
        throw std::invalid_argument("No info matches key passed to get_info");
      }

      Node node = read_node(current_pos);
      if (key == node.key) {
        return read_data(node.data_index);
      } else if (key > node.key) {
        return get_info_recursive(key, node.right);
      } else if (key < node.key) {
        return get_info_recursive(key, node.left);
      }
    }

    /**
     * Writes data and node, respectively, to data_file and tree_file
     */
    int write_data_node(const K& key, const T& info) {
      int data_index = write_data(info);
      Node* new_node_ptr = new Node(){ 1, key, data_index, 0, -1, -1};
      int node_index = write_node(new_node_ptr);
      return node_index;
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
      if (is_empty()) {
        return -1;
      } else {
        data_file.seekg(0, std::ios_base::beg);
        int pos;
        data_file >> pos;
        return pos;
      }
    }

    /**
     * Writes a data at the end of data_file and returns its index
     */
    int write_data(const T &data) {
      data_file.seekp(0, std::ios_base::end);
      data_file << reinterpret_cast<char*>(&data);
      data_file.flush();
      
      return (data_file.tellg() / sizeof(T)) - 1;
    }

    /**
     * Reads node from data_file at specified position 
     */
    T read_data(int pos) {
      char* data_chars;
      data_file.seekp(pos * sizeof(T), std::ios_base::beg);
      data_file >> data_chars;

      return *reinterpret_cast<T*>(data_chars);
    }

    /**
     * Writes a node at the end of tree_file and returns its index
     */
    int write_node(Node* node_ptr) {
      tree_file.seekp(0, std::ios_base::end);
      tree_file << reinterpret_cast<char*>(node_ptr);
      tree_file.flush();

      return tree_file_offset + (tree_file.tellg() / sizeof(T)) - 1;
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