#include <gtest/gtest.h>

int main(int argc, char** argv)
{
   printf("Running main() from %s\n", __FILE__);
   testing::InitGoogleTest(&argc, argv);
   testing::FLAGS_gtest_death_test_style = "threadsafe";
   return RUN_ALL_TESTS();
}