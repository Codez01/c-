#include <assert.h>

#include <iostream>

#include <string.h>

#include <math.h>

#include <fcntl.h>

#include <string>

#include <vector>

#include <algorithm>

using namespace std;
//---------------------------------------------------------------

#define DISK_SIZE 256 //disk size

int LastSize; //last size of each file

//----------------------------helping methods--------------------
char decToBinary(int n, char & c) //method for converting an int into a special 1 bit character
{
    // array to store binary number
    int binaryNum[8];

    // counter for binary array
    int i = 0;
    while (n > 0) {
        // storing remainder in binary array
        binaryNum[i] = n % 2;
        n = n / 2;
        i++;
    }

    // printing binary array in reverse order
    for (int j = i - 1; j >= 0; j--) {
        if (binaryNum[j] == 1)
            c = c | 1u << j;
    }
    return c;

}

//------------------------------------FsFile------------------------------------
class FsFile {
    int file_size;
    int block_in_use;
    int index_block;
    int block_size;

public:
    FsFile(int _block_size) {
        file_size = 0;
        block_in_use = 0;
        block_size = _block_size;
        index_block = -1;

    }
    int getfile_size() {
        return file_size;
    }
    void setIndex_block(int x) {
        index_block = x;
    }
    int getIndexBlock() {
        return index_block;
    }
    void setblock_in_use(int x) {
        block_in_use = x;
    }
    int getblock_in_use() {
        return block_in_use;
    }

};

//------------------------------------FileDescriptor------------------------------------
class FileDescriptor {
    string file_name;
    bool inUse;
public:
    FsFile * fs_file;
    FileDescriptor(string FileName, FsFile * fsi) {
        file_name = FileName;
        fs_file = fsi;
        inUse = true;

    }
    string getFileName() {
        return file_name;
    }
    bool getInUse() {
        return inUse;
    }
    void setInUse(bool state) {
        inUse = state;
    }

};

class FsFile2 { //helper class

    int block_in_use; //check if block is in use
    int index_block; //stores the index block number
    int lastBlock; //saves the index of last block
    int block__Size;
    int storedLength; //stores the length of each file
public:
    vector < int > indexes;
    FsFile2(int block_size) {

        block_in_use = 0;

        index_block = -1;
        lastBlock = -1;

        block__Size = block_size;

    }

    void setIndex_block(int x) {
        index_block = x;
    }
    int getIndexBlock() {
        return index_block;
    }
    void setblock_in_use(int x) {
        block_in_use = x;
    }
    int getblock_in_use() {
        return block_in_use;
    }
    void setlastblock(int x) {
        lastBlock = x;
    }
    int getlastBlock() {
        return lastBlock;
    }
    void setstoredlength(int x) {
        storedLength = x;
    }
    int getstoredLength() {
        return storedLength;
    }

};

#define DISK_SIM_FILE "DISK_SIM_FILE.txt"
#define DISK_SIZE 256

//====================================fsDisk============================================
class fsDisk {
public:
    FILE * sim_disk_fd;
    bool is_formated;
    int BitVectorSize;
    int * BitVector;

    vector < FileDescriptor > MainDir; //main dir
    vector < FileDescriptor > OpenFileDescriptors; //main dir
    vector < FsFile2 > files; //files

    int direct_enteris;
    int block_size;
    int maxSize;
    int freeBlocks;
    //--------------------------------------------
    fsDisk() { //initialize a disk
        sim_disk_fd = fopen(DISK_SIM_FILE, "r+");
        assert(sim_disk_fd);
        for (int i = 0; i < DISK_SIZE; i++) {
            int ret_val = fseek(sim_disk_fd, i, SEEK_SET);
            ret_val = fwrite("\0", 1, 1, sim_disk_fd);
            assert(ret_val == 1);

        }
        fflush(sim_disk_fd);
        direct_enteris = 0;
        block_size = 0;
        is_formated = false;
    }

    //                 --------list ALL-----------
    void listAll() { //method for listing files and disk content

        for (int i = 0; i < MainDir.size(); i++) {

            cout << " index" << i << ": FileName: " << MainDir[i].getFileName() << " , isInUse: " << MainDir[i].getInUse() << endl;

        }
        char bufy;
        cout << "Disk content: ";
        for (int i = 0; i < DISK_SIZE; i++) {
            int ret_val = fseek(sim_disk_fd, i, SEEK_SET);
            ret_val = fread( & bufy, 1, 1, sim_disk_fd);
            cout << bufy;

        }
        cout << "'" << endl;
    }
    //                  ---------------------------

