
==Description==

It's a program that embodiments the way that MemoryManaging happens in computers (takes each proccess and divide  it into pages depending on the page size
assigned by the proccessor) the program includes two functions load and store, load function checks whether the page we wanna load is in the main memory, if yes then the function will return the
page from main memory, if not it checks whether the main memory is full or not and if it has a reading or writing permissions . if the memory is not full then it will go find the page in (swap file if it has a writing permissions)
else it has a reading permissions that indicates that it can be found in the executable file and finally move it to the main memory in order to load it.
if the main memory is full then we decide from where we can find the requested page from swapfile or executable file depending on the points that were mentioned above, and then we'll have to replace it with a specific
chosen frame using a queue, this queue that can determine the least used page by adding the chosen frame to the end of the queue each time. and after that the page that is found in the chosen frame is deleted if it has reading permissions
else the page is moved to the swapfile and is given a swapfile index that determines the place of the page in the swapfile by using an array called swaparray_checker which is intialized with -1 and each time we save a page in the swap array
the index of that page in the swaparray_checker is changed to a value and if moved from swap array the index becomes -1 which means the place is free again.
if the requested page was from type bss  a new page intialized with zeros is stored and finally if it was from type heap or stack, an error message pops out.
the functions store , stores a page in the main memory with a specific character in a specific address within the range of the page's address  and also checks wether the main memory is full and does the same things that load does.
and surely checks if the page was from type bss, heap or stack and stores the specified character with zeros in the rest.
 
the whole actions are done using a table called page table which saves where each paged is saved in the main memory , swap etc
and the value of dirty , permissions and wether the page is placed in the main memory or not ( valid or invalid) 



functions:
the program have  function:
print_memory() which basically prints the whole memory
print_swap() which prints the whole swapfile
print_page_table() prints the page table 
sim_mem(char exe_file_name[], char swap_file_name[], int text_size, int data_size, int bss_size, int heap_stack_size, int num_of_pages, int page_size) it is a constructor that takes the data and the files names.
~sim_mem() it is a destructor that frees the memory from malloced arrays , strings and closes the opened files
void store(int address, char value) method that is for storing a specific character at a specific address in the main memory
char sim_mem::load(int address)  method that loads a specific page in the main memory



==Program Files==
header:
sim_mem.h

classes:
sim_mem.cpp
main.cpp




==How to compile?==

compile: g++ sim_mem.cpp main.cpp -o main -Wall


=======Input ==========

there aren't any inputs

==========Output=============
page table 
Valid	 Dirty	 Permission 	 Frame
[1]	[0]	[0]	[0]	
[0]	[0]	[0]	[-1]	
[0]	[0]	[0]	[-1]	
[0]	[0]	[0]	[-1]	
[0]	[0]	[0]	[-1]	
[0]	[1]	[1]	[-1]	
[1]	[0]	[1]	[1]	
[0]	[1]	[1]	[-1]	
[0]	[1]	[1]	[-1]	
[0]	[1]	[1]	[-1]	
[1]	[0]	[1]	[4]	
[0]	[0]	[1]	[-1]	
[1]	[0]	[1]	[2]	
[0]	[0]	[1]	[-1]	
[0]	[0]	[1]	[-1]	
[0]	[0]	[1]	[-1]	
[1]	[1]	[1]	[3]	
[0]	[0]	[1]	[-1]	
[0]	[0]	[1]	[-1]	
[0]	[0]	[1]	[-1]	
[0]	[0]	[1]	[-1]	
[0]	[0]	[1]	[-1]	
[0]	[0]	[1]	[-1]	
[0]	[0]	[1]	[-1]	
[0]	[0]	[1]	[-1]	

 Swap memory
0 - [f]	1 - [f]	2 - [f]	3 - [f]	4 - [f]	
0 - [i]	1 - [i]	2 - [i]	3 - [i]	4 - [i]	
0 - [h]	1 - [h]	2 - [h]	3 - [h]	4 - [h]	
0 - [j]	1 - [j]	2 - [j]	3 - [j]	4 - [j]	
0 - [0]	1 - [0]	2 - [0]	3 - [0]	4 - [0]	
0 - [0]	1 - [0]	2 - [0]	3 - [0]	4 - [0]	
0 - [0]	1 - [0]	2 - [0]	3 - [0]	4 - [0]	
0 - [0]	1 - [0]	2 - [0]	3 - [0]	4 - [0]	
0 - [0]	1 - [0]	2 - [0]	3 - [0]	4 - [0]	
0 - [0]	1 - [0]	2 - [0]	3 - [0]	4 - [0]	
0 - [0]	1 - [0]	2 - [0]	3 - [0]	4 - [0]	
0 - [0]	1 - [0]	2 - [0]	3 - [0]	4 - [0]	
0 - [0]	1 - [0]	2 - [0]	3 - [0]	4 - [0]	
0 - [0]	1 - [0]	2 - [0]	3 - [0]	4 - [0]	
0 - [0]	1 - [0]	2 - [0]	3 - [0]	4 - [0]	
0 - [0]	1 - [0]	2 - [0]	3 - [0]	4 - [0]	
0 - [0]	1 - [0]	2 - [0]	3 - [0]	4 - [0]	
0 - [0]	1 - [0]	2 - [0]	3 - [0]	4 - [0]	
0 - [0]	1 - [0]	2 - [0]	3 - [0]	4 - [0]	
0 - [0]	1 - [0]	2 - [0]	3 - [0]	4 - [0]	

 Physical memory
[a]
[a]
[a]
[a]
[a]
[g]
[y]
[g]
[g]
[g]
[m]
[m]
[m]
[m]
[m]
[l]
[0]
[0]
[0]
[0]
[k]
[k]
[k]
[k]
[k]
