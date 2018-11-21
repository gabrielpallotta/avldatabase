#include <cstring>
#include <cmath>
#include <algorithm>
#include <stdexcept>

#include "avl_database.hpp"
#include "gtest/gtest.h"

using namespace std;

AvlDatabase<int, int> tree("test_data.bin", "test_tree.bin");

bool validHeight(int n, int height) {
  if (height < ceil(log2(n + 1))) {
    return false;
  }

  if (height > floor(1.44f * log2(n + 2) - 0.328f)) {
    return false;
  }

  return true;
}

TEST(AvlDatabaseTest, InsertsAndGetsValue) {
  tree.add(0, 0);
  ASSERT_EQ(0, tree.get(0));
  tree.add(-1, -1);
  ASSERT_EQ(-1, tree.get(-1));
  tree.add(1, 1);
  ASSERT_EQ(1, tree.get(1));
}