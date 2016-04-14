#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <bitset> 
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

void get_file_data(int *num_iters, int *num_rounds, string *input_message, string *key);

bitset<64> revert_initial_perm(bitset<64> enc_data);

bitset<64> run_des(bitset<64> in_bits, int num_rounds);

bitset<32> run_fiestel(bitset<32> in_bits, bitset<48> key_round);

std::bitset<48> expand(std::bitset<32> toExpand);

void xor_with_key(std::bitset<48> *block, std::bitset<48> key_round);

bitset<32> sbox_process(bitset<48> in_bits);
	
bitset<32> final_fiestel_perm(bitset<32> in_bits);

bitset<64> perform_initial_perm(bitset<64> in_bits);

bitset<28> rotate_left_one(bitset<28> in_bits);

bitset<28> rotate_left_two(bitset<28> in_bits);

bitset<56> init_key_permute(bitset<64> in_bits);

void key_shift(bitset<56> in_bits, int num_rounds);

bitset<48> round_key_permute(bitset<56> in_bits);

void generate_all_keys(bitset<64> base_key, int number_of_rounds);
