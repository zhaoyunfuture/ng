#include <gtest/gtest.h>

#include "src/tmp/tmp.h"

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
}

TEST(timeout, edge) {
  EXPECT_EQ(0,test_edge(true));
  sleep(1);
  EXPECT_EQ(0,test_edge(false));
  sleep(1);
  EXPECT_EQ(0,test_edge(true));
  sleep(1);
  EXPECT_EQ(3,test_edge(false));
}

//this dep-on timeout case,
//better introduce init, run every case cleanly and indepencely
TEST(no_meet, edge) {
  EXPECT_EQ(0,test_edge(true));
  sleep(1);
  EXPECT_EQ(0,test_edge(false));
  sleep(1);
  EXPECT_EQ(0,test_edge(true));
  sleep(1);
  EXPECT_EQ(4,test_edge(false));
}