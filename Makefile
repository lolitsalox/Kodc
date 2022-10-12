TARGET = kod


SRCDIR := src
OBJDIR := lib

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRC = $(call rwildcard,$(SRCDIR),*.c)          
OBJS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRC))
DIRS = $(wildcard $(SRCDIR)/*)

CC = gcc
CFLAGS = -g -m64 -Wall -pedantic-errors

target: ${OBJS} link
	
${OBJDIR}/%.o: ${SRCDIR}/%.c
	${CC} ${CFLAGS} -c $^ -o $@

link:
	${CC} ${OBJS} -o ${TARGET}.exe

clean:
	rm -r lib/*.o