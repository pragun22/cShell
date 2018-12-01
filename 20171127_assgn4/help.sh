rm output.txt
gcc 3_normal.c
./a.out < input.txt | grep "time">> output.txt
gcc 3_process.c
./a.out < input.txt | grep "time">> output.txt
gcc -lpthread 3_thread.c
./a.out < input.txt | grep "time" >>output.txt

