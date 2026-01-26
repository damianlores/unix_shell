# USE "make" IN COMMAND LINE TO CREATE OBJECT FILES AND EXECUTABLE "a.out"
# THEN OPEN IN CMD WITH "./a.out"
CFLAGS = -Wall -g
TARGET = a.out
SRCS = p3.c commands.c dynamic_list.c shell.c file_system.c mem_management.c processes.c

$(TARGET): $(SRCS)
	gcc $(CFLAGS) $(SRCS) -o $(TARGET)

%: $(SRCS)
	gcc $(CFLAGS) $(SRCS) -o $@.out

clean:
	rm -f *.out

