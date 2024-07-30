CC = g++
CCARGS = -O2 -Wall -Wshadow -std=gnu++1z -march=native
GENERATOR_LIST = test \
	steps \
	pa_odd_square \
	distance \
	habitat_distribution

all: $(GENERATOR_LIST)

clean:
	rm -f $(GENERATOR_LIST)

pa_odd_square: pa_odd_square.cpp
	$(CC) pa_odd_square.cpp $(CCARGS) -o pa_odd_square

run_pa_odd_square: pa_odd_square
	./pa_odd_square 1
	./pa_odd_square 2
	./pa_odd_square 3

pb_6174_generator: pb_6174_generator.cpp
	$(CC) pb_6174_generator.cpp $(CCARGS) -o pb_6174_generator

run_pb_6174_generator: pb_6174_generator
	./pb_6174_generator 1 0xc65e71f8a
	./pb_6174_generator 2 0xc75e73f8a
	./pb_6174_generator 3 0xcf5e75f8b

%.exe: %.cpp
	$(CC) $^ $(CCARGS) -o $@

steps: steps.exe
	$(info Hi, I'm actually steps.exe.)

run_steps: steps.exe
	./steps.exe random 1  1 0xe0166f01   1095   1095    547    547    547    547
	./steps.exe random 1  2 0xe0166f02  10954  10954   5477   5477     54     54
	./steps.exe random 1  3 0xe0166f09  30000  30000  15000  15000     20     20
	./steps.exe random 1  4 0xe0166f04 300000 300000 299998 299998 149999 149999
	./steps.exe random 1  5 0xe0166f07  31250  31250  18750  18750     16     16
	./steps.exe random 1  6 0xe0166f06 300000 300000 299999 299999 299998 299998
	./steps.exe   path 1  7 0x1D6F0DCF    537    547      5      8
	./steps.exe   path 1  8 0x1D6F0DD0    537    547      2      2
	./steps.exe   path 1  9 0x1D6F0DD1    537    547      3      3
	./steps.exe   path 1 10 0x1D6F0DD2     45     55      5      8
	./steps.exe   path 1 11 0x1D6F0DD3     45     55      2      2
	./steps.exe   path 1 12 0x1D6F0DD4     45     55      3      3
	./steps.exe   path 1 13 0x1D6F0DD5      4      5      5      8
	./steps.exe   path 1 14 0x1D6F0DD6      4      5      2      2
	./steps.exe   path 1 15 0x1D6F0DD7      4      5      3      3
	./steps.exe  path2 1 16 0xE872399C    537    547      5      8
	./steps.exe  path2 1 17 0xE872399D    537    547      2      2
	./steps.exe  path2 1 18 0xE872399E    537    547      3      3
	./steps.exe  path2 1 19 0xE872399F     45     55      5      8
	./steps.exe  path2 1 20 0xE87239A0     45     55      2      2
	./steps.exe  path2 1 21 0xE87239A1     45     55      3      3
	./steps.exe  path2 1 22 0xE87239A2      4      5      5      8
	./steps.exe  path2 1 23 0xE87239A3      4      5      2      2
	./steps.exe  path2 1 24 0xE87239A4      4      5      3      3
	./steps_2.py 300000 > 1_many_steps_2_300000.in
	./steps_2.py 100000 > 1_many_steps_2_100000.in
	./steps_3.py 300000 > 1_many_steps_3_300000.in
	./steps_3.py 100000 > 1_many_steps_3_100000.in
	./steps_4.py > 1_many_steps_4.in

distance: distance.cpp
	$(CC) distance.cpp $(CCARGS) -o distance

run_distance: distance
	./distance gen 1 0 -1 -- -1 -- 0 seed=1 > 1_001.in 
	./distance gen 99 0 -1 -- -1 -- 0 seed=1 > 1_002.in 
	./distance gen 100 0 -1 -- -1 -- 0 seed=1 > 1_003.in 
	./distance gen 100 0 -1 -- 1 1 2 seed=1 > 1_004.in 
	./distance gen 100 0 2 1 1 1 2 seed=1 > 1_005.in 
	./distance gen 100 1 -1 -- -1 -- 0 seed=1 > 1_006.in 
	./distance gen 100 2 -1 -- -1 -- 0 seed=1 > 1_007.in 
	./distance gen 100 20 -1 -- 4 25,2,2,10 0 seed=1 > 1_008.in 
	./distance gen 100 48 4 49,2,2,25 1 1 0 seed=1 > 1_009.in 
	./distance gen 100 49 1 1 4 49,2,2,25 0 seed=1 > 1_010.in 
	./distance gen 2999 0 -1 -- -1 -- 0 seed=1 > 2_001.in 
	./distance gen 3000 0 -1 -- -1 -- 0 seed=1 > 2_002.in 
	./distance gen 3000 0 1 1 -1 -- 1 seed=1 > 2_003.in 
	./distance gen 3000 0 2 1 1 1 2 seed=1 > 2_004.in 
	./distance gen 3000 1 -1 -- -1 -- 0 seed=1 > 2_005.in 
	./distance gen 3000 2 -1 -- -1 -- 0 seed=1 > 2_006.in 
	./distance gen 3000 10 4 11,2,4,10 4 11,2,4,10 0 seed=1 > 2_007.in 
	./distance gen 3000 200 4 201,2,2,25 4 201,2,2,25 0 seed=1 > 2_008.in 
	./distance gen 3000 1498 4 1499,2,2,1000 1 1 0 seed=1 > 2_009.in 
	./distance gen 3000 1499 4 1499,2,2,1000 1 1 0 seed=1 > 2_010.in 
	./distance gen 3000 1000 4 1001,2,2,1000 4 1001,2,2,1000 0 seed=1 > 2_011.in 
	./distance gen 3000 1000 4 1001,2,2,1000 1 1 2 seed=1 > 2_012.in 
	./distance gen 1 0 -1 -- -1 -- 0 seed=1 > 3_001.in 
	./distance gen 200000 0 -1 -- 1 1 2 seed=1 > 3_002.in 
	./distance gen 200000 0 1 1 1 1 3 seed=1 > 3_003.in 
	./distance gen 200000 0 2 1 1 1 2 seed=1 > 3_004.in 
	./distance gen 200000 0 4 1,2,10,1 4 1,2,10,1 seed=1 > 3_005.in 
	./distance gen 200000 0 3 -- 3 -- seed=1 > 3_006.in 
	./distance gen 200000 0 3 -- 1 1 2 seed=1 > 3_007.in 
	./distance gen 200000 1 -1 -- -1 -- 0 seed=1 > 4_001.in 
	./distance gen 200000 2 -1 -- -1 -- 0 seed=1 > 4_002.in 
	./distance gen 200000 5 4 6,2,10,3 4 6,2,10,5 0 seed=1 > 4_003.in 
	./distance gen 200000 20 4 21,2,3,25 -1 -- 0 seed=1 > 4_004.in 
	./distance gen 200000 20 4 21,2,3,25 1 1 2 seed=1 > 4_005.in 
	./distance gen 200000 20 4 21,2,3,25 4 21,2,3,25 0 seed=1 > 4_006.in 
	./distance gen 200000 20 3 -- 1 1 2 seed=1 > 4_007.in 
	./distance gen 200000 10 3 -- 3 -- 0 seed=1 > 4_008.in 
	./distance gen 200000 100 -1 -- -1 -- 0 seed=1 > 5_001.in 
	./distance gen 200000 1000 -1 -- 1 1 2 seed=1 > 5_002.in 
	./distance gen 200000 10000 3 -- 1 1 2 seed=1 > 5_003.in 
	./distance gen 200000 10000 4 10001,2,2,10000 1 1 2 seed=1 > 5_004.in 
	./distance gen 200000 20000 4 20001,2,2,20000 1 1 2 seed=1 > 5_005.in 
	./distance gen 200000 99990 4 100000,2,2,1000000 1 1 0 seed=1 > 5_006.in 
	./distance gen 200000 50000 4 50001,2,2,50000 1 1 0 seed=1 > 5_007.in 
	./distance gen 200000 50000 4 50001,2,2,50000 4 58763,2,2,10000 0 seed=1 > 5_008.in 

habitat_distribution: habitat_distribution.cpp
	$(CC) habitat_distribution.cpp $(CCARGS) -o habitat_distribution

run_habitat_distribution: habitat_distribution
	./habitat_distribution gen 100 300000 3 12,13 seed=1 > 1_01.in
	./habitat_distribution gen 1 300000 3 12 seed=1 > 1_02.in
	./habitat_distribution gen 1 300000 3 13 seed=1 > 1_03.in
	./habitat_distribution gen 100 300000 3 2,6,9,10,11 seed=1 > 2_01.in
	./habitat_distribution gen 100 300000 3 9,10,11 seed=1 > 2_03.in
	./habitat_distribution gen 100 300000 3 10,11 seed=1 > 2_04.in
	./habitat_distribution gen 100 300000 3 10,11 seed=2 > 2_05.in
	./habitat_distribution gen 1 300000 3 10 seed=1 > 2_06.in
	./habitat_distribution gen 1 300000 3 11 seed=1 > 2_07.in
	./habitat_distribution gen 99999 300000 3 6 seed=1 > 2_08.in
	./habitat_distribution gen 10 300000 3 17 seed=1 > 2_09.in
	./habitat_distribution gen 3 300000 3 17 seed=1 > 2_10.in
	./habitat_distribution gen 1 300000 3 6 seed=1 > 2_11.in
	./habitat_distribution gen 5 500 3 1,8,9  seed=1 > 3_01.in
	./habitat_distribution gen 10 500 3 2,6,15,16  seed=1 > 3_02.in
	./habitat_distribution gen 5 500 3 3,4,5  seed=1 > 3_02.in
	./habitat_distribution gen 5 500 3 7,17  seed=1 > 3_03.in
	./habitat_distribution gen 5 500 3 10,11  seed=1 > 3_04.in
	./habitat_distribution gen 5 500 3 14,18  seed=1 > 3_05.in
	./habitat_distribution gen 5 500 3 21,22  seed=1 > 3_07.in
	./habitat_distribution gen 1 100000 3 1,3,9,12,13,14,15,16 seed=513522658358386294 > 4_01.in
	./habitat_distribution gen 100 300000 3 0,8,14,15 seed=1 > 4_02.in
	./habitat_distribution gen 1000 300000 3 0,8,14,15 seed=1 > 4_03.in
	./habitat_distribution gen 100 300000 3 1,3,4,5,7,14,16 seed=1 > 4_04.in
	./habitat_distribution gen 1000 300000 3 1,3,4,5,7,14,16 seed=1 > 4_05.in
	./habitat_distribution gen 1 300000 3 1 seed=1 > 4_06.in
	./habitat_distribution gen 1 300000 3 3 seed=1 > 4_07.in
	./habitat_distribution gen 1 300000 3 4 seed=1 > 4_08.in
	./habitat_distribution gen 1 300000 3 5 seed=1 > 4_09.in
	./habitat_distribution gen 1 300000 3 5 seed=2 > 4_10.in
	./habitat_distribution gen 1 300000 3 7 seed=1 > 4_11.in
	./habitat_distribution gen 1 300000 3 14 seed=1 > 4_12.in
	./habitat_distribution gen 4285 300000 7 14 seed=1 > 4_13.in
	./habitat_distribution gen 1 300000 3 16 seed=1 > 4_14.in
	./habitat_distribution gen 1 300000 3 18 seed=1 > 4_15.in
	./habitat_distribution gen 3 300000 100000 1,14,18 seed=1 > 4_16.in
	./habitat_distribution gen 3 300000 100000 20 seed=1 > 4_17.in
	./habitat_distribution gen 1 300000 3 20 seed=1 > 4_18.in
	./habitat_distribution gen 1 300000 3 21 seed=1 > 4_19.in
	./habitat_distribution gen 3 300000 100000 21 seed=1 > 4_20.in
	./habitat_distribution gen 1 300000 3 22 seed=1 > 4_21.in
	./habitat_distribution gen 3 300000 100000 22 seed=1 > 4_22.in
	./habitat_distribution gen 1 300000 3 23 seed=1 > 4_23.in
	./habitat_distribution gen 3 300000 100000 23 seed=1 > 4_24.in
	./habitat_distribution gen 1 300000 3 24 seed=1 > 4_25.in
	./habitat_distribution gen 3 300000 100000 24 seed=1 > 4_26.in
	./habitat_distribution gen 3 300000 100000 25 seed=1 > 4_27.in
	./habitat_distribution gen 1 300000 3 26 seed=1 > 4_28.in
	./habitat_distribution gen 3 300000 100000 26 seed=1 > 4_29.in
	./habitat_distribution gen 3 300000 100000 27 seed=1 > 4_30.in