    //                 ----------FsFormat---------

    void fsFormat(int blockSize) { //method for formating a disk
        if (is_formated == true) {
            MainDir.clear(); //empty the main directory vector
            OpenFileDescriptors.clear(); //empty open file descriptors vector
            files.clear(); //empty the files vector
            delete[] BitVector;

        }

        BitVectorSize = DISK_SIZE / blockSize;
        BitVector = new int[BitVectorSize]; //intialize an a Bit vector array
        for (int i = 0; i < BitVectorSize; i++) {
            BitVector[i] = 0; //intialize bitvector array with zeros
        }
        block_size = blockSize;
        maxSize = blockSize * blockSize;
        freeBlocks = BitVectorSize;

        for (int i = 0; i < DISK_SIZE; i++) { //reformating disk itself
            int ret_val = fseek(sim_disk_fd, i, SEEK_SET);
            ret_val = fwrite("\0", 1, 1, sim_disk_fd);
            assert(ret_val == 1);

        }

        is_formated = true; //it is formated
    }
    //                  ---------------------------

    //                 ----------CreateFile---------

    int CreateFile(string fileName) { //method for creating a new file

        if (!is_formated) { //if the disk is not formated stop..
            return -1;
        }

        for (auto it = begin(MainDir); it != end(MainDir); ++it) {
            if (fileName.compare(it -> getFileName()) == 0) // makes sure that the file doesn't already exist
                {

                return -1;
                }
        }

        FsFile * fs = new FsFile(block_size); //create new file

        FileDescriptor * fd = new FileDescriptor(fileName, fs);
        MainDir.push_back( * fd); //put the  file in the main directory
        files.push_back(block_size);

        for (int i = 0; i < OpenFileDescriptors.size(); i++) {

            if (OpenFileDescriptors[i].fs_file == NULL) {
                OpenFileDescriptors[i] = MainDir[MainDir.size() - 1];
                return i;

            }

        }

        OpenFileDescriptors.push_back( * fd); //put the file in the vector
        return OpenFileDescriptors.size() - 1;

    }
    //                  ---------------------------

    //                 ---------OpenFile----------
    int OpenFile(string fileName) { //method for opening a file
        for (int i = 0; i < MainDir.size(); i++) {

            if (MainDir[i].getFileName() == fileName && MainDir[i].getInUse() == 0) {

                MainDir[i].setInUse(true);
                for (int j = 0; j < OpenFileDescriptors.size(); j++) {
                    if (OpenFileDescriptors[j].fs_file == NULL) {
                        OpenFileDescriptors[j].getFileName() = fileName; //move file name
                        OpenFileDescriptors[j].fs_file = MainDir[i].fs_file; //add file from maindirec to open dir

                        return j;

                    }

                }
                OpenFileDescriptors.push_back(MainDir[i]);

                return OpenFileDescriptors.size() - 1;

            }
            if (MainDir[i].getFileName() == fileName && MainDir[i].getInUse() == 1) { //if the file was already opened

                return -1;
            }

            if (i == MainDir.size() - 1) { //if the file wasn't found

                return -1;

            }

        }

        return -1;

    }
    //                  ---------------------------

    //                 ---------CloseFile----------

    string CloseFile(int fd) { //method for closing a file

        if (!is_formated) { //check the format
            return "-1";
        }

        if (fd > MainDir.size() - 1) { //if the file wasn't found

            return "-1";
        }

        if (MainDir[fd].getInUse() == 1) {

            MainDir[fd].setInUse(false);
            for (int j = 0; j < OpenFileDescriptors.size(); j++) {
                if (OpenFileDescriptors[j].getFileName() == MainDir[fd].getFileName()) {

                    OpenFileDescriptors.erase(OpenFileDescriptors.begin() + j); //deletes the element from the vector

                    return MainDir[fd].getFileName();

                }

            }

        }

        if (MainDir[fd].getInUse() == 0) { //if the file was already closed

            return MainDir[fd].getFileName();
        }

        return "-1";
    }
    //                  ---------------------------

