#include <iostream>
#include <deque>
#include <unordered_map>
#include <stdio.h>
#include <fstream>
#include <cmath>
#include <chrono>

// void print_buffer(std::deque<char> buffer) {
//     std::cout << "BUFFER:" << std::endl;
//     for (int i = 0; i < buffer.size(); i++) {
//         std::cout << buffer[i];
//     }
//     std::cout << std::endl << std::endl;
// }

//obtain filesize by opening file
double filesize(const char* filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    
    double x = in.tellg();
    in.close();
    return x;
}


//parses a single command line parameter
int parameters(std::string param, int* N, int* L, int* S) {
    int val;
    
    if (param.substr(0,2).compare("-N") == 0) {
        int temp = std::stoi(param.substr(3));
        
        if (temp > 14 || temp < 9) {
            std::cerr << "N must be equal to or between 14 and 9." << std::endl;
            return -1;
        }
        
        else {
            *N = temp;
            return *N;
        }
    }
    
    else if (param.substr(0,2).compare("-L") == 0) {
        int temp = std::stoi(param.substr(3));
        
        if (temp > 4 || temp < 3) {
            std::cerr << "L must be 3 or 4." << std::endl;
            return -1;
        }
        
        else {
            *L= temp;
            return *L;
        }
    }
    
    else if (param.substr(0,2).compare("-S") == 0) {
        int temp = std::stoi(param.substr(3));
        // std::cerr << temp << std::endl;
        if (temp > 5 || temp < 1) {
            std::cerr << "S must be equal to or between 1 and 5." << std::endl;
            return -1;
        }
        
        else {
            *S = temp;
            return *S;
        }
    }
    
    //if not one of the parameters, return -2
    else {
        // std::cerr << "returning -2" << std::endl;
        return -2;
    }
}


//takes a string literal and outputs each character of the string one char at a time
void output_literal(unsigned char* output, int* current_output_length, std::string literal, int L, int S, double max_literal_length, double* compressed_size) {
   
    //get literal length
    unsigned char s = literal.length();
    int output_length = *current_output_length;
    
    //amount to shift new character
    int shift_amount = 8 - output_length - L;
    
    if (shift_amount < 0) {
        //if shift amount is < 0, then there is not enough room to fit L bits, so we shift right to take the amount we can fit
        shift_amount = abs(shift_amount);
        
        //send output
        std::cout.put(*output);
        (*compressed_size)++;
        
        *output = 0;
        output_length = shift_amount;
    }
    else {
        
        //len to encode is going to be 0, so we simply increment the current length since everything after the output_length bit is 0 already
        output_length = output_length + L;
        
        if (output_length == 8) {
            //if there are 8 set bits, output it
            std::cout.put(*output);
            (*compressed_size)++;
            *output = 0;
            output_length = 0;
        }
    }
    
    //same process as above but for the next S bits to encode the string length
    shift_amount = 8 - output_length - S;
    if (shift_amount < 0) {
        shift_amount = abs(shift_amount);
        unsigned char leftover_from_s = s << (8-shift_amount);
        *output |= (s >> shift_amount);
        
        std::cout.put(*output);
        (*compressed_size)++;
        
        *output = leftover_from_s;
        output_length = shift_amount;
    }
    else {
        //shift s to the left to align with current output length of the output and add the bits to the output
        output_length = output_length + S;
        *output |= (s << shift_amount);
        
        if (output_length == 8) {
            //if 8 bits are set, output
            std::cout.put(*output);
            (*compressed_size)++;
            *output = 0;
            output_length = 0;
        }
    }
    
    //for each character, add the amount of bits that can fit in output, send output, and then add remaining bits from c to output
    for (int i = 0; i < literal.length(); i++) {
        unsigned char c = literal[i];
        
        unsigned char add_to_output = c >> (output_length);
        *output |= add_to_output;
        //send output
        std::cout.put(*output);
        (*compressed_size)++;
        *output = c << (8 - output_length);
        
        // current_output_length = current_output_length;
    }
    *current_output_length = output_length;
}

