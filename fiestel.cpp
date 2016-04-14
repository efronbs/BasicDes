#include "des.h"
#include "constants.h"

int main()
{

	//setup
	//read from file, get all necessary data, set up initial bitmaps
	int num_iterations, num_rounds;
	string in_msg, in_key;
	get_file_data(&num_iterations, &num_rounds, &in_msg, &in_key);		

	bitset<64> bit_msg (in_msg);
	bitset<64> bit_key (in_key);

	auto current_encryption = codebook0; //codebook0 is a bitmap of all 0s. Used as IV for CBC

	//generate all round keys store them in all_round_keys array. Declared in constants.h	
	generate_all_keys(bit_key, num_rounds);
	
	//for each iteration xor with prev result and run des on that value
	for (int i = 0; i < num_iterations; i++)
	{	
		auto starting_msg = current_encryption ^ bit_msg; //using base message for all messages
		auto permed_msg = perform_initial_perm(starting_msg);
		
		//run iteration of des
		current_encryption = run_des(permed_msg, num_rounds);
	}
	cout << current_encryption << "\n";
}

/*just grabs the data from desinput.txt */
void get_file_data(int *num_iters, int *num_rounds, string *input_message, string *key)
{
	std::ifstream infile("desinput.txt");
	string line;

	//get iterations
	getline(infile, line); 
	*num_iters = stoi(line);

	//get num rounds
	getline(infile, line);
	*num_rounds = stoi(line);
	
	//get input message
	getline(infile, *input_message);
	
	//get key
	getline(infile, *key);
}

/* DES ALGORITHM */

bitset<64> run_des(bitset<64> in_bits, int num_rounds)
{
	bitset<32> l_msg;
	bitset<32> r_msg;
		
	bitset<32> new_l;
	bitset<32> new_r;
	
	bitset<64> enc_data;	

	//break into initial L and R
	for (int i = 0; i < 64; i++)
	{
		if (i < 32) {
			r_msg[i] = in_bits[i];
		} else {
			l_msg[i - 32] = in_bits[i];
		}
	}
	;
		
	//run the fiestel network for the designated amount of rounds
	for (int i = 0; i < num_rounds; i++)
	{
		new_l = r_msg;
		auto fiestel_result = run_fiestel(r_msg, all_round_keys[i]);
		
		new_r = l_msg ^ fiestel_result;

		l_msg = new_l;
		r_msg = new_r;
	}

	//finally, flip l and r, and reverse the initial permutation		
	for (int i = 0; i < 64; i++)
	{
		if (i < 32) {
			enc_data[i] = l_msg[i];
		} else {
			enc_data[i] = r_msg[i - 32];
		}
	}

	enc_data = revert_initial_perm(enc_data);	
	return enc_data;
}

bitset<64> revert_initial_perm(bitset<64> enc_data)
{
        bitset<64> out;
        for (int i = 0; i < 64; i++)
                out[63 - i] = enc_data[63 - (initial_perm_inverse[i] - 1)];

        return out;
}


/* KEY GENERATION */

//generates all round keys from the original 64 bit
void generate_all_keys(bitset<64> base_key, int number_of_rounds)
{
	auto perm_key = init_key_permute(base_key);
	key_shift(perm_key, number_of_rounds);

}

void key_shift(bitset<56> in_bits, int num_rounds)
{
	bitset<28> c_set;
	bitset<28> d_set;
	bitset<56> out;
	bitset<56> full_shifted_keys[16];	

	//initial break up of original key into C and D sets
	for (int i = 0; i < 56; i++)
	{
		if (i < 28) {
			d_set[i] = in_bits[i];
		} else {
			c_set[i - 28] = in_bits[i];
		}
	}
	
	//key generation loop
	for (int j = 0; j < num_rounds; j++)
	{	
		int shift_type = shift_schedule[j];
		
		//get new C and D sets	
		//double and single rotations are split up for speed. Doing a shift left by 2 is faster than two single shifts
		if (shift_type == 2) {
			c_set = rotate_left_two(c_set);
			d_set = rotate_left_two(d_set);
		} else {
			c_set = rotate_left_one(c_set);
			d_set = rotate_left_one(d_set);			 
		}		

		//put back together to get "full" key after shift. c and d are saved for next roation, so only one split instead of 12
		for (int i = 0; i < 56; i++)
		{
			if (i < 28) {
				out[i] = d_set[i];
			} else {
				out[i] = c_set[i - 28];
			}
		}	
		//permute the 56 bit key into final round ke yform
		all_round_keys[j] = round_key_permute(out);	
	}
	 	
}

