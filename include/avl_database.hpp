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
  int balance;
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
        int root_pos = read_root_pos();
        int balance_delta = 0;

        add_recursive(key, info, root_pos, &balance_delta);

        if (balance_delta != 0) {
          Node root = read_node(root_pos);
          root.balance += balance_delta;
          update_node(root_pos, root);
        }
        balance_node(root_pos);
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
    void add_recursive(const K &key, const T &info, int current_pos, int* balance_delta) {
      // Get current node
      Node node = read_node(current_pos);

      // Check current node key to find where to insert
      if (key == node.key) {
        throw std::invalid_argument("Info already on tree");
      } else if (key > node.key) {
        // Insert to right
        if (node.right == -1) {
          if (node.left == -1) {
            *balance_delta = 1;
          }
          node.right = write_data_node(key, info);
          update_node(current_pos, node);
        } else {
          add_recursive(key, info, node.right, balance_delta);
          if (balance_delta != 0) {
            node.balance += *balance_delta;
            update_node(current_pos, node);
          }
          balance_node(current_pos);
        }
      } else if (key < node.key) {
        // Insert to left
        if (node.left == -1) {
          if (node.right == -1) {
            *balance_delta = -1;
          }
          node.left = write_data_node(key, info);
          update_node(current_pos, node);
        } else {
          add_recursive(key, info, node.left, balance_delta);
          if (balance_delta != 0) {
            node.balance += *balance_delta;
            update_node(current_pos, node);
          }
          balance_node(current_pos);
        }
      }
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

    /** 
     * Swap nodes of specified positions physically on tree_file
     */
    void swap_nodes(int pos_a, int pos_b) {
      Node node_a;
      if (pos_a == -1) {
        node_a.valid = -1;
      } else {
        node_a = read_node(pos_a);
      }
      
      Node node_b;
      if (pos_b == -1) {
        node_b.valid = -1;
      } else {
        node_b = read_node(pos_b);
      }

      update_node(pos_a, node_b);
      update_node(pos_b, node_a);
    }

    /** 
     * Gets height of node at specified position
     */
    int get_node_height(int pos) {
      if (pos == -1) {
        return 0;
      }

      Node node = read_node(pos);

      return std::max(get_node_height(node.right), get_node_height(node.left)) + 1;
    }

    /** 
     * @deprecated node balance is calculated dynamically
     * Gets balance of node at specified position
     */
    int get_node_balance(int pos) {
      if (pos == -1) {
        return 0;
      }

      Node node = read_node(pos);

      return (get_node_height(node.left) - get_node_height(node.right));
    }

    /** 
     * Balances node at specified position
     */
    void balance_node(int pos) {
      Node node = read_node(pos);
      if (node.balance > 1) {
        if (read_node(node.right).balance < 0) {
          rotate_double_left(pos);
        } else {
          rotate_left(pos);
        }
      } else if (node.balance < -1) {
        if (read_node(node.right).balance > 0) {
          rotate_double_right(pos);
        } else {
          rotate_right(pos);
        }
      }
    }
    
    /** 
     * Applies left rotation to node at given position
     */
    void rotate_left(int pos) {
      Node old_root = read_node(pos);

      swap_nodes(pos, old_root.right);

      int old_root_pos = old_root.right;
      Node new_root = read_node(pos);
      
      int new_root_left_pos = new_root.left;

      new_root.left = old_root_pos;
      old_root.right = new_root_left_pos;

      old_root.balance = old_root.balance - 1 - std::max(new_root.balance, 0);
      new_root.balance = new_root.balance - 1 + std::min(old_root.balance, 0);

      update_node(pos, new_root);
      update_node(old_root_pos, old_root);
    }

    /** 
     * Applies right rotation to node at given position
     */
    void rotate_right(int pos) {
      Node old_root = read_node(pos);

      swap_nodes(pos, old_root.left);

      int old_root_pos = old_root.left;
      Node new_root = read_node(pos);
      
      int new_root_right_pos = new_root.right;

      new_root.right = old_root_pos;
      old_root.left = new_root_right_pos;

      old_root.balance = old_root.balance + 1 - std::min(new_root.balance, 0);
      new_root.balance = new_root.balance + 1 + std::max(old_root.balance, 0);

      update_node(pos, new_root);
      update_node(old_root_pos, old_root);
    }

    /** 
     * Applies double left rotation to node at given position
     */
    void rotate_double_left(int pos) {
      Node node = read_node(pos);
      rotate_right(node.right);
      rotate_left(pos);
    }

    /** 
     * Applies right rotation to node at given position
     */
    void rotate_double_right(int pos) {
      Node node = read_node(pos);
      rotate_left(node.left);
      rotate_right(pos);
    }
};

#endif