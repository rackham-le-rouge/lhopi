#
# \file                 Makefile
# \author               Jerome GRARD
# \version              1.0
# \date                 8/14
# \brief                Makefile for Lhopi
#
# \details              Cleaning and compilation with the 70 options of the libcommon (http://www.thelibcommonproject.org)



#ignored option
# -Wc++-compat          : because don't stand the private keyword of the openMP call.
# -Wpadded              : need to fix it. There is a problem with the structure, memory is not optimized

CC=gcc
CFLAGS=         -W -Waggregate-return -Wall -Warray-bounds -Wbad-function-cast -Wcast-align -Wcast-qual         \
                 -Wchar-subscripts -Wcomment -Wdeclaration-after-statement -Werror -Wextra -Wfloat-equal        \
                 -Wformat -Wformat-nonliteral -Wformat-security -Wformat-y2k -Wignored-qualifiers -Wimplicit    \
                 -Wimplicit-function-declaration -Wimplicit-int -Winit-self -Winline -Winvalid-pch -Wlogical-op \
                 -Wlong-long -Wmain -Wmissing-braces -Wmissing-declarations -Wmissing-field-initializers        \
                 -Wmissing-include-dirs -Wmissing-noreturn -Wmissing-parameter-type -Wmissing-prototypes        \
                 -Wmultichar -Wnested-externs -Wno-multichar -Wnonnull -Wold-style-definition                   \
                 -Woverlength-strings -Wparentheses -Wpointer-arith -Wpointer-sign                              \
                 -Wreturn-type -Wsequence-point -Wshadow -Wsign-compare -Wstack-protector                       \
                 -Wswitch -Wswitch-default -Wswitch-enum -Wsync-nand -Wtrigraphs -Wundef -Wuninitialized        \
                 -Wunknown-pragmas -Wunsafe-loop-optimizations -Wunused -Wunused-function -Wunused-label        \
                 -Wunused-parameter -Wunused-value -Wunused-variable -Wvla -Wvolatile-register-var              \
                 -Wwrite-strings -fno-common -fstack-protector-all -pedantic -std=c99 -Wstrict-aliasing=3

LDFLAGS=-lncurses -ansi -O2
EXEC=lhopi

all: $(EXEC)






lhopi: drawing.o game.o main.o
	@$(CC) -o $@ $^ $(LDFLAGS)
	@echo [LD] $@

%.o: %.c
	@$(CC) -o $@ -c $< $(CFLAGS)
	@echo [CC] $@






.PHONY: clean mrproper

clean:
	@echo ---------------
	@echo      clean
	@echo ---------------
	rm -rf *.o

mrproper: clean
	@echo ---------------
	@echo   clean all
	@echo ---------------
	rm -rf $(EXEC)


