Authored by: Hanna

===================Description======================

This program is for embodimenting the real simulation of how a disk works using the plan of index allocation method , this program starts with formating the disk and making sure that the disk is empty to  start to store in it
,then by giving the input of 3 and a name it creates a new file in the disk and adds it to a vector called MainDir which is the main directory  and gives it the state ("opened") which is symbolized by the number 1 so it adds itself to a vector called OpenFileDescriptors which
symoblizes the real file descriptors in our computers , then by giving the input of 6 and the number of the file created and after a string to be written into the file specified  just by checking the Bitvector which tells where the free blocks are lcoated in the disk 
and when the content is written to the disk the Indexes of the blocks turns to occupied in the bitvector , the indexes that are being used in each file are stored in a vector that is located in a class called fsdisk2 which used for storing the indexes used and so on
for each file that we have written to , has a Blocksized blocks before that store each index and since the index block cannot contain a number we use a function called decTobinary that takes the index number and converts it to a single bit character and stores in within the 
index block.. if the input was 5 it means that we wanna close a specific file , and if the input was 4 then it opens a specified file if it was closed , by giving the input of 8 and giving a spcified file name it deletes it and its content from the disk and finally if the input was 1 then it lists the disk content and the files in it.


===================Functions=========================
char decToBinary(int n, char & c) //method for converting an int into a special 1 bit character
void listAll() //method for listing files and disk content
void fsFormat(int blockSize) //method for formating a disk
int CreateFile(string fileName)  //method for creating a new file
int OpenFile(string fileName)  //method for opening a file
string CloseFile(int fd) //method for closing a file
vector < int > removeDuplicates(vector < int > v, int fd) //method for removing duplicates in a vector
int WriteToFile(int fd, char * buf, int len)  //method for writing on the disk
int DelFile(string FileName)  //method for deleting an existing file
int find_free_index()  //method for finding a free block
int ReadFromFile(int fd, char * buf, int len) //method for reading from file



=====================Program Files=========================
1) Ex7_Final.cpp // is the file that contains the whole code of the disk and includes many classes within the disk and main.
2) DISK_SIM_FILE.txt // is the file that embodiments the disk in this program 

============= How To Run ( Compilation) ===================

compile:  g++ -o program main.cpp


run: ./program

================= Output ====================================
2
4
3
A
CreateFile: A with File Descriptor #: 0
3
B
CreateFile: B with File Descriptor #: 1
3
C
CreateFile: C with File Descriptor #: 2
5
2
CloseFile: C with File Descriptor #: 2
6
1
ABCDEFG
Writed: 7 Char's into File Descriptor #: 1
1
 index0: FileName: A , isInUse: 1
 index1: FileName: B , isInUse: 1
 index2: FileName: C , isInUse: 0
Disk content: ABCDEFG'
6
0
QWERQWER
Writed: 8 Char's into File Descriptor #: 0
1
 index0: FileName: A , isInUse: 1
 index1: FileName: B , isInUse: 1
 index2: FileName: C , isInUse: 0
Disk content: ABCDEFGQWERQWER'
7
1
4
ReadFromFile: ABCD
7
0
8
ReadFromFile: QWERQWER
4
C
OpenFile: C with File Descriptor #: 2
6
2
AZXSWEDCFRTG
Writed: 12 Char's into File Descriptor #: 2
1
 index0: FileName: A , isInUse: 1
 index1: FileName: B , isInUse: 1
 index2: FileName: C , isInUse: 1
Disk content: ABCDEFGQWERQWERAZXSWEDCFRTG'

2
12
ReadFromFile: AZXSWEDCFRTG