int main(int argc, char *argv[]) {
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    if (argc < 2) {
        std::cout << "Filename Required" << std::endl;
        exit(1);
    }
    int N = 11;
    int L = 4;
    int S = 3;
    
    double size_of_file;
    double compressed_size = 0;
    std::ifstream inFile;
    std::ifstream sizeStream;
    std::ofstream outFile;
    
    
    
    if (argc == 2) {
        char* filename = argv[1];
        std::cerr << filename << std::endl;
        inFile.open(filename, std::ios::in);
        if (!inFile) {
            std::cerr << "Unable to open file";
            exit(1); // terminate with error
        }
        size_of_file = filesize(filename);
    }
    else if (argc > 2) {
        for (int i = 1; i < argc; i++) {
            std::string p = argv[i];
            int val = parameters(p, &N, &L, &S);
            if (val == -1) {
                exit(1);
            }
            else if (val == -2) {
                char* filename = argv[i];
                inFile.open(filename, std::ios::in);
                if (!inFile) {
                    std::cerr << "Unable to open file";
                    exit(1); // terminate with error
                }
                size_of_file = filesize(filename);
            }
        }
    }
    
    
    if (!inFile) {
        std::cerr << "Unable to open file";
        exit(1); // terminate with error
    }
    std::cerr<< "COMPRESSING..." << std::endl;
    // std::cerr << "N = " << N << std::endl;
    // std::cerr << "L = " << L << std::endl;
    // std::cerr << "S = " << S << std::endl;
    
    int eof_count = 0;
    
    
    char s_to_write = S;
    char n_to_write = N;
    char l_to_write = L;
    
    std::cout.put(n_to_write);
    std::cout.put(l_to_write);
    std::cout.put(s_to_write);
    compressed_size +=3;
    double s_pow = S;
    double max_literal_length = pow(2.0, s_pow) - 1;
    
    double n_pow = N;
    double l_pow = L;
    
    int W = pow(2.0, n_pow);
    int F = pow(2.0, l_pow);
    int past_window_size = W - F;
    int window_size = W;
    std::deque<char> window(W, '\0');
    
    char x;
    
    //add first F characters to lookahead buffer
    for (int i = 0; i < F; i++) {
        //need to check if this reaches the end early on
        // std::cout << i << std::endl;
        inFile.get(x);
        window.pop_front();
        window.push_back(x);
        
    }
    
    std::string literal = "";
    // inFile.get(x);
    int iteration = 0;
    
    int current_output_length = 0;
    unsigned char output = 0;
    while (1) {
        
        //start of search
        char new_first = window[past_window_size];
        char next_match = new_first;
        
        int last_starting_point = 0;
        
        int current_window_index = 0;
        int current_lookahead_index = past_window_size;
        
        int highest_match = 0;
        int highest_match_index = 0;
        
        int i = 0;
        // std::cout << "finished loop" << std::endl;
        
        
        //for each element in the past, check if that element equals the first element in the lookahead buffer
        for (int i = 0; i < past_window_size; i++) {
            
            if (window[i] == new_first) {
                //if you found a match, check the next character of both the past and the lookahead until something doesnt match
                int match_size = 1;
                current_window_index = i+1;
                current_lookahead_index = past_window_size+1;
                next_match = window[current_lookahead_index];
                while (window[current_window_index] == next_match) {
                    
                    match_size++;
                    current_window_index++;
                    current_lookahead_index++;
                    
                    //replace with W
                    if (current_lookahead_index >= window_size) {
                        break;
                    }
                    next_match = window[current_lookahead_index];
                    
                }
                
                if (match_size > highest_match) {
                    //take the highest match size of the entire buffer
                    highest_match = match_size;
                    highest_match_index = i;
                }
                
            }
            
        }
        
        if (highest_match >= 2) {
            //if a match was greater than 2, output whatever literal was store up to that point.
            if (literal.length() > 0) {
                //output 0, literal.length(), literal's chars
                output_literal(&output, &current_output_length, literal, L, S, max_literal_length, &compressed_size);
                
                literal="";
            }
            
            //encode the match len by storing it in an 8 bit char
            unsigned char converted_len = highest_match - 1;
            
            
            int shift_amount = 8 - L - current_output_length;
            
            if (shift_amount < 0) {
                //if L bits cannot fit, shift right to obtain the amount of bits that can fit
                shift_amount = abs(shift_amount);
                char leftover_from_len = converted_len << (8-shift_amount);
                converted_len >>= shift_amount;
                
                output |= converted_len;
                std::cout.put(output);
                compressed_size++;
                //send output
                
                output = leftover_from_len;
                current_output_length = shift_amount;
                //send output
            }
            else {
                //else shift left to align with output
                converted_len <<= shift_amount;
                output |= converted_len;
                current_output_length = current_output_length + L;
                
                
                if (current_output_length == 8) {
                    //send output
                    std::cout.put(output);
                    compressed_size++;
                    
                    output = 0;
                    current_output_length = 0;
                }
                
            }
            
            //store offset in 16 bit int
            uint16_t offset = past_window_size - highest_match_index;
            int current_offset_length = 16;
            
            int amount_taken_from_offset = 8 - current_output_length;
            shift_amount = N - amount_taken_from_offset;
            
            unsigned char converted_offset = offset >> (shift_amount);
            output |= converted_offset;
            
            //send output
            
            std::cout.put(output);
            compressed_size++;
            
            output = 0;
            current_output_length = 0;
            
            offset <<= (amount_taken_from_offset + 16 - N);
            current_offset_length = N - amount_taken_from_offset;
            
            //since offset is 16 and we can only output 8 at a time, loop to keep outputting 8 until less than 8 are left
            while (current_offset_length >= 8) {
                
                int extra = current_offset_length - 8;
                shift_amount = (amount_taken_from_offset + 16 - N) + extra;
                amount_taken_from_offset = 8;
                converted_offset = offset >> shift_amount;
                output |= converted_offset;
                
                //send output
                
                
                std::cout.put(output);
                compressed_size++;
                output = 0;
                current_output_length = 0;
                
                offset <<= 8;
                
                current_offset_length -= 8;
                
            }
            //put leftovers in output
            
            converted_offset = offset >> 8;
            output |= converted_offset;
            current_output_length = current_offset_length;
            
            //advance highest_match positions
            for (int i = 0; i < highest_match; i++) {
                //if you can get a new char
                if (inFile.get(x)){
                    window.pop_front();
                    window.push_back(x);
                    
                }
                else  {
                    //you cannot read so you have reached the eof
                    if (inFile.eof()) {
                        window.pop_front();
                        
                        window.push_back(std::char_traits<wchar_t>::eof());
                        eof_count++;
//                        std::cerr << eof_count << std::endl;
                    }
                    
                }
                
            }
            //if you couldnt read F times, then there is nothing left in the lookahead
            if (eof_count == F) {
                //output whatever literal you had
                if (literal.length() > 0) {
                    output_literal(&output, &current_output_length, literal, L, S, max_literal_length, &compressed_size);
                    literal="";
                }
                //output zeros to mark end of file
                std::cout.put(output);
                std::cout.put(0);
                compressed_size+=2;
                double savings = compressed_size/size_of_file;
                std::cerr << "ORIGINAL SIZE: " << size_of_file << " BYTES" <<std::endl;
                std::cerr << "COMPRESSED SIZE: " << compressed_size << " BYTES"<< std::endl;
                std::cerr << "SAVINGS: " << (1 - savings) * 100 << "%" << std::endl;
                
                std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
                double elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end -  start).count();
                std::cerr << "PROGRAM RUN TIME TO COMPRESS = " << elapsed/1000 << " MILLISECONDS" << std::endl;
                // std::cerr.flush();
                
                return 0;
            }
        }
        else {
            //if no match, add the first char in lookahead buffer to literal
            literal += new_first;
            //if max length is reached, output literal
            if (literal.length() >= max_literal_length) {
                output_literal(&output, &current_output_length, literal, L, S, max_literal_length, &compressed_size);
                literal="";
            }
            
            if (inFile.get(x)) {
                window.pop_front();
                window.push_back(x);
            }
            else {
                if (inFile.eof()) {
                    window.pop_front();
                    window.push_back(std::char_traits<wchar_t>::eof());
                    eof_count++;
//                    std::cerr << eof_count << std::endl;
                    
                    
                    if (eof_count == F) {
                        if (literal.length() > 0) {
                            output_literal(&output, &current_output_length, literal, L, S, max_literal_length, &compressed_size);
                            literal="";
                        }
                        
                        std::cout.put(output);
                        std::cout.put(0);
                        compressed_size+=2;
                        
                        double savings = compressed_size/size_of_file;
                        std::cerr << "ORIGINAL SIZE: " << size_of_file << " BYTES" <<std::endl;
                        std::cerr << "COMPRESSED SIZE: " << compressed_size << " BYTES"<< std::endl;
                        std::cerr << "SAVINGS: " << (1 - savings) * 100 << "%" << std::endl;
                        
                        std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
                        double elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end -  start).count();
                        std::cerr << "PROGRAM RUN TIME TO COMPRESS = " << elapsed/1000 << " MILLISECONDS" << std::endl;
                        // std::cerr.flush();
                        return 0;
                    }
                }
                
                
            }
            
        }
        
    }
    
    return 0;
    
}