//performs the round key permutation
bitset<48> round_key_permute(bitset<56> in_bits)
{
	bitset<48> out;
        for (int i = 0; i < 48; i++)
                out[47 - i] = in_bits[55 - (round_key_perm[i] - 1)];
        return out;

}

//performs the initial key permutation (before generating round keys)
bitset<56> init_key_permute(bitset<64> in_bits)
{
	bitset<56> out;
	for (int i = 0; i < 56; i++)
                out[55 - i] = in_bits[63 - (key_perm[i] - 1)];

	return out;
}

//single rotation. Save leftmost bit, perform shift, put it on right
bitset<28> rotate_left_one(bitset<28> in_bits)
{
	bitset<28> out;
	auto temp = in_bits[27];
	out = in_bits<<1;
	out[0] = temp;
	return out;
}

//double rotation. Save two leftmost bits, perform shift, put them on right
bitset<28> rotate_left_two(bitset<28> in_bits)
{
	bitset<28> out;
	auto temp1 = in_bits[27];
	auto temp2 = in_bits[26];
	out = in_bits<<2;
	out[1] = temp1;
	out[0] = temp2;
	return out;
}

/* FIESTEL FUNCTION */

//perform the initial message permutation
bitset<64> perform_initial_perm(bitset<64> in_bits)
{
        bitset<64> out;
        for (int i = 0; i < 64; i++)
                out[63 - i] = in_bits[63 - (initial_perm[i] - 1)];

        return out;
}

//runs the fiestel function of the input bit string. Expansion -> xor with key -> run through sboxes -> recombine
bitset<32> run_fiestel(bitset<32> in_bits, bitset<48> key_round)
{
	auto expanded = expand(in_bits);	
	//cout << "expansion :" << expanded.to_string() << "\n";
	xor_with_key(&expanded, key_round);
	//cout << "key xor: " << expanded.to_string() << "\n";
	auto sb_out = sbox_process(expanded);
	//cout << "sbox result :" << sb_out.to_string() << "\n";
	return final_fiestel_perm(sb_out);
}

//runs the expansion permutation
std::bitset<48> expand(std::bitset<32> toExpand)
{
	std::bitset<48> out;

	//magic number 48, the size of the permuation
	for (int i = 0; i < 48; i++)
		out[47 - i] = toExpand[31 - (expansion_perm_bits[i] - 1)]; //grab the bit number from that place in the expansion array, sub 1 for correct index
	return out;	
		
}

//just for ease of reading. Just does an xor
void xor_with_key(std::bitset<48> *block, std::bitset<48> key_round)
{
	*block ^= key_round;	
}

//runs the sbox permutation on the expanded message half
bitset<32> sbox_process(bitset<48> in_bits)
{
	string full_str = in_bits.to_string();
	string result_str = "";
	string strset[8];

	//splits expanded msg half into 8 sets of 6 for the sboxes
	for (int i = 0; i < 8; i++) 
	{
		strset[i] = full_str.substr(i * 6, 6);
	}
	
	//puts are 8 sets through their respective sboxes
	for (int i = 0; i < 8; i++)
	{
		//grabs current sbox and bit set based on round
		auto current_box = all_sboxes[i];
		string current_str = strset[i];
		
		//grab the first and last bits to select the row of the current sbox
		string temp = "";
		temp += current_str[0];
		temp += current_str[5];
		bitset<2> selectors (temp);

		//grabs the selected row out of the sbox and grabs 4 column selection bits
		auto current_row = current_box[selectors.to_ulong()];
		bitset<4> s_in (current_str.substr(1, 4));
		
		//uses column selection to grab the sbox output and parses that number into a bitset
		int s_out_raw = current_row[s_in.to_ulong()];
		bitset<4> s_out_bts (s_out_raw);
		
		//concatenation of results
		result_str += s_out_bts.to_string();	
	}	
	
	bitset<32> result_bts(result_str);
	return result_bts; 
}

//after msg is put through the fiestel function, performs the final fiestel permutation
bitset<32> final_fiestel_perm(bitset<32> in_bits)
{
	bitset<32> out;
	
	for (int i = 0; i < 32; i++)
	{
		out[31 - i] = in_bits[31 - (fiestel_perm[i] - 1)];
	}
	
	return out;	
}


