#include <cstring>
#include <iostream>

#include "avl_database.hpp"

using namespace std;

typedef struct Student {
  char name[20];
  int ra;
} Student;

int main ()
{
  Student dario;
  strcpy(dario.name, "Dario");
  dario.ra = 16168;

  Student xerxes;
  strcpy(xerxes.name, "Xerxes");
  xerxes.ra = 16169;

  Student jorge;
  strcpy(jorge.name, "Jorge");
  jorge.ra = 16167;

  AvlDatabase<int, Student> students("data.bin", "tree.bin");

  // Add 10 students to AvlDatabase
  for (int i = 0; i < 10; i++) {
    Student generic;
    strcpy(generic.name, "Lorem Ipsum");
    generic.ra = i;
    students.add(generic.ra, generic);
  }

  students.print_tree(cout);
  students.remove(9);
  students.print_tree(cout);
  students.remove(8);
  students.print_tree(cout);
  students.remove(7);
  students.print_tree(cout);
  students.remove(6);
  students.print_tree(cout);

  return 0;
}