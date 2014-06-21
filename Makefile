CC=clang
CFLAGS=

SRC_DIR=src
BUILD_DIR=bin
DATA_DIR=data
SOURCES=$(wildcard ${SRC_DIR}/*.c)
TARGET=calculator_scanner
EXAMPLE_INPUT=example_input.txt

all: clean build run

build: 
	mkdir bin 2>/dev/null
	${CC} ${CFLAGS} ${SOURCES} -o ${BUILD_DIR}/${TARGET}

clean: 
	rm -rf ${BUILD_DIR}

run: 
	./${BUILD_DIR}/${TARGET} ${DATA_DIR}/${EXAMPLE_INPUT}
