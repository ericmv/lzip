# lzip
Program that compresses and decompresses files using lempel-ziv method

Compile using g++

To Compress: LZ <filename> >> <compressed filename>
  Optional Parameters: -L <integer between 3 and 4>
                       -N <integer between 11 and 14>
                       -S <integer between 3 and 5>
    
L = Max number of bytes used to encode match length (default 4)
N = Max number of bytes used to encode offset (default 11)
S = Max number of bytes used to encode string literal (default 3)

Example: LZ myfile.xls -N 12 -L 4 >> mycompressedfile


Compile expand.cpp using g++
To Decompress: expand <compressed filename> >> <recovered filename>
  
Example: expand mycompressedfile >> myrecoveredfile