    vector < int > removeDuplicates(vector < int > v, int fd) { //method for removing duplicates in a vector

        sort(v.begin(), v.end()); //sorting vector

        vector < int > v1;
        files[fd].setIndex_block(v[0]); //set the number of the index file

        for (int i = 1; i < v.size() - 1; i++)
            if (v[i] != v[i + 1])
                v1.push_back(v[i]);
            if (v[v.size() - 1] != v1[v1.size() - 1])
                v1.push_back(v[v.size() - 1]);
            return v1;
    }

    //                 ---------WriteToFile----------
    int WriteToFile(int fd, char * buf, int len) { //method for writing on the disk
        if (!is_formated) { //check for format
            return -1;
        }
        vector < int > indexes;
        char _char;

        int block_num = DISK_SIZE / block_size;
        if (len > block_size * block_size) {

            return -1;

        } else {

            int index_free_block;
            if (find_free_index() == -1) {
                return -1;
            } else {
                index_free_block = find_free_index() + 1;

            }

            if (files[fd].getblock_in_use() == 0) { //not located in the disk

                files[fd].setstoredlength(len); // stores the length of the string

                int j = 0;
                int i = 0;

                files[fd].setIndex_block(index_free_block - 1);

                files[fd].setblock_in_use(1); //it means that is located on the disk

                indexes.push_back(index_free_block - 1);
                files[fd].indexes.push_back(index_free_block - 1);

                while (1) {

                    if (j == len) {
                        for (int i = 0; i < indexes.size(); i++) {

                            BitVector[indexes[i]] = 1;

                        }
                        files[fd].setblock_in_use(len); //give the file a length

                        files[fd].setlastblock(index_free_block - 1); //store the last block written

                        files[fd].setIndex_block(files[fd].indexes[0]); //set index block to the first index

                        if ((files[fd].indexes).size() == 2) {

                            fseek(sim_disk_fd, files[fd].getIndexBlock() * block_size, SEEK_SET);

                            _char = decToBinary((files[fd].indexes[1]), _char);

                            fwrite( & _char, 1, 1, sim_disk_fd);
                            return len;
                        }

                        for (int i = 0; i < removeDuplicates(files[fd].indexes, fd).size(); i++) {

                            fseek(sim_disk_fd, files[fd].getIndexBlock() * block_size + i, SEEK_SET);

                            _char = decToBinary(removeDuplicates(files[fd].indexes, fd)[i], _char);
                            fwrite( & _char, 1, 1, sim_disk_fd);

                        }

                        return len;
                    }
                    if (BitVector[index_free_block] == 1) {

                        index_free_block++;

                    } else {
                        indexes.push_back(index_free_block);
                        files[fd].indexes.push_back(index_free_block);

                        fseek(sim_disk_fd, index_free_block * block_size, SEEK_SET);
                        for (int i = 1; i < block_size + 1; i++) {
                            if (j == len) {

                                break;

                            }

                            fwrite( & buf[j], 1, 1, sim_disk_fd);

                            fseek(sim_disk_fd, index_free_block * block_size + i, SEEK_SET);

                            j++;

                        }

                        index_free_block++;

                    }
                }

            } else { //already located in the disk

                int NewLength = len + files[fd].getstoredLength();

                if (NewLength > block_size * block_size) { //check if the new string combined with the stored is bigger than the max

                    return -1;

                }

                //*******************************

                if (files[fd].getstoredLength() % block_size != 0) { //if there is a remainder it means that there is a space left in the last block

                    int j = files[fd].getstoredLength();
                    int i = 0;
                    int k = 0;
                    int l = 0;
                    bool full_block = false;
                    bool complete = false;
                    int start = files[fd].getlastBlock() * block_size + (files[fd].getstoredLength() % block_size); //get the starting index where the string should continue storing

                    index_free_block = files[fd].getlastBlock();

                    while (1) {

                        if (j >= files[fd].getstoredLength() + len) {

                            for (int i = 0; i < indexes.size(); i++) {

                                BitVector[indexes[i]] = 1;

                                files[fd].indexes.push_back(indexes[i]);
                                for (int i = 0; i < files[fd].indexes.size(); i++) {

                                    // cout << "fd[" << i << "] =" << files[fd].indexes[i] << endl;

                                }

                                files[fd].setIndex_block(files[fd].indexes[0]); //set index block to the first index

                            }

                            files[fd].setblock_in_use(len); //give the file a length
                            files[fd].setstoredlength(len + files[fd].getstoredLength());

                            files[fd].setlastblock(index_free_block - 1); //store the last block written

                            if ((files[fd].indexes).size() == 2) {

                                fseek(sim_disk_fd, files[fd].getIndexBlock() * block_size, SEEK_SET);

                                _char = decToBinary((files[fd].indexes[1]), _char);

                                fwrite( & _char, 1, 1, sim_disk_fd);
                                return len;
                            }

                            for (int i = 0; i < removeDuplicates(files[fd].indexes, fd).size(); i++) {

                                fseek(sim_disk_fd, files[fd].getIndexBlock() * block_size + i, SEEK_SET);

                                _char = decToBinary(removeDuplicates(files[fd].indexes, fd)[i], _char);
                                fwrite( & _char, 1, 1, sim_disk_fd);

                            }
                            return len;
                        }

                        if (index_free_block != files[fd].getlastBlock() && BitVector[index_free_block] == 1) {

                            index_free_block++;

                        } else {

                            k = 0;

                            if (full_block == true) {
                                start = index_free_block * block_size;

                            }

                            fseek(sim_disk_fd, start, SEEK_SET);

                            indexes.push_back(index_free_block);

                            for (int i = 1; i < block_size + 1; i++) {

                                if (complete == false) {
                                    if (k + (files[fd].getstoredLength() % block_size) == block_size) {
                                        k = 0;
                                        full_block = true;
                                        complete = true;

                                        break;
                                    }
                                }

                                if (complete == true) {
                                    if (k == block_size) {
                                        k = 0;
                                        break;
                                    }
                                }
                                if (j == len + files[fd].getstoredLength()) {

                                    break;

                                }

                                fwrite( & buf[l], 1, 1, sim_disk_fd);

                                fseek(sim_disk_fd, start + i, SEEK_SET);

                                j++;
                                k++;
                                l++;
                            }

                            index_free_block++;

                        }
                    }

                } else { //if the blocks are full , then move the string to other free blocks

                    int j = files[fd].getstoredLength();
                    int i = 0;
                    int k = 0;
                    int l = 0;

                    while (1) {

                        if (j >= files[fd].getstoredLength() + len) {

                            for (int i = 0; i < indexes.size(); i++) {

                                BitVector[indexes[i]] = 1;
                                files[fd].indexes.push_back(indexes[i]);

                            }

                            files[fd].setblock_in_use(len); //give the file a length
                            files[fd].setstoredlength(len + files[fd].getstoredLength());

                            files[fd].setlastblock(index_free_block - 1); //store the last block written

                            files[fd].setIndex_block(files[fd].indexes[0]); //set index block to the first index

                            if ((files[fd].indexes).size() == 2) {

                                fseek(sim_disk_fd, files[fd].getIndexBlock() * block_size, SEEK_SET);

                                _char = decToBinary((files[fd].indexes[1]), _char);

                                fwrite( & _char, 1, 1, sim_disk_fd);
                                return len;
                            }

                            for (int i = 0; i < removeDuplicates(files[fd].indexes, fd).size(); i++) {

                                fseek(sim_disk_fd, files[fd].getIndexBlock() * block_size + i, SEEK_SET);

                                _char = decToBinary(removeDuplicates(files[fd].indexes, fd)[i], _char);
                                fwrite( & _char, 1, 1, sim_disk_fd);

                            }
                            return len;
                        }

                        if (BitVector[index_free_block] == 1) {

                            index_free_block++;

                        } else {

                            k = 0;

                            fseek(sim_disk_fd, index_free_block * block_size, SEEK_SET);

                            indexes.push_back(index_free_block);

                            for (int i = 1; i < block_size + 1; i++) {

                                if (k == block_size) {
                                    k = 0;
                                    break;

                                }
                                if (j == len + files[fd].getstoredLength()) {

                                    break;

                                }

                                fwrite( & buf[l], 1, 1, sim_disk_fd);

                                fseek(sim_disk_fd, (index_free_block * block_size) + i, SEEK_SET);

                                j++;
                                k++;
                                l++;
                            }

                            index_free_block++;

                        }
                    }

                }

            }

        }
    }
    //                  ---------------------------

