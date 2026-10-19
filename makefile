FLAGS = -O3 -lm -lpthread
DEBUG_FLAGS = -O0 -g -lm -lpthread
CFILES = newton.c args.h args.c calc_attr.h calc_attr.c thrds.h thrds.c calc.h calc.c write_ppm.h write_ppm.c

.PHONY : all debug clean
all : newton

newton : $(CFILES)
	gcc $(CFILES) -o $@ $(FLAGS)

debug : $(CFILES)
	gcc $(CFILES) -o newton_debug $(DEBUG_FLAGS)

clean :
	rm -f newton newton_debug