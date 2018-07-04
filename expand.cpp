#include <iostream>
#include <deque>
#include <unordered_map>
#include <stdio.h>
#include <fstream>
#include <cmath> 
#include <chrono>

// void print_buffer(std::deque<char> buffer) {
// 	std::cout << "BUFFER:" << std::endl;
// 	for (int i = 0; i < buffer.size(); i++) {
// 		std::cout << buffer[i];
// 	}
// 	std::cout << std::endl << std::endl;
// }


int main(int argc, char* argv[]) {
	std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
	std::istream* in;

	std::ifstream inFile;
	std::ofstream outfile;

	// inFile.open("output123", std::ios::in);
	if (argc == 1) {
		//use std::in;
		in = &std::cin;
	}
	else if (argc == 2) {
		std::string filename = argv[1];
		std::cerr << filename << std::endl;
		inFile.open(filename, std::ios::in);
		if (!inFile) {
	        std::cerr << "Unable to open file" << std::endl;
	        exit(1); // terminate with error
	    }
	    in = &inFile;
	}
	else {
		std::cerr << "Too many paramater values, please only input one filename" << std::endl;
		exit(1); 
	}
	

	char read;
	in->get(read);
	int N = read;
	in->get(read);
	int L = read;
	in->get(read);
	int S = read;

	double s = S;
	double n = N;

	int window_size = pow(2.0, n);

	int lookahead_size = pow(2.0, L);
	uint16_t past_window_size = window_size - lookahead_size; 
	int max_literal_length = pow(2.0, s);

	std::deque<char> window(window_size, '\0');

	std::deque<char> past_window(past_window_size, '\0');


	unsigned char current = 0;
	int current_length = 0;

	char output = 0;
	unsigned char leftover = 0;

	int amount_to_read = L;
	int to_fill = 8;

	uint16_t val = 0;
	int shift = 0;

	int iteration = 0;
	while (1) {
		
		//if you dont have enough to read L bits then add bits to obtain 8 and read L of it. After that shift current to the right to get rid of bits you read
		if (current_length < amount_to_read) {
			in->get(read);
			unsigned char c = read;
			
			to_fill = 8 - current_length;
			shift = current_length;

			current |= (c >> shift);
			
			int leftover_from_read = current_length;

			current_length = 8;
			val = current >> (8 - amount_to_read);
			

			current <<= amount_to_read;
			current_length -= amount_to_read;
			

			//still some left over in variable 'read'
			//add those to current, it should never overflow because it is in the if statement
			c <<= to_fill;
			
			
			c >>= current_length;
			
			current |= c;
			current_length += leftover_from_read;
			
		}
		// inFile.get(read);

		//if you already have enough in current, then you don't need to read more
		else {

			val = current >> (8 - amount_to_read);
			

			current <<= amount_to_read;
			current_length -= amount_to_read;
			
		}
		

		//now current needs to_fill more bits
		if (val == 0) { // read string literal -> token triple
			
			amount_to_read = S;
		
			if (current_length < amount_to_read) {
				
				in->get(read);
				unsigned char c = read;
				to_fill = 8 - current_length;
				shift = current_length;
				
				current |= (c >> shift);

				int leftover_from_read = current_length;
				
				current_length = 8;
				val = current >> (8 - amount_to_read);
				

				current <<= amount_to_read;
				current_length -= amount_to_read;
			

				//still some left over in variable 'read'
				//add those to current, it should never overflow because it is in the if statement
				c <<= to_fill;
				
				c >>= current_length;
				

				current |= c;
				current_length += leftover_from_read;
				
			}
			//if you already have enough in current, then you don't need to read more
			else {
				

				val = current >> (8 - amount_to_read);
				current <<= amount_to_read;
				current_length -= amount_to_read;
			}
			//read string len
			
			
			//val represents how many characters to read
            //exit program if val is zero at this point
			if (val == 0) {
				outfile.close();
				std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
         		double elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end -  start).count();
         		std::cerr << "PROGRAM RUN TIME TO EXPAND = " << elapsed/1000 << " MILLISECONDS" << std::endl;
				return 0;

			}
			amount_to_read = 8;
			
            //loop val times, filling current, and outputting it when its filled to obtain the literal chars
			for (int i = 0; i < val; i++) {

				in->get(read);

				unsigned char c = read;
				
			
				shift = current_length;
				
				current |= (c >> shift);
				// output = current;
				std::cout.put(current);

				past_window.pop_front();
				past_window.push_back(current);
				

				current = c << (8 - current_length);

			}

			amount_to_read = L;

		}

		//there was a match so now we read the value of the offset
		else {
			
            //store current in 16 bit int
			uint16_t len = val;

			
            //add 1 since we subtracted when encoding
			len++;
		
			amount_to_read = N;

			uint16_t offset = current;
		
			offset <<= 8;
			int current_offset_length = current_length;
		


			int leftover_length = 0;
			leftover = 0;
            //add 8 bits until you have enought to read offset
			while (current_offset_length < amount_to_read) {
				in->get(read);
				unsigned char c = read;
				

				int to_fill = 16 - current_offset_length;
				if (to_fill < 8) {
					shift = 8 - to_fill;
					offset |= (c >> shift); 
					
					current_offset_length = 16;

					leftover_length = shift;
					leftover = c << to_fill;
				
				}
				else {
					uint16_t holder = c;
				

					
					shift = 8 - current_offset_length;
					holder <<= shift;
					

					offset |= holder;
					current_offset_length += 8;
					
				}
				
			}

			//now you have enough to read N bits
			
			val = offset >> (16 - amount_to_read);
			
			current_offset_length -= amount_to_read;

			offset <<= amount_to_read;
			
			offset >>= 8;

			current = offset;

			current_length = current_offset_length;
			
			int to_fill = 8 - current_length;
			leftover >>= current_length;
			
            //after reading store leftover in current
			current |= (leftover);
			current_length+= leftover_length;
            
            //start at starting index and loop len times, reading values from the past window
			int starting_index = past_window_size - val;
			int index = starting_index;
			unsigned char arr[len];
			int arr_index = 0;
			for (int i = 0; i < len; i++) {
				if (index == past_window_size) {
					index = starting_index;
				}
				output = past_window[index];
				arr[arr_index] = output;
				std::cout.put(output);
				// outfile.flush();
				index++;
				arr_index++;
			}
			// outfile.flush();

			//advance len times
			for (int j = 0; j < len; j++) {

				unsigned char x = arr[j];
				past_window.pop_front();
				past_window.push_back(x);
			}
			amount_to_read = L;

		}

	}
}
