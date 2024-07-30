CC = g++
CCARGS = -O2 -Wall -Wshadow -std=gnu++1z -D CMS -D TESTSYS
CHECKER_LIST = \
	steps \
	habitat_distribution \
	diff

all: $(CHECKER_LIST)

clean:
	rm -f $(CHECKER_LIST)

$(CHECKER_LIST): %: %.cpp
	$(CC) $^ $(CCARGS) -o $@
