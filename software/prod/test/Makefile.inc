TESTINCLUDES = -Itest
CFLAGS += `pkg-config --cflags check`
LIBS += `pkg-config --libs check` -lpthread

test/scheduler: test/scheduler_test.o test/hal/timer2.o src/scheduler.o
	$(CC) $(LDFLAGS) $+ $(LIBS) -o $@

test: test/scheduler
