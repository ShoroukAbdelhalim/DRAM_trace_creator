//============================================================================
// Name        : DRAM_memory_trace_creator.cpp
// Author      : Shorouk Abdelhalim
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <cstdlib>
#include <random>
#include <string.h>
#include <fstream>
#include <math.h>

using namespace std;

void usage(void)
{
	printf( "DRAM_memory_trace_creator Usage: \n" );
	printf( "DRAM_memory_trace_creator -w [address_width] -m [address_mapping] -n [num_requests] -p [address_pattern] -t [type_pattern] -o [outputfile_name]\n");
}


int main(int argc, char *argv[]) {

	int address_width = 64;
	int num_requests = 1000;
	int offset_bits = 6;
	int column_bits = 10;
	int row_bits = 15;
	int bank_bits = 3;
	//int rank_bits = 0;
	//int channel_bits = 0;
	char output_file[30] = "output_trace.trc";
	char address_mapping[15] = "RW-RNK-BNK-CL";
	char address_pattern[10] =  "hit";
	char type_pattern[3] = "rd";
	char *arg;
	char type = 'W';

	unsigned long long address = 0;
	unsigned long long column_mask = pow(2,column_bits)-1; // column bits 9:0
	unsigned long long bank_mask = pow(2,bank_bits)-1;	   // bank bits 2:0
	unsigned long long row_mask  = pow(2,row_bits)-1;	   // row bits 14:0
	unsigned long long temp_address = 0;


	if (argc < 10) {
		usage();
		return 1;
	}

	for (int i = 1; i < argc; ++i) {
		arg = argv[i];

		if (!strcmp(arg, "-h")) {
				usage();
				return 0;
		}else if (!strcmp(arg, "-w")){
			i++;
			address_width = atoi(argv[i]);
			if ((address_width != 32) && (address_width != 64)){
				printf("Invalid address_width: it should be 32 or 64\n");
				return 1;
			}
		}else if (!strcmp(arg, "-m")){
			i++;
			strcpy(address_mapping, argv[i]);
			if(strcmp(address_mapping, "RW-RNK-BNK-CL") && strcmp(address_mapping, "RW-CL-RNK-BNK") &&  strcmp(address_mapping, "RNK-BNK-CL-RW")){
				printf("Invalid address_mapping: it should be RW-RNK-BNK-CL or RW-CL-RNK-BNK or RNK-BNK-CL-RW\n");
				return 1;
			}
		}else if (!strcmp(arg, "-n")){
			i++;
			num_requests = atoi(argv[i]);
			if (num_requests <=0 ){
				printf("Invalid num_requests: it should be any positive integer number\n");
				return 1;
			}
		} else if (!strcmp(arg, "-p")){
			i++;
			strcpy(address_pattern, argv[i]);
			if (strcmp(address_pattern, "hit") && strcmp(address_pattern, "conflict") && strcmp(address_pattern,"random") && strcmp(address_pattern, "linear") ){
				printf("Invalid address_pattern: it should be hit [all row hit] or conflict [all row conflicts] or random [random rows] or linear [sequential rows]\n");
				return 1;
			}

		}else if (!strcmp(arg , "-t")){
			i++;
			strcpy(type_pattern, argv[i]);
			if (strcmp(type_pattern, "rd") && strcmp(type_pattern, "wr") && strcmp(type_pattern, "sw")){
				printf("Invalid type_pattern: it should be rd [read requests] or wr [write requests] or sw [switching between reads and writes]\n");
				return 1;
			}
		}else if (!strcmp(arg, "-o")){
			i++;
			strcpy(output_file,  argv[i]);
		}

	}

	//printf("address_width: %d\naddress_mapping:\nnum_requests: %d\naddress_pattern: %s\ntype_pattern: %s\noutfile: %s\n",address_width,address_mapping, num_requests,address_pattern,type_pattern ,output_file);
	//printf("column_mask: %llx, row_mask: %llx, bank_mask: %llx\n", column_mask, row_mask, bank_mask);

	FILE * pFile;
	pFile = fopen (output_file,"w");
	if(pFile == NULL){
		printf("Cannot create a file for trace\n");
		return 1;
	}

		for (int i=0;i<num_requests;i++){
			// create linear addresses and print it in the file
			if (!strcmp(address_pattern,"linear")){
				if (!strcmp(type_pattern,"rd"))
					fprintf(pFile,"0x%.8llx READ 0\n",address);
				else if (!strcmp(type_pattern,"wr"))
					fprintf(pFile,"0x%.8llx WRITE 0\n",address);
				else if (!strcmp(type_pattern,"sw")){
					if (type == 'W'){
						fprintf(pFile,"0x%.8llx READ 0\n",address);
						type = 'R';
					} else {
						fprintf(pFile,"0x%.8llx WRITE 0\n",address);
						type = 'W';
					}

				}
				address += 64;
			}

			// create hit addresses and print it in the file, incrementing column_bits
			else if(!strcmp(address_pattern,"hit")){
				temp_address++;
				address = (temp_address & column_mask);

				if (!strcmp(address_mapping, "RW-RNK-BNK-CL")){
					address <<= offset_bits; // column bits 9:0
				}else if(!strcmp(address_mapping, "RW-CL-RNK-BNK")){
					address <<= (offset_bits+bank_bits); // column bits 9:0
				}else if(!strcmp(address_mapping, "RNK-BNK-CL-RW")){
					address <<= (offset_bits+row_bits); // column bits 9:0
				}

				if (!strcmp(type_pattern,"rd"))
					fprintf(pFile,"0x%llx READ 0\n",address);
				else if (!strcmp(type_pattern,"wr"))
					fprintf(pFile,"0x%llx WRITE 0\n",address);
				else if (!strcmp(type_pattern,"sw")){
					if (type == 'W'){
						fprintf(pFile,"0x%llx READ 0\n",address);
						type = 'R';
					} else {
						fprintf(pFile,"0x%llx WRITE 0\n",address);
						type = 'W';
					}

				}

			}


			// create conflict addresses and print it in the file, increment row_bits
			else if(!strcmp(address_pattern,"conflict")){
				temp_address++;
				address = (temp_address & row_mask);

				if (!strcmp(address_mapping, "RW-RNK-BNK-CL")){
					address <<= (offset_bits+column_bits+bank_bits); // column bits 9:0
				}else if(!strcmp(address_mapping, "RW-CL-RNK-BNK")){
					address <<= (offset_bits+bank_bits+column_bits); // column bits 9:0
				}else if(!strcmp(address_mapping, "RNK-BNK-CL-RW")){
					address <<= (offset_bits); // column bits 9:0
				}

				if (!strcmp(type_pattern,"rd"))
					fprintf(pFile,"0x%llx READ 0\n",address);
				else if (!strcmp(type_pattern,"wr"))
					fprintf(pFile,"0x%llx WRITE 0\n",address);
				else if (!strcmp(type_pattern,"sw")){
					if (type == 'W'){
						fprintf(pFile,"0x%llx READ 0\n",address);
						type = 'R';
					} else {
						fprintf(pFile,"0x%llx WRITE 0\n",address);
						type = 'W';
					}

				}

			}


			// create random addresses and print it in the file
			else if (!strcmp(address_pattern,"random")){

				if (!strcmp(address_mapping, "RW-RNK-BNK-CL")){
					address = (((rand() % 8)&bank_mask)<< (offset_bits+column_bits)) | (((rand() % 1024)&column_mask) << offset_bits) | (((rand() % 32768)&row_mask) << (offset_bits+column_bits+bank_bits));
				}else if(!strcmp(address_mapping, "RW-CL-RNK-BNK")){
					address = (((rand() % 8)&bank_mask)<< (offset_bits)) | (((rand() % 1024)&column_mask) << (offset_bits+bank_bits)) | (((rand() % 32768)&row_mask) << (offset_bits+column_bits+bank_bits));
				}else if(!strcmp(address_mapping, "RNK-BNK-CL-RW")){
					address = (((rand() % 8)&bank_mask)<< (offset_bits+column_bits+row_bits)) | (((rand() % 1024)&column_mask) << (offset_bits+row_bits)) | (((rand() % 32768)&row_mask) << (offset_bits));
				}

				if (!strcmp(type_pattern,"rd"))
					fprintf(pFile,"0x%llx READ 0\n",address);
				else if (!strcmp(type_pattern,"wr"))
					fprintf(pFile,"0x%llx WRITE 0\n",address);
				else if (!strcmp(type_pattern,"sw")){
					if (type == 'W'){
						fprintf(pFile,"0x%llx READ 0\n",address);
						type = 'R';
					} else {
						fprintf(pFile,"0x%llx WRITE 0\n",address);
						type = 'W';
					}
				}
				address = 0;
			}
		}

	printf("Please check the output file\n");


	return 0;
}
