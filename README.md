# DRAM_trace_creator

DRAM_trace_generator is an engine for generating trace file contains main memory addresses (with “DDR3_2Gb_x8” device structure) for testing purposes and comparing between different cases in simulation time, hit/miss rate, bandwidth, and worst-case delay for every requestor, using memory controller simulations, as MCsim: https://github.com/uwuser/MCsim. The trace format matches that required as an input file trace for MCsim.  

<addr> <read/write> <cpu-inst>  

Address fields are: 

0bits offset 		// to match address mapping of the MCsim simulator, it should be 6bits 

10 bits column 

15 bits row 

3 bits bank 

0 bits channel and rank. 

 

Usage 

DRAM_trace_generator generates addresses with different patterns according to the user’s configurations and “DDR3_2Gb_x8” device structure. 

-w  address width in bits 

-m  address mapping format (RW-RNK-BNK-CL, RW-CL-RNK-BNK, RNK-BNK-CL-RW) 

-n  number of requests 

-p  addresses pattern (seq, hit, conflict, random) 

-t  type of requests pattern (rd, wr, sw) 

-o  name of the output file 

 

Building and Running 

 

$ cd /src 

$ g++ DRAM_memory_trace_creator.cpp -o main 

$ ./main -w [address_width] -m [address_mapping] -n [num_requests] -p [address_pattern] -t [type_pattern] -o [outputfile_name] 