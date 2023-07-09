SRCDIR = src_new
OBJDIR = obj
OUTFILE = sim.out

CC = gcc
LD = ld

CCFLAGS := -c -g

SRC_C   := $(shell find ${SRCDIR} -name "*.c")
OBJ     := $(patsubst ${SRCDIR}%.c, ${OBJDIR}%.o, ${SRC_C})

DIRS := $(shell find ${SRCDIR} -type d)
DIRS := $(patsubst ${SRCDIR}%, ${OBJDIR}%, ${DIRS})

all: ${DIRS} link

link: ${OBJ}
	@${CC} -o ${OUTFILE} $^

${DIRS}:
	@mkdir -p $@

${OBJDIR}/%.o: ${SRCDIR}/%.c
	@echo CC $<
	@${CC} ${CCFLAGS} -o $@ $<

.PHONY: clean
clean:
	rm -rf ${OBJDIR}