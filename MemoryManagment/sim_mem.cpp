#include <iostream>

#include <unistd.h>

#include <fcntl.h>

#include "sim_mem.h"

#include<queue>

#include <string.h>

#include <string>

using namespace std;

queue < int > Q;
queue < int > Q_Checker;
int * SwapArray_checker;
char * SwapArray;
char swap_file_Name[510];
int length;
int Read_permission_only2;

//****************************************************************CONSTRUCTOR************************************************************************

sim_mem::sim_mem(char exe_file_name[], char swap_file_name[], int text_size, int data_size, int bss_size, int heap_stack_size, int num_of_pages, int page_size) {

    this -> bss_size = bss_size;
    this -> num_of_pages = num_of_pages;
    this -> page_size = page_size;
    this -> heap_stack_size = heap_stack_size;
    this -> text_size = text_size;
    this -> data_size = data_size;

    strcpy(swap_file_Name, swap_file_name); //copy swap file name

    program_fd = open(exe_file_name, O_RDWR);
    if (program_fd == -1) {
        perror("error while loading file");
        this->sim_mem::~sim_mem();
        exit(1);
    }

    if ((swapfile_fd = open(swap_file_name, O_RDWR)) == -1) {
        swapfile_fd = open(swap_file_name, O_CREAT | O_RDWR); //if the file doesn't exist create one
    }

    for (int i = 0; i < MEMORY_SIZE; i++) { //intializing an array of memory containing zeros

        main_memory[i] = 0; //each index contains zero
    }

    page_table = new page_descriptor[num_of_pages]; //intialization of pages table

    int swap_File_Size = page_size * (num_of_pages - (text_size / page_size)); //size of the swap file

    char swapArrayOfZeros[swap_File_Size];

    for (int i = 0; i < swap_File_Size; i++) {
        swapArrayOfZeros[i] = '0';

    }

    write(swapfile_fd, swapArrayOfZeros, swap_File_Size); //intialize N zeros to the swipe file

    int Read_permission_only = text_size / page_size;

    for (int i = 0; i < num_of_pages; i++) { //intializing the table

        page_table[i].frame = -1; //intialize frame to be -1
        page_table[i].D = 0; //intialize dirty to be 0
        page_table[i].V = 0; //intialize validation to be 0

        if (i < Read_permission_only) {
            page_table[i].P = 0;

        } else {

            page_table[i].P = 1;
        }
    }

    for (int i = 0; i < (MEMORY_SIZE / page_size); i++) { //queue that contains all the frames

        Q.push(i);

    }
    Read_permission_only2 = Read_permission_only;
    length = swap_File_Size;

    SwapArray_checker = new int[swap_File_Size]; // swap array that decides where to put the pages
    for (int i = 0; i < (swap_File_Size); i++) {
        SwapArray_checker[i] = -1;
    }
    SwapArray = new char[swap_File_Size]; // swap array that decides where to put the pages
    for (int i = 0; i < (swap_File_Size); i++) {
        SwapArray[i] = '0';
    }

}
//****************************************************************LOAD**********************************************************************************

