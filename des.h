#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <bitset> 
#include <string>
#include <iostream>

using namespace std;

bitset<32> run_fiestel(bitset<32> in_bits, bitset<48> key_round);

std::bitset<48> expand(std::bitset<32> toExpand);

void xor_with_key(std::bitset<48> *block, std::bitset<48> key_round);

bitset<32> sbox_process(bitset<48> in_bits);
	
bitset<32> final_fiestel_perm(bitset<32> in_bits);

bitset<64> perform_initial_perm(bitset<64> in_bits);
