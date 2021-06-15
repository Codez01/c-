

#include <iostream>
#include "sim_mem.cpp"

int main(int argc, const char * argv[]) {
    
    
    sim_mem mem_sm((char*) "exec_file.txt", (char*) "swap_file.txt" ,25, 50, 25,25, 25, 5);
    
    mem_sm.load(5);
    mem_sm.load(10);
    mem_sm.load(15);
    mem_sm.load(20);
    mem_sm.load(25);
    mem_sm.load(30);
    mem_sm.store(31,'y');
    mem_sm.load(35);
    mem_sm.load(40);
    mem_sm.load(45);
    mem_sm.load(50);
    mem_sm.load(1);
    mem_sm.load(30);
    mem_sm.load(60);
    mem_sm.store(80,'l');
    

    mem_sm.print_page_table();
    mem_sm.print_swap();
    mem_sm.print_memory();
    
    return 0;
    }
