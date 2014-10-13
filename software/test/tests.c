
#include <check.h>
#include <stdlib.h>

#include "clock_test.h"
#include "timer_test.h"
#include "spi_master_test.h"
#include "rotary_test.h"
#include "mcp4922_test.h"
#include "process_test.h"
#include "pwlf_test.h"

int main(void)
{
  int number_failed;

  SRunner *sr = srunner_create(clock_suite());
  srunner_add_suite(sr, timer_suite());
  srunner_add_suite(sr, spi_master_suite());
  srunner_add_suite(sr, rotary_suite());
  srunner_add_suite(sr, mcp4922_suite());
  srunner_add_suite(sr, process_suite());
  srunner_add_suite(sr, pwlf_suite());

  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