char sim_mem::load(int address) { // method that loads a specific page in the main memory

    int offsetNumber = address % page_size;

    int PageNumber = address / page_size;

    if (page_table[PageNumber].V == 1) { //if the page was valid

        unsigned int frame = page_table[PageNumber].frame; // the value of the frame in the pagenumber place within the table
        int physicalAddress = page_size * frame + offsetNumber; //calculating the physical address
        return main_memory[physicalAddress]; //return the stored value that was stored in the main memory

    } else { //not valid in the main memory

        if (page_table[PageNumber].P == 0) { //it only has the permission to read(text)

            int skip = PageNumber * page_size;

            char buffer[5];
            unsigned int frame;
            lseek(program_fd, skip, SEEK_SET);
            read(program_fd, buffer, page_size);

            if ((int) Q_Checker.size() == (MEMORY_SIZE / page_size)) { //full memory

                frame = Q.front();
                Q.push(Q.front());
                Q.pop(); //pop from the first index
                Q_Checker.pop(); //decrementing the size of Q_checker Queue

                //move to swap file ...

                int j = 0;
                int n = 0;
                int k;
                while (j != length) {
                    if (SwapArray_checker[j] == -1) { //first empty place
                        SwapArray_checker[j] = frame; //the place is reserved for the frame
                        for (k = j * page_size; k < (j * page_size + 5); k++) {
                            SwapArray[k] = main_memory[(frame * page_size) + n];

                            n++;

                        }

                        int swap_File_Size = page_size * (num_of_pages - (text_size / page_size)); //size of the swap file

                        close(swapfile_fd);
                        remove(swap_file_Name);

                        if ((swapfile_fd = open(swap_file_Name, O_RDWR)) == -1) {
                            swapfile_fd = open(swap_file_Name, O_CREAT | O_RDWR); //if the file doesn't exist create one
                        }

                        write(swapfile_fd, SwapArray, swap_File_Size); //intialize N zeros to the swipe file
                        for (int i = 0; i < num_of_pages; i++) {
                            if (page_table[i].frame == frame) {
                                if (page_table[i].P != 0) {
                                    page_table[i].D = 1;
                                    page_table[i].frame = -1;
                                    page_table[i].swap_index = j * page_size;
                                    page_table[i].V = 0;
                                }

                            }
                        }

                        break;

                    }
                    j++;
                }

                for (int i = 0; i < num_of_pages; i++) { // clear the page from the swap

                    if (page_table[i].frame == frame) {

                        if (page_table[i].P == 0) {
                            SwapArray_checker[j] = -1;

                            for (k = j * page_size; k < (j * page_size + 5); k++) {
                                SwapArray[k] = '0';

                                n++;

                            }

                            int swap_File_Size =
                                    page_size * (num_of_pages - (text_size / page_size)); //size of the swap file

                            close(swapfile_fd);
                            remove(swap_file_Name);

                            if ((swapfile_fd = open(swap_file_Name, O_RDWR)) == -1) {
                                swapfile_fd = open(swap_file_Name,
                                                   O_CREAT | O_RDWR); //if the file doesn't exist create one
                            }

                            write(swapfile_fd, SwapArray, swap_File_Size); //intialize N zeros to the swipe file

                            for (int i = 0; i < num_of_pages; i++) {

                                if (page_table[i].frame == frame) {
                                    page_table[i].D = 0;
                                    page_table[i].swap_index = 0;
                                    page_table[i].V = 0;
                                    page_table[i].frame = -1;
                                }

                            }

                        }
                    }

                }

            }
                //the page has been sent to swap
            else {
                frame = Q.front();
                Q.push(Q.front());
                Q.pop(); //pop from the first index

            }

            int Page_physicalAddress = page_size * frame; //physical address for the beginning of a page
            int physicalAddress = page_size * frame + offsetNumber; //physical address for the addressed char

            for (int i = 0; i < page_size; i++) {
                main_memory[Page_physicalAddress + i] = buffer[i]; //copy the page tot the main memory

            }

            page_table[PageNumber].V = 1; //the place becomes valid
            Q_Checker.push(1); //incrementing the size of Q_checker Queue

            page_table[PageNumber].frame = frame;

            return main_memory[physicalAddress]; // return

        } else { //it has the permission to write (heap, stack , bss ,data)

            if (page_table[PageNumber].D == 1) { //if it was dirty then it will be located in swap

                int skip = page_table[PageNumber].swap_index;

                char buffer[5];
                unsigned int frame;

                lseek(swapfile_fd, skip, SEEK_SET);
                read(swapfile_fd, buffer, page_size);
                int swap_File_Size = page_size * (num_of_pages - (text_size / page_size)); //size of the swap file

                if ((int) Q_Checker.size() == (MEMORY_SIZE / page_size)) { //full memory

                    frame = Q.front();
                    Q.push(Q.front());
                    Q.pop(); //pop from the first index
                    Q_Checker.pop(); //decrementing the size of Q_checker Queue
                    //move to swap file ...

                    int j = 0;
                    int n = 0;
                    int k;

                    while (j != length) {

                        if (SwapArray_checker[j] == -1) { //first empty place
                            SwapArray_checker[j] = frame; //the place is reserved for the frame

                            for (k = j * page_size; k < (j * page_size + 5); k++) {
                                SwapArray[k] = main_memory[(frame * page_size) + n];
                                n++;
                            }

                            for (int i = skip; i < page_size +
                                                   skip; i++) { //loop from the swapindex until the page size in the swap array and give each index the value of 0

                                SwapArray[i] = '0';

                            }

                            SwapArray_checker[skip /
                                              page_size] = -1; //it tells the swap array checker where there is a free space
                            page_table[PageNumber].swap_index = 0;
                            page_table[PageNumber].D = 0;

                            close(swapfile_fd);
                            remove(swap_file_Name);

                            if ((swapfile_fd = open(swap_file_Name, O_RDWR)) == -1) {
                                swapfile_fd = open(swap_file_Name,
                                                   O_CREAT | O_RDWR); //if the file doesn't exist create one
                            }
                            write(swapfile_fd, SwapArray, swap_File_Size);
                            for (int i = 0; i < num_of_pages; i++) {
                                if (page_table[i].frame == frame) {
                                    if (page_table[i].P != 0) {
                                        page_table[i].D = 1;
                                        page_table[i].frame = -1;
                                        page_table[i].swap_index = j * page_size;
                                        page_table[i].V = 0;
                                    }

                                }
                            }

                            break;

                        }
                        j++;

                    }

                    for (int i = 0; i < num_of_pages; i++) {

                        if (page_table[i].frame == frame) {

                            if (page_table[i].P == 0) {
                                SwapArray_checker[j] = -1;

                                for (k = j * page_size; k < (j * page_size + 5); k++) {
                                    SwapArray[k] = '0';

                                    n++;

                                }

                                int swap_File_Size =
                                        page_size * (num_of_pages - (text_size / page_size)); //size of the swap file

                                close(swapfile_fd);
                                remove(swap_file_Name);

                                if ((swapfile_fd = open(swap_file_Name, O_RDWR)) == -1) {
                                    swapfile_fd = open(swap_file_Name,
                                                       O_CREAT | O_RDWR); //if the file doesn't exist create one
                                }

                                write(swapfile_fd, SwapArray, swap_File_Size); //intialize N zeros to the swipe file

                                for (int i = 0; i < num_of_pages; i++) {

                                    if (page_table[i].frame == frame) {
                                        page_table[i].D = 0;
                                        page_table[i].swap_index = 0;
                                        page_table[i].V = 0;
                                        page_table[i].frame = -1;
                                    }

                                }

                            }
                        }

                    }

                } else {

                    frame = Q.front();

                    Q.push(Q.front());
                    Q.pop(); //pop from the first index

                    for (int i = skip; i < page_size + skip; i++) { //loop from the swapindex until the page size in the swap array and give each index the value of 0

                        SwapArray[i] = '0';

                    }

                    SwapArray_checker[skip / page_size] = -1; //it tells the swap array checker where there is a free space
                    page_table[PageNumber].swap_index = 0;
                    page_table[PageNumber].D = 0;

                    close(swapfile_fd);
                    remove(swap_file_Name);

                    if ((swapfile_fd = open(swap_file_Name, O_RDWR)) == -1) {
                        swapfile_fd = open(swap_file_Name, O_CREAT | O_RDWR); //if the file doesn't exist create one
                    }
                    write(swapfile_fd, SwapArray, swap_File_Size);

                }

                int Page_physicalAddress = page_size * frame; //physical address for the beginning of a page
                int physicalAddress = page_size * frame + offsetNumber;

                for (int i = 0; i < page_size; i++) {
                    main_memory[Page_physicalAddress + i] = buffer[i];
                }
                page_table[PageNumber].V = 1;
                page_table[PageNumber].frame = frame;

                Q_Checker.push(1); //incrementing the size of Q_checker Queue

                return main_memory[physicalAddress];

            } else { //if it wasn't dirty check if the type was heap, bss,stack or data

                int pages_of_text = text_size / page_size;
                int pages_of_data = data_size / page_size;
                unsigned int frame;

                if ((int) Q_Checker.size() == (MEMORY_SIZE / page_size)) { //full memory

                    frame = Q.front();
                    Q.push(Q.front());
                    Q.pop(); //pop from the first index
                    Q_Checker.pop(); //decrementing the size of Q_checker Queue

                    //move to swap file ...

                    int j = 0;
                    int n = 0;
                    int k;
                    while (j != length) {
                        if (SwapArray_checker[j] == -1) { //first empty place
                            SwapArray_checker[j] = frame; //the place is reserved for the frame
                            for (k = j * page_size; k < (j * page_size + 5); k++) {

                                SwapArray[k] = main_memory[(frame * page_size) + n];
                                n++;
                            }

                            int swap_File_Size =
                                    page_size * (num_of_pages - (text_size / page_size)); //size of the swap file

                            close(swapfile_fd);
                            remove(swap_file_Name);

                            if ((swapfile_fd = open(swap_file_Name, O_RDWR)) == -1) {
                                swapfile_fd = open(swap_file_Name,
                                                   O_CREAT | O_RDWR); //if the file doesn't exist create one
                            }
                            write(swapfile_fd, SwapArray, swap_File_Size); //intialize N zeros to the swipe file
                            for (int i = 0; i < num_of_pages; i++) {
                                if (page_table[i].frame == frame) {
                                    if (page_table[i].P != 0) {
                                        page_table[i].D = 1;
                                        page_table[i].frame = -1;
                                        page_table[i].swap_index = j * page_size;
                                        page_table[i].V = 0;
                                    }

                                }
                            }

                            break;

                        }
                        j++;

                    }

                    for (int i = 0; i < num_of_pages; i++) {

                        if (page_table[i].frame == frame) {

                            if (page_table[i].P == 0) {
                                SwapArray_checker[j] = -1;

                                for (k = j * page_size; k < (j * page_size + 5); k++) {
                                    SwapArray[k] = '0';

                                    n++;

                                }

                                int swap_File_Size =
                                        page_size * (num_of_pages - (text_size / page_size)); //size of the swap file

                                close(swapfile_fd);
                                remove(swap_file_Name);

                                if ((swapfile_fd = open(swap_file_Name, O_RDWR)) == -1) {
                                    swapfile_fd = open(swap_file_Name,
                                                       O_CREAT | O_RDWR); //if the file doesn't exist create one
                                }

                                write(swapfile_fd, SwapArray, swap_File_Size); //intialize N zeros to the swipe file

                                for (int i = 0; i < num_of_pages; i++) {

                                    if (page_table[i].frame == frame) {
                                        page_table[i].D = 0;
                                        page_table[i].swap_index = 0;
                                        page_table[i].V = 0;
                                        page_table[i].frame = -1;
                                    }

                                }

                            }
                        }

                    }

                } else {
                    frame = Q.front();
                    Q.push(Q.front());
                    Q.pop(); //pop from the first index

                }

                if (PageNumber < pages_of_data + pages_of_text) { //it means that it's type is data

                    int skip = PageNumber * page_size;

                    char buffer[5];

                    lseek(program_fd, skip, SEEK_SET);
                    read(program_fd, buffer, page_size);

                    int Page_physicalAddress = page_size * frame; //physical address for the beginning of a page
                    int physicalAddress = page_size * frame + offsetNumber;

                    for (int i = 0; i < page_size; i++) {
                        main_memory[Page_physicalAddress + i] = buffer[i];
                    }

                    page_table[PageNumber].V = 1;
                    Q_Checker.push(1); //incrementing the size of Q_checker Queue
                    page_table[PageNumber].frame = frame;

                    return main_memory[physicalAddress];

                } else { //type bss, heap ,stack

                    int pages_of_text = text_size / page_size;
                    int pages_of_data = data_size / page_size;
                    int pages_of_bss = bss_size / page_size;

                    if (pages_of_data + pages_of_text + pages_of_bss < PageNumber) { //it means that it is from the type stack or heap

                        cout << "Error: trying to load a file from type heap or stack!\n" << endl;
                        return '\0';

                    } else {
                        int Page_physicalAddress = page_size * frame; //physical address for the beginning of a page
                        int physicalAddress = page_size * frame + offsetNumber; //physical address for the addressed char

                        for (int i = 0; i < page_size; i++) {
                            main_memory[Page_physicalAddress + i] = '0';

                        }

                        page_table[PageNumber].V = 1; //the place becomes valid
                        Q_Checker.push(1); //incrementing the size of Q_checker Queue
                        page_table[PageNumber].frame = frame;
                        return main_memory[physicalAddress]; // return

                    }
                }

            }

        }

    }

}
//*************************************************************STORE*****************************************************************************

