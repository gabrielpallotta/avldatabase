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
  Student me;
  strcpy(me.name, "Pallotta");
  me.ra = 16168;

  AvlDatabase<int, Student> students("data.bin", "tree.bin");
  
  return 0;
}