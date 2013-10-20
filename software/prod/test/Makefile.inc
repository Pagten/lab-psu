TESTINCLUDES = -Itest
CFLAGS += `pkg-config --cflags check`
LIBS += `pkg-config --libs check` -lpthread

test/scheduler: $(BUILD_DIR)/test/scheduler_test.o $(BUILD_DIR)/test/hal/timer2.o $(BUILD_DIR)/src/scheduler.o
	$(CC) $(LDFLAGS) $+ $(LIBS) -o $@

test: test/scheduler
