#ifndef AVLDATABASE_H
#define AVLDATABASE_H

#include <stdexcept>
#include <ios>
#include <iostream>
#include <fstream>

/**
 * Struct for Node stored in a binary file
 * 
 * valid -> if the node is valid
 * key -> the key which will be used to compare this node with others
 * data_index -> the index of the data stored in this node
 * balance -> left tree height - right tree height
 * left -> left child index
 * right -> right child index
 */
typedef struct Node {
  int valid;
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
     * AvlDatabase constructor
     * @param data_path path to the data binary file
     * @param tree_path path to the tree binary file
     */
    AvlDatabase(std::string data_path, std::string tree_path) {
      // Create files if it doesn't exist
      data_file.open(data_path, std::ios::app);
      tree_file.open(tree_path, std::ios::app);

      data_file.close();
      tree_file.close();
      
      // Open files for reading / writing
      data_file.open(data_path, std::ios::in | std::ios::out | std::ios::binary | std::ios::ate);
      tree_file.open(tree_path, std::ios::in | std::ios::out | std::ios::binary | std::ios::ate);
    }

    /** 
     * AvlDatabase destructor
     * Removes invalid data and node blocks from the binary files and closes 
     * their streams
     * @todo remove invalid data and node blocks before writing to files
     */
    ~AvlDatabase() {
      // ...
      
      data_file.close();
      tree_file.close();
    }

    /** 
     * Add new information to the tree
     * @param key Key of the information (must be unique)
     * @param info The information that will be inserted
     * @throws invalid_argument If another information has the same key
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
     * Removes info from tree
     */
    void remove(const K &key) {
      // Throw exception if tree is empty
      if (is_empty()) {
        throw std::invalid_argument("No info matches key passed to remove()");
      }
      
      write_root_pos(remove_recursive(key, read_root_pos()));
    }

    /** 
     * Gets info from tree
     */
    T get(const K &key) {
      return get_info_recursive(key, read_root_pos());
    }

    /** 
     * Gets the tree height 
     */
    int get_height() {
      return get_node_height(read_root_pos());
    }

    /**
     * Check if the tree is empty
     * @return true if the tree is empty
     * @return false if the tree is not empty
     */
    bool is_empty() {
      return tree_file.tellg() == 0;
    }

    void print(std::ostream &os) {
      os << "-----------------------------" << std::endl;
      os << "Tree height: " << get_height() << std::endl;
      print_recursive(os, read_root_pos(), 0);
      os << "-----------------------------" << std::endl;
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
      
      // Update node balance
      node.balance = get_node_balance(current_pos);
      update_node(current_pos, node);
      balance_node(current_pos);
    }

    /**
     * Removes info recursively from the tree.
     *
     * Returns new child node index (that the parent node that called this
     * method should point to). 
     *
     * If the node doesn't have any childs, this will always return -1. 
     *
     * If it has childs, it will return the index to the biggest node from the
     * left or the smallest node from the right.
     */
    int remove_recursive(const K &key, int current_pos) {
      if (current_pos == -1) {
          throw std::invalid_argument("Info not on tree");
      }

      Node node = read_node(current_pos);

      // If this node must be removed
      if (node.key == key) {
        if (node.left != -1) { 
          Node biggest_node = read_node(get_biggest_node_pos(node.left));
          
          remove_recursive(biggest_node.key, current_pos);

          if (read_node(node.left).valid == -1) {
            node.left = -1;
          }

          node.key = biggest_node.key;
          node.data_index = biggest_node.data_index;
        } else if (node.right != -1) {
          Node smallest_node = read_node(get_smallest_node_pos(node.right));
          
          remove_recursive(smallest_node.key, current_pos);
          
          if (read_node(node.right).valid == -1) {
            node.right = -1;
          }

          node.key = smallest_node.key;
          node.data_index = smallest_node.data_index;
        } else {
          delete_node(current_pos);
          return -1;
        }
      } else if (key > node.key) {
        node.right = remove_recursive(key, node.right);
      } else if (key < node.key) {
        node.left = remove_recursive(key, node.left);
      }
      
      node.balance = get_node_balance(current_pos);
      update_node(current_pos, node);
      balance_node(current_pos);
      
      return current_pos;
    }

    /**
     * Gets smallest node index starting from node at position passed by parameter
     */ 
    K get_smallest_node_pos(int current_pos) {
      Node node = read_node(current_pos);
      if (node.left != -1) {
        return get_smallest_node_pos(node.left);
      } else {
        return current_pos;
      }
    }

    /**
     * Gets biggest key starting from node at position passed by parameter
     */ 
    K get_biggest_node_pos(int current_pos) {
      Node node = read_node(current_pos);
      if (node.right != -1) {
        return get_biggest_node_pos(node.right);
      } else {
        return current_pos;
      }
    }

    /**
     * Gets info recursively from the tree 
     */
    T get_info_recursive(const K &key, int current_pos) {
      if (current_pos == -1) {
        throw std::invalid_argument("No info matches key passed to get_info()");
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
        tree_file.clear();
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

      return ((int)(tree_file.tellg()) - tree_file_offset) / sizeof(Node) - 1;
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
     * Defines the node at specified position as invalid
     */ 
    void delete_node(int pos) {
      Node invalid_node = Node();
      invalid_node.valid = -1;
      update_node(pos, invalid_node);
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

      if (node.valid == -1) {
         return 0;
      }

      return std::max(get_node_height(node.right), get_node_height(node.left)) + 1;
    }

    /** 
     * Gets balance of node at specified position
     */
    int get_node_balance(int pos) {
      if (pos == -1) {
        return 0;
      }

      Node node = read_node(pos);

      return (get_node_height(node.right) - get_node_height(node.left));
    }

    /** 
     * Check if the node at specified position must be balanced and returns if
     * it was
     */
    bool balance_node(int pos) {
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
      } else {
        return false;
      }
      return true;
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

      // Adjust old and new root balances dynamically
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

      // Adjust old and new root balances dynamically
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
     * Applies double right rotation to node at given position
     */
    void rotate_double_right(int pos) {
      Node node = read_node(pos);
      rotate_left(node.left);
      rotate_right(pos);
    }

    /**
     * Prints tree recursively
     */
    void print_recursive(std::ostream& os, int pos, int space) {
      if (pos == -1) {
        return;
      }

      Node node = read_node(pos);

      if (node.valid == -1) {
        os << "INVALID NODE" << std::endl;
        return;
      }
      
      space += 5;
      print_recursive(os, node.right, space);

      os << std::endl;

      for (int i = 5 ; i < space;  i++) {
        os << " ";
      }

      os << node.key  << " : " << node.balance << std::endl;
    
      print_recursive(os, node.left, space);
    }
};

#endif