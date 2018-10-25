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
  students.add(dario.ra, dario);
  students.add(xerxes.ra, xerxes);
  students.add(jorge.ra, jorge);

  return 0;
}