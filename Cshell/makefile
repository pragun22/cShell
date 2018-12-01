TARGETS = cShell

all: $(TARGETS)

%o: %.c
	gcc -g -c -o  $@ $<
cShell: cd.o clrspaces.o echo.o exec.o handler.o ls.o ls_l.o main.o pinfo.o pwd.o relative_path.o remindme.o redirection.o jobs.c
	gcc -o cShell cd.o clrspaces.o echo.o exec.o handler.o ls.o ls_l.o main.o pinfo.o pwd.o relative_path.o remindme.o redirection.o jobs.c

.PHONY: clean
clean:
	rm -rf $(TARGETS) *.o
