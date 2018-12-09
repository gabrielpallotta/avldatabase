#ifndef BINARYSTORAGE_H
#define BINARYSTORAGE_H

#include <stdexcept>
#include <ios>
#include <iostream>
#include <fstream>

/**
 * Simple encapsulator
 */
template <typename T>
class FlaggedBlock {
  public:
    int valid;
    T data;

    FlaggedBlock() { }
  
    FlaggedBlock(int valid, T data) {
      this->valid = valid;
      this->data = data;
    }

    bool is_valid() {
      return valid == 1;
    }
};


/**
 * Implementation of a class that stores an array of a certain type on a binary
 * file and has implementations of basic CRUD (create, remove, update and
 * delete).
 * 
 * The deletion process just marks a block as invalid.
 * 
 * It also allows flags (of type int) that will be written on the start
 * of the file.
 *
 * @tparam T The type of the info stored 
 */
template <typename T>
class BinaryStorage {
  public:
    BinaryStorage(std::string path, int number_of_flags) {
      this->number_of_flags = number_of_flags;

      // Create files if it doesn't exist
      file.open(path, std::ios::app);
      file.close();
      // Open files for reading / writing
      file.open(path, std::ios::in | std::ios::out | std::ios::binary | std::ios::ate);
      
      // Write default flags if file is empty
      if (is_empty()) {
        for (int i = 0; i < number_of_flags; i++) {
          write_flag(i, -1);
        }
      }
    }

    int read_flag(int index) {
      int flag;
      file.clear();
      file.seekg(index * sizeof(int), std::ios::beg);
      file.read(reinterpret_cast<char*>(&flag), sizeof(int));
      return flag;
    }

    void write_flag(int index, int flag) {
      file.clear();
      file.seekp(index * sizeof(int), std::ios::beg);
      file.write(reinterpret_cast<char*>(&flag), sizeof(int));
      file.flush();
    }

    FlaggedBlock<T> read(int index) {
      T data;
      int valid;
      file.clear();
      file.seekg(get_binary_pos(index), std::ios::beg);
      file.read(reinterpret_cast<char*>(&valid), sizeof(int));
      file.read(reinterpret_cast<char*>(&data), sizeof(T));
      FlaggedBlock<T> block(valid, data);
      return block;
    }

    int write(FlaggedBlock<T> block) {
      int index = get_insertion_index();
      write_block(block, index);
      return index;
    }

    int write(FlaggedBlock<T> block, int index) {
      write_block(block, index);
      return index;
    }
    
    // void update(int index, FlaggedBlock block) {
    //   T* data_ptr = new T(data);
      
    //   file.clear();
    //   file.seekp(get_binary_pos(index), std::ios::beg);
    //   file.write(reinterpret_cast<char*>(data_ptr), sizeof(T));
    //   file.flush();
    // }

    void remove(int index) {
      int valid = 0;
      file.clear();
      file.seekp(get_binary_pos(index), std::ios::beg);
      file.write(reinterpret_cast<char*>(&valid), sizeof(int));
      file.flush();
    }

    void swap(int index_a, int index_b) {
      FlaggedBlock<T> block_a;
      if (index_a == -1) {
        block_a.valid = -1;
      } else {
        block_a = read(index_a);
      }
      
      FlaggedBlock<T> block_b;
      if (index_b == -1) {
        block_b.valid = -1;
      } else {
        block_b = read(index_b);
      }

      write(block_a, index_b);
      write(block_b, index_a);
    }

    bool is_empty() {
      return (get_file_size() == 0);
    }
    
  private:
    std::fstream file;
    int number_of_flags;

    int get_file_size() {
      file.clear();
      file.seekg(0, std::ios::end);
      int size = file.tellg();
      file.clear();
      return size;
    }
    
    int get_binary_pos(int index) {
      return number_of_flags * sizeof(int) + index * (sizeof(T) + sizeof(int));
    }

    int get_data_count() {
      file.clear();
      // int file_size = get_file_size();
      // int first = (int)(get_file_size) - (int)(number_of_flags * sizeof(int));
      // int second = (int)(sizeof(T) + sizeof(int));
      return ((int)(get_file_size()) - (int)(number_of_flags * sizeof(int))) / (int)(sizeof(T) + sizeof(int));
    }

    int get_insertion_index() {
      int data_count = get_data_count();

      for(int i = 0; i < data_count; i++) {
        FlaggedBlock<T> block = read(i);
        if (!block.is_valid()) {
          return i;
        }
      }
      
      return data_count;
    }

    void write_block(FlaggedBlock<T> block, int index) {
      file.clear();
      file.seekp(get_binary_pos(index), std::ios::beg);
      file.write(reinterpret_cast<char*>(&block.valid), sizeof(int));
      file.write(reinterpret_cast<char*>(&block.data), sizeof(T));
      file.flush();
    }

};

#endif