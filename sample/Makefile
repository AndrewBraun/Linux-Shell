.phony all:
all: sample inf args ssi

sample: sample.c
	gcc sample.c -lreadline -lhistory -ltermcap -o sample

inf: inf.c
	gcc inf.c -o inf

args: args.c
	gcc args.c -o args

ssi: ssi.c
	gcc ssi.c -lreadline -lhistory -ltermcap -o ssi

.PHONY clean:
clean:
	-rm -rf *.o *.exe
