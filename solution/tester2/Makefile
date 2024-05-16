CC = g++
CCARGS = -DEVAL -Wall -Wshadow -Wconversion -std=c++17 -O2 -pipe -static -s
SOLUTION_LIST = \
	steps_ek_1_2_not_3 \
	steps_ek_1_2_3_not_4 \
	steps_ek_1_2_3_4_not_5 \
	steps_ek_sqrt_e_map_without_cut \
	steps_ek_sqrt_e_vector_without_cut \
	steps_ek_sqrt_e

.PHONY: clean $(SOLUTION_LIST)

all: ${SOLUTION_LIST:%=%.exe}

${SOLUTION_LIST:%=%.exe}: %.exe: %.cpp
	$(CC) $^ $(CCARGS) -o $@

$(SOLUTION_LIST): %: %.exe
	$(info haha)

${SOLUTION_LIST:%=run_%}: run_%: %.exe
	./$^

clean:
	rm -f $(SOLUTION_LIST:%=%.exe)