    //                 ---------DelFile----------
    int DelFile(string FileName) { //method for deleting an existing file

        int fd;

        if (!is_formated) {
            return -1;
        }

        for (int i = 0; i < MainDir.size(); i++) {
            if (MainDir[i].getFileName() == FileName) {
                fd = i;
                break;

            }
            if (i == MainDir.size() - 1) { //not founded
                return -1;
            }

        }
        if(MainDir[fd].getInUse() == 1){
            return -1;
        }

        int start = files[fd].getlastBlock() * block_size + (files[fd].getstoredLength() % block_size); //location of last stored word in the file

        int j = 0;
        int k = 0;
        int l = 0;
        int i = 0;

        files[fd].setstoredlength(0);

        while (1) {

            if (j == start) {

                return fd;
            }

            if (l / block_size == 1) {

                k++;
                l = 0;
            }

            fseek(sim_disk_fd, removeDuplicates(files[fd].indexes, fd)[k] * block_size + l, SEEK_SET);
            fwrite("\0", 1, 1, sim_disk_fd);

            BitVector[files[fd].indexes[k]] = 0; //free the block

            j++;
            l++;
            i++;
        }


    }

    int find_free_index() { //method for finding free block
        for (int i = 0; i < BitVectorSize; i++) {
            if (BitVector[i] == 0) {
                return i;

            }
        }
        return -1;

    }

