#include "des.h"
#include "constants.h"

int main()
{
        bitset<64> bts1 (string("0101010101010101010101010101010101010101010101010101010101010101"));
        cout << perform_initial_perm(bts1).to_string() << "\n";
}

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
	xor_with_key(&expanded, key_round);
	auto sb_out = sbox_process(expanded);
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
	cout << out.to_string() << "\n";
	return out;	
}