void sim_mem::store(int address, char value) { //method that is for storing a specific character at a specific address in the main memory

    int pages_of_text = text_size / page_size;
    int pages_of_data = data_size / page_size;

    int offsetNumber = address % page_size;

    int PageNumber = address / page_size;

    if (page_table[PageNumber].V == 1) { //if valid in the main memory

        int offsetNumber = address % page_size;

        unsigned int frame;
        frame = page_table[PageNumber].frame;

        int physicalAddress = page_size * frame + offsetNumber; //physical address for the addressed char

        main_memory[physicalAddress] = value;

    } else {

        if (page_table[PageNumber].D == 0 && pages_of_data + pages_of_text > PageNumber) { //if the page's dirty was = 0 and it wasn't a type of data,bss,or heap...

            int offsetNumber = address % page_size;

            int PageNumber = address / page_size;
            int skip = page_size * PageNumber;

            char buffer[5];
            unsigned int frame;

            lseek(program_fd, skip, SEEK_SET);
            read(program_fd, buffer, page_size);

            if ((int) Q_Checker.size() == (MEMORY_SIZE / page_size)) { //full memory

                frame = Q.front();
                Q.push(Q.front());
                Q.pop(); //pop from the first index
                Q_Checker.pop(); //decrementing the size of Q_checker Queue

                //move to swap file ...

                int j = 0;
                int n = 0;
                int k;
                while (j != length) {
                    if (SwapArray_checker[j] == -1) { //first empty place
                        SwapArray_checker[j] = frame; //the place is reserved for the frame
                        for (k = j * page_size; k < (j * page_size + 5); k++) {
                            SwapArray[k] = main_memory[(frame * page_size) + n];
                            n++;

                        }

                        int swap_File_Size =
                                page_size * (num_of_pages - (text_size / page_size)); //size of the swap file

                        close(swapfile_fd);
                        remove(swap_file_Name);

                        if ((swapfile_fd = open(swap_file_Name, O_RDWR)) == -1) {
                            swapfile_fd = open(swap_file_Name, O_CREAT | O_RDWR); //if the file doesn't exist create one
                        }

                        write(swapfile_fd, SwapArray, swap_File_Size); //intialize N zeros to the swipe file

                        for (int i = 0; i < num_of_pages; i++) {
                            if (page_table[i].frame == frame) {
                                if (page_table[i].P != 0) {
                                    page_table[i].D = 1;
                                    page_table[i].frame = -1;
                                    page_table[i].swap_index = j * page_size;
                                    page_table[i].V = 0;
                                }

                            }
                        }

                        break;

                    }
                    j++;
                }
                for (int i = 0; i < num_of_pages; i++) {

                    if (page_table[i].frame == frame) {

                        if (page_table[i].P == 0) {
                            SwapArray_checker[j] = -1;

                            for (k = j * page_size; k < (j * page_size + 5); k++) {
                                SwapArray[k] = '0';

                                n++;

                            }

                            int swap_File_Size = page_size * (num_of_pages - (text_size / page_size)); //size of the swap file

                            close(swapfile_fd);
                            remove(swap_file_Name);

                            if ((swapfile_fd = open(swap_file_Name, O_RDWR)) == -1) {
                                swapfile_fd = open(swap_file_Name,
                                                   O_CREAT | O_RDWR); //if the file doesn't exist create one
                            }

                            write(swapfile_fd, SwapArray, swap_File_Size); //intialize N zeros to the swipe file

                            for (int i = 0; i < num_of_pages; i++) {

                                if (page_table[i].frame == frame) {
                                    page_table[i].D = 0;
                                    page_table[i].swap_index = 0;
                                    page_table[i].V = 0;
                                    page_table[i].frame = -1;
                                }

                            }

                        }
                    }

                }

            }
                //the page has been sent to swap
            else {
                frame = Q.front();
                Q.push(Q.front());
                Q.pop(); //pop from the first index

            }

            int Page_physicalAddress = page_size * frame; //physical address for the beginning of a page
            int physicalAddress = page_size * frame + offsetNumber; //physical address for the addressed char

            for (int i = 0; i < page_size; i++) {

                main_memory[Page_physicalAddress + i] = buffer[i];

            }

            main_memory[physicalAddress] = value;

            page_table[PageNumber].V = 1; //the place becomes valid
            Q_Checker.push(1); //incrementing the size of Q_checker Queue
            page_table[PageNumber].frame = frame;
            page_table[PageNumber].D = 1;

        } else if (pages_of_data + pages_of_text < PageNumber && (page_table[PageNumber].D == 0)) { //bss heap or stack

            unsigned int frame;

            if ((int) Q_Checker.size() == (MEMORY_SIZE / page_size)) { //full memory

                frame = Q.front();
                Q.push(Q.front());
                Q.pop(); //pop from the first index
                Q_Checker.pop(); //decrementing the size of Q_checker Queue

                //move to swap file ...

                int j = 0;
                int n = 0;
                int k;
                while (j != length) {
                    if (SwapArray_checker[j] == -1) { //first empty place
                        SwapArray_checker[j] = frame; //the place is reserved for the frame
                        for (k = j * page_size; k < (j * page_size + 5); k++) {

                            SwapArray[k] = main_memory[(frame * page_size) + n];
                            n++;
                        }

                        int swap_File_Size =
                                page_size * (num_of_pages - (text_size / page_size)); //size of the swap file

                        close(swapfile_fd);
                        remove(swap_file_Name);

                        if ((swapfile_fd = open(swap_file_Name, O_RDWR)) == -1) {
                            swapfile_fd = open(swap_file_Name,
                                               O_CREAT | O_RDWR); //if the file doesn't exist create one
                        }
                        write(swapfile_fd, SwapArray, swap_File_Size); //intialize N zeros to the swipe file

                        for (int i = 0; i < num_of_pages; i++) {
                            if (page_table[i].frame == frame) {
                                if (page_table[i].P != 0) {
                                    page_table[i].D = 1;
                                    page_table[i].frame = -1;
                                    page_table[i].swap_index = j * page_size;
                                    page_table[i].V = 0;
                                }

                            }
                        }
                        break;

                    }
                    j++;
                }

                for (int i = 0; i < num_of_pages; i++) {

                    if (page_table[i].frame == frame) {

                        if (page_table[i].P == 0) {
                            SwapArray_checker[j] = -1;

                            for (k = j * page_size; k < (j * page_size + 5); k++) {
                                SwapArray[k] = '0';

                                n++;

                            }

                            int swap_File_Size =
                                    page_size * (num_of_pages - (text_size / page_size)); //size of the swap file

                            close(swapfile_fd);
                            remove(swap_file_Name);

                            if ((swapfile_fd = open(swap_file_Name, O_RDWR)) == -1) {
                                swapfile_fd = open(swap_file_Name,
                                                   O_CREAT | O_RDWR); //if the file doesn't exist create one
                            }

                            write(swapfile_fd, SwapArray, swap_File_Size); //intialize N zeros to the swipe file

                            for (int i = 0; i < num_of_pages; i++) {

                                if (page_table[i].frame == frame) {
                                    page_table[i].D = 0;
                                    page_table[i].swap_index = 0;
                                    page_table[i].V = 0;
                                    page_table[i].frame = -1;
                                }

                            }

                        }
                    }

                }

            } else {
                frame = Q.front();
                Q.push(Q.front());
                Q.pop(); //pop from the first index

            }

            int Page_physicalAddress = page_size * frame; //physical address for the beginning of a page
            int physicalAddress = page_size * frame + offsetNumber; //physical address for the addressed char

            for (int i = 0; i < page_size; i++) {
                main_memory[Page_physicalAddress + i] = '0';

            }

            main_memory[physicalAddress] = value;
            page_table[PageNumber].V = 1; //the place becomes valid
            Q_Checker.push(1); //incrementing the size of Q_checker Queue
            page_table[PageNumber].frame = frame;
            page_table[PageNumber].D = 1;

        } else {

            int offsetNumber = address % page_size;

            int PageNumber = address / page_size;
            int skip = page_table[PageNumber].swap_index;

            char buffer[5];
            unsigned int frame;

            lseek(swapfile_fd, skip, SEEK_SET);
            read(swapfile_fd, buffer, page_size);

            if ((int) Q_Checker.size() == (MEMORY_SIZE / page_size)) { //full memory

                frame = Q.front();
                Q.push(Q.front());
                Q.pop(); //pop from the first index
                Q_Checker.pop(); //decrementing the size of Q_checker Queue

                //move to swap file ...

                int j = 0;
                int n = 0;
                int k;
                while (j != length) {
                    if (SwapArray_checker[j] == -1) { //first empty place
                        SwapArray_checker[j] = frame; //the place is reserved for the frame
                        for (k = j * page_size; k < (j * page_size + 5); k++) {
                            SwapArray[k] = main_memory[(frame * page_size) + n];
                            n++;

                        }

                        int swap_File_Size =
                                page_size * (num_of_pages - (text_size / page_size)); //size of the swap file

                        close(swapfile_fd);
                        remove(swap_file_Name);

                        if ((swapfile_fd = open(swap_file_Name, O_RDWR)) == -1) {
                            swapfile_fd = open(swap_file_Name, O_CREAT | O_RDWR); //if the file doesn't exist create one
                        }

                        write(swapfile_fd, SwapArray, swap_File_Size); //intialize N zeros to the swipe file

                        for (int i = 0; i < num_of_pages; i++) {
                            if (page_table[i].frame == frame) {
                                if (page_table[i].P != 0) {
                                    page_table[i].D = 1;
                                    page_table[i].frame = -1;
                                    page_table[i].swap_index = j * page_size;
                                    page_table[i].V = 0;
                                }

                            }
                        }

                        break;

                    }
                    j++;
                }
                for (int i = 0; i < num_of_pages; i++) {

                    if (page_table[i].frame == frame) {

                        if (page_table[i].P == 0) {
                            SwapArray_checker[j] = -1;

                            for (k = j * page_size; k < (j * page_size + 5); k++) {
                                SwapArray[k] = '0';

                                n++;

                            }

                            int swap_File_Size =
                                    page_size * (num_of_pages - (text_size / page_size)); //size of the swap file

                            close(swapfile_fd);
                            remove(swap_file_Name);

                            if ((swapfile_fd = open(swap_file_Name, O_RDWR)) == -1) {
                                swapfile_fd = open(swap_file_Name,
                                                   O_CREAT | O_RDWR); //if the file doesn't exist create one
                            }

                            write(swapfile_fd, SwapArray, swap_File_Size); //intialize N zeros to the swipe file

                            for (int i = 0; i < num_of_pages; i++) {

                                if (page_table[i].frame == frame) {
                                    page_table[i].D = 0;
                                    page_table[i].swap_index = 0;
                                    page_table[i].V = 0;
                                    page_table[i].frame = -1;
                                }

                            }

                        }

                    }

                }

            }
                //the page has been sent to swap
            else {
                frame = Q.front();
                Q.push(Q.front());
                Q.pop(); //pop from the first index

            }

            for (int i = page_table[PageNumber].swap_index; i < page_table[PageNumber].swap_index + 5; i++) {

                SwapArray[i] = '0';

            }
            SwapArray_checker[page_table[PageNumber].swap_index / page_size] = -1;
            int swap_File_Size =
                    page_size * (num_of_pages - (text_size / page_size)); //size of the swap file

            close(swapfile_fd);
            remove(swap_file_Name);

            if ((swapfile_fd = open(swap_file_Name, O_RDWR)) == -1) {
                swapfile_fd = open(swap_file_Name,
                                   O_CREAT | O_RDWR); //if the file doesn't exist create one
            }

            write(swapfile_fd, SwapArray, swap_File_Size); //intialize N zeros to the swipe file

            int Page_physicalAddress = page_size * frame; //physical address for the beginning of a page
            int physicalAddress = page_size * frame + offsetNumber; //physical address for the addressed char

            for (int i = 0; i < page_size; i++) {
                main_memory[Page_physicalAddress + i] = buffer[i];

            }

            main_memory[physicalAddress] = value;
            page_table[PageNumber].V = 1; //the place becomes valid
            Q_Checker.push(1); //incrementing the size of Q_checker Queue
            page_table[PageNumber].frame = frame;
            page_table[PageNumber].D = 0;

        }

    }

}

sim_mem::~sim_mem() {

    delete(page_table);
    close(program_fd);
    close(swapfile_fd);
    delete(SwapArray);
    delete(SwapArray_checker);



}

/**********/
void sim_mem::print_memory() {
    int i;
    printf("\n Physical memory\n");
    for (i = 0; i < MEMORY_SIZE; i++) {
        printf("[%c]\n", main_memory[i]);
    }

}
/**********/
void sim_mem::print_swap() {
    char * str = (char * ) malloc(this -> page_size * sizeof(char));
    int i;
    printf("\n Swap memory\n");
    lseek(swapfile_fd, 0, SEEK_SET); // go to the start of the file
    while (read(swapfile_fd, str, this -> page_size) == this -> page_size) {
        for (i = 0; i < page_size; i++) {
            printf("%d - [%c]\t", i, str[i]);
        }
        printf("\n");

    }
}
/***********/
void sim_mem::print_page_table() {
    int i;
    printf("\n page table \n");
    printf("Valid\t Dirty\t Permission \t Frame\n");
    for (i = 0; i < num_of_pages; i++) {
        printf("[%d]\t[%d]\t[%d]\t[%d]\t\n", page_table[i].V, page_table[i].D, page_table[i].P, page_table[i].frame);
    }

}
