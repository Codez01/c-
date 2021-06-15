

#ifndef sim_mem_h
#define sim_mem_h
#define MEMORY_SIZE 25
char main_memory[MEMORY_SIZE];
using namespace  std;
typedef struct page_descriptor
{
    unsigned int V; // valid
    unsigned int D; // dirty
    unsigned int P; // permission
    unsigned int frame; //the number of a frame if in case it is page-mapped
    int swap_index;
} page_descriptor;

class sim_mem {
    int swapfile_fd;
    int program_fd;
    int text_size;
    int data_size;
    int bss_size;
    int heap_stack_size;
    int num_of_pages;
    int page_size;

    page_descriptor *page_table;  //pointer to page table
public:

    ~sim_mem();
    sim_mem(char exe_file_name[], char swap_file_name[], int text_size,int data_size, int bss_size, int heap_stack_size, int num_of_pages, int page_size);

    char load(int address);
    void store(int address, char value);
    void print_memory();
    void print_swap ();
    void print_page_table();
    void swapfrm(int demdpg);
    int swapint;
};

#endif /* Header_h */
