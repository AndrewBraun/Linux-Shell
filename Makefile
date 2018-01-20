.phony all:
all: inf ssi

inf: inf.c
	gcc inf.c -o inf

ssi: ssi.c
	gcc ssi.c -lreadline -lhistory -ltermcap -o ssi

.PHONY clean:
clean:
	-rm -rf *.o *.exe
