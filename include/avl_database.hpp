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
      // Create files if it doesn't exist
      data_file = std::fstream(data_path, std::ios::app);
      tree_file = std::fstream(tree_path, std::ios::app);

      data_file.close();
      tree_file.close();
      
      // Open files for reading / writing
      data_file = std::fstream(data_path, std::ios::in | std::ios::out | std::ios::binary | std::ios::ate);
      tree_file = std::fstream(tree_path, std::ios::in | std::ios::out | std::ios::binary | std::ios::ate);

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
        write_data_node(key, info);
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
          update_node(current_pos, node);
        } else {
          add_recursive(key, info, node.right);
        }
      } else if (key < node.key) {
        // Insert to left
        if (node.left == -1) {
          node.left = write_data_node(key, info);
          update_node(current_pos, node);
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
      Node new_node = { 1, key, data_index, 0, -1, -1};
      int node_index = write_node(new_node);
      return node_index;
    }
    
    /** 
     * Writes root position at the start of tree_file
    */
    void write_root_pos(int pos) {
      tree_file.clear();
      tree_file.seekp(0, std::ios::beg);
      tree_file.write(reinterpret_cast<char*>(&pos), sizeof(int));
      tree_file.flush();
    }

    /**
     * Reads root position from the start of tree_file and returns it
    */
    int read_root_pos() {
      if (is_empty()) {
        return -1;
      } else {
        tree_file.seekg(0, std::ios::beg);
        int pos;
        tree_file >> pos;
        return pos;
      }
    }

    /**
     * Writes a data at the end of data_file and returns its index
     */
    int write_data(const T &data) {
      T* data_ptr = new T(data);
      
      tree_file.clear();
      data_file.seekp(0, std::ios::end);
      data_file.write(reinterpret_cast<char*>(data_ptr), sizeof(T));
      data_file.flush();
      
      return (data_file.tellg() / sizeof(T)) - 1;
    }

    /**
     * Reads node from data_file at specified position 
     */
    T read_data(int pos) {
      T data;

      tree_file.clear();
      data_file.seekp(pos * sizeof(T), std::ios::beg);
      data_file.read(reinterpret_cast<char*>(&data), sizeof(T));

      return data;
    }

    /**
     * Writes a node at the end of tree_file and returns its index
     */
    int write_node(Node node) {
      Node* node_ptr = new Node(node);
      
      tree_file.clear();
      tree_file.seekp(0, std::ios::end);
      tree_file.write(reinterpret_cast<char*>(node_ptr), sizeof(Node));
      tree_file.flush();

      return (tree_file.tellg() - tree_file_offset) / sizeof(Node) - 1;
    }

    /**
     * Reads node from tree_file at specified position 
     */
    Node read_node(int pos) {
      Node node;

      tree_file.clear();
      tree_file.seekg(tree_file_offset + pos * sizeof(Node), std::ios::beg);
      tree_file.read(reinterpret_cast<char*>(&node), sizeof(Node));
      
      return node;
    }

    /**
     * Overrides node at specified position with node passed as parameter
     */
    void update_node(int pos, Node node) {
      Node* node_ptr = new Node(node);

      tree_file.clear();
      tree_file.seekp(tree_file_offset + pos * sizeof(Node), std::ios::beg);
      tree_file.write(reinterpret_cast<char*>(node_ptr), sizeof(Node));
      tree_file.flush();
    }

};

#endif