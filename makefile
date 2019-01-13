SRC_DIR = src/
INC_DIR = includes/
DAT_DIR = dataset/
EXEC_NAME = myAE.exe
CC = gcc
CFLAGS = -W -Wall -lm -I $(INC_DIR)


compile:  $(SRC_DIR)keccak.c $(SRC_DIR)mrg32k3a.c $(SRC_DIR)arguments.c $(SRC_DIR)aes.c $(SRC_DIR)cipher.c $(SRC_DIR)main.c
	$(CC) $(CFLAGS) -o $(EXEC_NAME) $^

test: compile
	./$(EXEC_NAME) -o $(DAT_DIR)MyOutPutFile.txt -k MyPassword -c $(DAT_DIR)MyInputFile.txt
	./$(EXEC_NAME) -o $(DAT_DIR)Test.txt -k MyPassword -d $(DAT_DIR)MyOutPutFile.txt
	hexdump -C $(DAT_DIR)MyInputFile.txt
	hexdump -C $(DAT_DIR)MyOutPutFile.txt
	hexdump -C $(DAT_DIR)Test.txt


install_python_requirements:
	pip3 install -r report/pip-requirements.txt

execute_tests:
	python3 $(SRC_DIR)runner.py

clean:
	rm $(EXEC_NAME)
	rm $(DAT_DIR)/*

mrproper: clean
