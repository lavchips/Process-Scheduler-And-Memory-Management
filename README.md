## Process-Scheduler-And-Memory-Management ##
#### Last Modified Date: May / 27 / 2020

COMP30023 Computer Systems Project 2

### Build 

$ make clean<br>
$ make

### Run

$ ./scheduler -f [filename] -a [scheduling-algorithm] -m [memory-allocation] -s [memory-size] -q [quantum]

-f filename <br>
-a scheduling-algorithm where scheduling algorithm is one of {ff,rr,cs} <br>
-m memory-allocation where memory-allocation is one of {u,p,v,cm} <br>
-s memory-size where memory-size is an integer indicating the size of memory in KB <br>
-q quantum where quantum is an integer (in seconds). The parameter will be used only for round-robin scheduling algorithm with the default value set to 10 seconds.<br>

ff first-come first-served
rr round-robin
cs customized scheduling algorithms

u unlimited memory
p swapping-X
v virtual memory
cm customized memory management