    //                  ---------------------------

    //                 ---------ReadFromFile----------
    int ReadFromFile(int fd, char * buf, int len) { //method for reading from file
        if (!is_formated) {
            return -1;
        }
        if (fd > files.size()) {
            return -1;

        }
        for (int i = 0; i < LastSize; i++) {
            buf[i] = ' ';
        }
        string temp;
        if(MainDir[fd].getInUse() == 0){
            return -1;
        }

        int start = files[fd].getlastBlock() * block_size + (files[fd].getstoredLength() % block_size); //location of last stored word in the file

        int j = 0;
        int k = 0;
        int l = 0;
        int i = 0;

        while (1) {
            if (j == start) {
                LastSize = len; //save last length

                return fd;
            }

            if (j == len) {
                LastSize = len; //save last length

                return fd;
            }

            if (l / block_size == 1) {

                k++;
                l = 0;
            }

            char bufy;

            int ret_val = fseek(sim_disk_fd, removeDuplicates(files[fd].indexes, fd)[k] * block_size + l, SEEK_SET);
            ret_val = fread( & bufy, 1, 1, sim_disk_fd);

            assert(ret_val == 1);

            temp.append(1, bufy);

            buf[i] = temp[i];

            j++;
            l++;
            i++;
        }

    }

};
//-----------------------------end of 8 methods--------------------------------------------

//----------------------------------MAIN---------------------------------------------------

int main() {
    int blockSize;
    int direct_entries;
    string fileName;
    char str_to_write[DISK_SIZE];
    char str_to_read[DISK_SIZE];
    int size_to_read;
    int _fd;
    int writed;

    fsDisk * fs = new fsDisk();
    int cmd_;
    while (1) {
        cin >> cmd_;
        switch (cmd_) {
            case 0: // exit
            delete fs;
            exit(0);
            break;

            case 1: // list-file
            fs -> listAll();
            break;

            case 2: // format
            cin >> blockSize;
            fs -> fsFormat(blockSize);
            break;

            case 3: // creat-file
            cin >> fileName;
            _fd = fs -> CreateFile(fileName);
            cout << "CreateFile: " << fileName << " with File Descriptor #: " << _fd << endl;
            break;

            case 4: // open-file
            cin >> fileName;
            _fd = fs -> OpenFile(fileName);
            cout << "OpenFile: " << fileName << " with File Descriptor #: " << _fd << endl;
            break;

            case 5: // close-file
            cin >> _fd;
            fileName = fs -> CloseFile(_fd);
            cout << "CloseFile: " << fileName << " with File Descriptor #: " << _fd << endl;
            break;

            case 6: // write-file
            cin >> _fd;
            cin >> str_to_write;
            writed = fs -> WriteToFile(_fd, str_to_write, strlen(str_to_write));
            cout<< "Writed: " << writed << " Char's into File Descriptor #: "<<_fd <<endl;

            break;

            case 7: // read-file
            cin >> _fd;
            cin >> size_to_read;
            fs -> ReadFromFile(_fd, str_to_read, size_to_read);
            cout << "ReadFromFile: " << str_to_read << endl;
            break;

            case 8: // delete file
            cin >> fileName;
            _fd = fs -> DelFile(fileName);
            cout << "DeletedFile: " << fileName << " with File Descriptor #: " << _fd << endl;
            break;
            default:
                break;
        }
    }

}
//--------------END--------END---------END---------END---------END--------END-------END------END--------END--------END------END--------END----------END-----------END-------------END-----------END-----------END-------------------------------------------------------
