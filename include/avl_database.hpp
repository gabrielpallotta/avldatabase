#ifndef AVLDATABASE_H
#define AVLDATABASE_H

#include <stdexcept>
#include <ios>
#include <iostream>
#include <fstream>

#include "binary_storage.hpp"

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
  int key;
  int data_index;
  int balance;
  int left;
  int right;
} Node;

/**
 * Implementation of a database using AvlTree concepts and binary files
 * 
 * @tparam K The type of the key used to compare infos
 * @tparam T The type of the info stored
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
      data_storage = BinaryStorage<T>(data_path, 0);
      node_storage = BinaryStorage<Node>(tree_path, 1);
      if (tree_is_empty()) {
        write_root_pos(-1);
      }
    }

    /** 
     * AvlDatabase destructor
     * Removes invalid data and node blocks from the binary files and closes 
     * their streams
     * @todo remove invalid data and node blocks before writing to files
     */
    ~AvlDatabase() {

    }

    /** 
     * Add new information to the tree
     * @param key Key of the information (must be unique)
     * @param info The information that will be inserted
     * @throws invalid_argument If another information has the same key
     */
    void add(const K &key, const T &info) {      
      // If tree is empty, first insertion
      if (tree_is_empty()) {
        write_root_pos(write_data_node(key, info));
      } else {
        add_recursive(key, info, read_root_pos());
      }
    }

    /** 
     * Removes info from tree
     * @param key Key of the information
     * @throws invalid_argument If information with that key doesn't exist
     */
    void remove(const K &key) {
      // Throw exception if tree is empty
      if (tree_is_empty()) {
        throw std::invalid_argument("No info matches key passed to remove()");
      }
      
      int new_root_pos = remove_recursive(key, read_root_pos());
      write_root_pos(new_root_pos);
    }

    /** 
     * Gets info from tree
     * @param key Key of the information
     * @throws invalid_argument If information with that key doesn't exist
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
     * Checks if the tree is empty
     * @return true If the tree is empty
     * @return false If the tree is not empty
     */
    bool tree_is_empty() {
      return node_storage.is_empty() || read_root_pos() == -1;
    }

    /**
     * Prints the tree
     * @param os The output stream to print the tree
     */
    void print(std::ostream &os) {
      os << "-----------------------------" << std::endl;
      os << "Tree height: " << get_height() << std::endl;
      print_recursive(os, read_root_pos(), 0);
      os << "-----------------------------" << std::endl;
    }

  private:
    BinaryStorage<T> data_storage;
    BinaryStorage<Node> node_storage;

    /**
     * Adds data recursively on the tree
     *
     * First current_pos passed is root_pos, if the tree is empty, this method
     * cannot be called
     */
    void add_recursive(const K &key, const T &info, int current_pos) {
      // Get current node
      FlaggedBlock<Node> block = node_storage.read(current_pos);
      Node node = block.data;

      // Check current node key to find where
      // to insert
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
     * Removes info recursively from the tree
     *
     * Returns new child node index (that the parent node that called this
     * method should point to)
     *
     * If the node doesn't have any childs, this will always return -1
     *
     * If it has childs, it will return the index to the biggest node from the
     * left or the smallest node from the right
     */
    int remove_recursive(const K &key, int current_pos) {
      if (current_pos == -1) {
        throw std::invalid_argument("Info not on tree");
      }

      FlaggedBlock<Node> block = node_storage.read(current_pos);
      Node node = block.data;

      // If this node must be removed
      if (node.key == key) {
        if (node.left != -1) {
          FlaggedBlock<Node> biggest_block = node_storage.read(get_biggest_node_pos(node.left));
          Node biggest_node = biggest_block.data;

          remove_recursive(biggest_node.key, node.left);

          FlaggedBlock<Node> left_block = node_storage.read(node.left);
          if (!left_block.is_valid()) {
            node.left = -1;
          }

          node.key = biggest_node.key;
          node.data_index = biggest_node.data_index;
        } else if (node.right != -1) {
          FlaggedBlock<Node> smallest_block = node_storage.read(get_smallest_node_pos(node.right));
          Node smallest_node = smallest_block.data;
          
          remove_recursive(smallest_node.key, node.right);

          FlaggedBlock<Node> right_block = node_storage.read(node.right);
          if (!right_block.is_valid()) {
            node.right = -1;
          }

          node.key = smallest_node.key;
          node.data_index = smallest_node.data_index;
        } else {
          node_storage.remove(current_pos);
          data_storage.remove(node.data_index);
          return -1;
        }
      } else if (key > node.key) {
        node.right = remove_recursive(key, node.right);
      } else if (key < node.key) {
        node.left = remove_recursive(key, node.left);
      }
      
      update_node(current_pos, node);
      node.balance = get_node_balance(current_pos);
      update_node(current_pos, node);
      balance_node(current_pos);
      
      return current_pos;
    }

    /**
     * Gets smallest node index starting from node at position passed by parameter
     */ 
    K get_smallest_node_pos(int current_pos) {
      Node node = node_storage.read(current_pos).data;
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
      Node node = node_storage.read(current_pos).data;
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

      FlaggedBlock<Node> block = node_storage.read(current_pos);
      Node node = block.data;

      if (key == node.key) {
        return data_storage.read(node.data_index).data;
      } else if (key > node.key) {
        return get_info_recursive(key, node.right);
      } else if (key < node.key) {
        return get_info_recursive(key, node.left);
      }
    }

    /** 
     * Gets height of node at specified position
     */
    int get_node_height(int pos) {
      if (pos == -1) {
        return 0;
      }

      FlaggedBlock<Node> block = node_storage.read(pos);
      Node node = block.data;

      if (!block.is_valid()) {
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

      Node node = node_storage.read(pos).data;

      return (get_node_height(node.right) - get_node_height(node.left));
    }

    /** 
     * Balance node at specified position
     */
    void balance_node(int pos) {
      Node node = node_storage.read(pos).data;
      if (node.balance > 1) {
        if (node_storage.read(node.right).data.balance < 0) {
          rotate_double_left(pos);
        } else {
          rotate_left(pos);
        }
      } else if (node.balance < -1) {
        if (node_storage.read(node.left).data.balance > 0) {
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
      Node old_root = node_storage.read(pos).data;
      node_storage.swap(pos, old_root.right);
      Node new_root = node_storage.read(pos).data;

      int old_root_pos = old_root.right;
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
      Node old_root = node_storage.read(pos).data;
      node_storage.swap(pos, old_root.left);
      Node new_root = node_storage.read(pos).data;
      
      int old_root_pos = old_root.left;
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
      Node node = node_storage.read(pos).data;
      rotate_right(node.right);
      rotate_left(pos);
    }

    /** 
     * Applies double right rotation to node at given position
     */
    void rotate_double_right(int pos) {
      Node node = node_storage.read(pos).data;
      rotate_left(node.left);
      rotate_right(pos);
    }

    
    /**
     * Writes data and node, respectively, to data_storage and node_storage
     */
    int write_data_node(const K& key, const T& info) {
      int data_index = data_storage.write(FlaggedBlock<T>(1, info));
      Node new_node = { key, data_index, 0, -1, -1 };
      int node_index = node_storage.write(FlaggedBlock<Node>(1, new_node));
      return node_index;
    }
    
    /** 
     * Writes root position at the start of tree_file
    */
    void write_root_pos(int pos) {
      node_storage.write_flag(0, pos);
    }

    /**
     * Reads root position from the start of tree_file and returns it
    */
    int read_root_pos() {
      return node_storage.read_flag(0);
    }

    /**
     * Updates node on node_storage (with valid flag equal to 1) on position
     * passed as parameter
     *
     * This is an auxilar function, since this code is used a lot in this class
     */ 
    void update_node(int pos, Node node) {
      node_storage.write(FlaggedBlock<Node>(1, node), pos);
    }

    /**
     * Prints tree recursively
     */
    void print_recursive(std::ostream& os, int pos, int space) {
      if (pos == -1) {
        return;
      }

      FlaggedBlock<Node> block = node_storage.read(pos);
      Node node = block.data;

      if (!block.is_valid()) {
        os << "INVALID NODE (this shouldn't happen)" << std::endl;
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