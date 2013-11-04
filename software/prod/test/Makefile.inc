TESTINCLUDES = -Itest
CFLAGS += `pkg-config --cflags check`
LIBS += `pkg-config --libs check` -lpthread

test/all_tests: $(BUILD_DIR)/test/scheduler_test.o $(BUILD_DIR)/test/spi_master_test.o $(BUILD_DIR)/test/hal/spi.o $(BUILD_DIR)/test/hal/timer2.o $(BUILD_DIR)/src/scheduler.o $(BUILD_DIR)/src/spi_master.o $(BUILD_DIR)/test/all_tests.o
	$(CC) $(LDFLAGS) $+ $(LIBS) -o $@

test: test/all_tests
