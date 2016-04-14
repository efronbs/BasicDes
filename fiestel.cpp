#include "des.h"
#include "constants.h"

#define ROUNDCOUNT 16 
#define ITERATIONS 100000 

int main()
{
	//Main test
	bitset<64> test_msg (string("0101010101010101010101010101010101010101010101010101010101010101"));
	bitset<64> test_key (string("0111111101001001110000101000001100011100000011101111010011101001"));
	//bitset<64> test_key (string("0001001100110100010101110111100110011011101111001101111111110001"));
	//setup
	generate_all_keys(test_key, ROUNDCOUNT);
	//cout << (all_round_keys[15].to_string() == string("110010110011110110001011000011100001011111110101")) << "\n\n";
	
	//for each iteration xor with prev result and run des on that value
	auto current_encryption = codebook0;

	for (int i = 0; i < ITERATIONS; i++)
	{	
		auto starting_msg = current_encryption ^ test_msg; //using base message for all messages
		auto permed_msg = perform_initial_perm(starting_msg);	
		
		//run iteration of des
		current_encryption = run_des(permed_msg, ROUNDCOUNT);
	}
	cout << current_encryption << "\n";
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
	
	//cout << "L: " << l_msg << "\n";
	//cout << "R: " << r_msg << "\n";
	
	//run the fiestel network for the designated amount of rounds
	for (int i = 0; i < num_rounds; i++)
	{
		new_l = r_msg;
		auto fiestel_result = run_fiestel(r_msg, all_round_keys[i]);
		//cout << "fiestel round " << i + 1 << ": " <<  fiestel_result.to_string() << "\n\n";
		new_r = l_msg ^ fiestel_result;

		l_msg = new_l;
		r_msg = new_r;

		//cout << "L " << i + 1 << ": " << l_msg << "\n";
		//cout << "R " << i + 1 << ": " << (r_msg.to_string() == string("01101010001000101111110010001100")) << "\n\n";	
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
	//cout << "encrypted data: " << enc_data.to_string() << "\n";
}

bitset<64> revert_initial_perm(bitset<64> enc_data)
{
        bitset<64> out;
        for (int i = 0; i < 64; i++)
                out[63 - i] = enc_data[63 - (initial_perm_inverse[i] - 1)];

        return out;
}

//generates all keys for this DES iteration
void generate_all_keys(bitset<64> base_key, int number_of_rounds)
{
	auto perm_key = init_key_permute(base_key);
	key_shift(perm_key, number_of_rounds);

}

/* KEY GENERATION */

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
		if (shift_type == 2) {
			c_set = rotate_left_two(c_set);
			d_set = rotate_left_two(d_set);
		} else {
			c_set = rotate_left_one(c_set);
			d_set = rotate_left_one(d_set);			 
		}
		
		//cout << "shift amt: " << shift_type << "\n";
		//cout << "c_set " << j + 1 << ": " << c_set << "\n";		
		//cout << "d_set " << j + 1 << ": " << d_set << "\n\n";		


		//put back together to get "full" key after shift
		for (int i = 0; i < 56; i++)
		{
			if (i < 28) {
				out[i] = d_set[i];
			} else {
				out[i] = c_set[i - 28];
			}
		}
		all_round_keys[j] = round_key_permute(out);	
		//permute into final form
		//cout << "key " << j << ": " <<  round_key_permute(out).to_string() << "\n\n";
	}
	 	
}

bitset<48> round_key_permute(bitset<56> in_bits)
{
	bitset<48> out;
        for (int i = 0; i < 48; i++)
                out[47 - i] = in_bits[55 - (round_key_perm[i] - 1)];
        return out;

}

bitset<56> init_key_permute(bitset<64> in_bits)
{
	bitset<56> out;
	for (int i = 0; i < 56; i++)
                out[55 - i] = in_bits[63 - (key_perm[i] - 1)];

	return out;
}

bitset<28> rotate_left_one(bitset<28> in_bits)
{
	bitset<28> out;
	auto temp = in_bits[27];
	out = in_bits<<1;
	out[0] = temp;
	return out;
}

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

bitset<64> perform_initial_perm(bitset<64> in_bits)
{
        bitset<64> out;
        for (int i = 0; i < 64; i++)
                out[63 - i] = in_bits[63 - (initial_perm[i] - 1)];

        return out;
}

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

std::bitset<48> expand(std::bitset<32> toExpand)
{
	std::bitset<48> out;

	//magic number 48, the size of the permuation
	for (int i = 0; i < 48; i++)
		out[47 - i] = toExpand[31 - (expansion_perm_bits[i] - 1)]; //grab the bit number from that place in the expansion array, sub 1 for correct index
	return out;	
		
}

//just for ease of reading
void xor_with_key(std::bitset<48> *block, std::bitset<48> key_round)
{
	*block ^= key_round;	
}

bitset<32> sbox_process(bitset<48> in_bits)
{
	string full_str = in_bits.to_string();
	string result_str = "";
	string strset[8];
	for (int i = 0; i < 8; i++) 
	{
		strset[i] = full_str.substr(i * 6, 6);
		//cout << "b" << i << " : " << strset[i] << "\n";
	}
	
	for (int i = 0; i < 8; i++)
	{
		auto current_box = all_sboxes[i];
		string current_str = strset[i];
	
		//cout << "sbox in: " << current_str << "\n";
		
		//grab the first and last bits
		string temp = "";
		temp += current_str[0];
		temp += current_str[5];
		bitset<2> selectors (temp);

		//cout << "row chosen: " << selectors.to_ulong() << "\n\tbinary: " << temp  << "\n";

		auto current_row = current_box[selectors.to_ulong()];
		bitset<4> s_in (current_str.substr(1, 4));
		
		//cout << "column chosen: " << s_in.to_ulong() << "\n";
		
		int s_out_raw = current_row[s_in.to_ulong()];
		bitset<4> s_out_bts (s_out_raw);
		
		//cout << "output bits: " << s_out_bts.to_string() << "\n";
		
		result_str += s_out_bts.to_string();	
	}	
	bitset<32> result_bts(result_str);
	return result_bts; 
}

bitset<32> final_fiestel_perm(bitset<32> in_bits)
{
	bitset<32> out;
	
	for (int i = 0; i < 32; i++)
	{
		out[31 - i] = in_bits[31 - (fiestel_perm[i] - 1)];
	}
	//cout << out.to_string() << "\n";
	return out;	
}


