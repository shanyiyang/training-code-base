DIR_INC = ./include
DIR_SRC = ./src
DIR_OBJ = ./obj
DIR_BIN = ./bin

SRC  =  $(wildcard ${DIR_SRC}/*.c)
OBJ  =  $(patsubst %.c,${DIR_OBJ}/%.o,$(notdir ${SRC}))

LDFLAG = -l pthread
CFLAG = -Wall

TARGET = main

BIN_TARGET = ${DIR_BIN}/${TARGET}

CC = gcc

ALL:CHECK_D ${BIN_TARGET}

CHECK_D:
ifeq "$(wildcard ${DIR_OBJ})" ""
	@mkdir $(DIR_OBJ)
	@echo "Create $(DIR_OBJ)"
endif
ifeq "$(wildcard ${DIR_BIN})" ""
	@mkdir $(DIR_BIN)
	@echo "Create $(DIR_BIN)"
endif

${BIN_TARGET}:${OBJ}
	${CC} ${OBJ} -o $@
	
${DIR_OBJ}/%.o:${DIR_SRC}/*.c
	${CC} -c $< -o $@ $(CFLAG)

.PHONY:CLEAN
CLEAN:
	rm -rf ${DIR_OBJ}/*.o
	
run:
	@./bin/main

