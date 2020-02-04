#include "../tap/tap.h"

int main() {
  // Number of tests
  plan(5);
  int bronze = 1, silver = 2, gold = 3;

  // Test 1
  ok(bronze < silver, "bronze is less than silver");

  // Test 2
  ok(bronze > silver, "not quite");

  // Test 3
  is("gold", "gold", "gold is gold");

  // Test 4
  cmp_ok(silver, "<", gold, "%d <= %d", silver, gold);

  // Test 5
  like("platinum", ".*inum", "platinum matches .*inum");

  done_testing();
}