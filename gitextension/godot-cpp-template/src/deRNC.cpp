#include "deRNC.h"
uint8 read_byte(uint8 *buf, size_t *offset) {
	return buf[(*offset)++];
}
uint16 read_word_be(uint8 *buf, size_t *offset) {
	uint8 b1 = read_byte(buf, offset);
	uint8 b2 = read_byte(buf, offset);

	return (b1 << 8) | b2;
}
uint32 read_dword_be(uint8 *buf, size_t *offset) {
	uint16 w1 = read_word_be(buf, offset);
	uint16 w2 = read_word_be(buf, offset);

	return (w1 << 16) | w2;
}
uint16 crc_block(uint8 *buf, size_t offset, int size) {
	uint16 crc = 0;

	while (size--) {
		crc ^= read_byte(buf, &offset);
		crc = (crc >> 8) ^ crc_table[crc & 0xFF];
	}

	return crc;
}
void read_buf(uint8 *dest, uint8 *source, size_t *offset, int size) {
	memmove(dest, &source[*offset], size);
	*offset += size;
}
uint8 read_source_byte(vars_t *v) {
	if (v->pack_block_start == &v->mem1[0xFFFD]) {
		int left_size = v->file_size - v->input_offset;

		int size_to_read;
		if (left_size <= 0xFFFD)
			size_to_read = left_size;
		else
			size_to_read = 0xFFFD;

		v->pack_block_start = v->mem1;

		read_buf(v->pack_block_start, v->input, &v->input_offset, size_to_read);

		if (left_size - size_to_read > 2)
			left_size = 2;

		read_buf(&v->mem1[size_to_read], v->input, &v->input_offset, left_size);
		v->input_offset -= left_size;
	}

	return *v->pack_block_start++;
}
uint32 input_bits_m2(vars_t *v, short count) {
	uint32 bits = 0;

	while (count--) {
		if (!v->bit_count) {
			v->bit_buffer = read_source_byte(v);
			v->bit_count = 8;
		}

		bits <<= 1;

		if (v->bit_buffer & 0x80)
			bits |= 1;

		v->bit_buffer <<= 1;
		v->bit_count--;
	}

	return bits;
}

uint32 input_bits_m1(vars_t *v, short count) {
	uint32 bits = 0;
	uint32 prev_bits = 1;

	while (count--) {
		if (!v->bit_count) {
			uint8 b1 = read_source_byte(v);
			uint8 b2 = read_source_byte(v);
			v->bit_buffer = (v->pack_block_start[1] << 24) | (v->pack_block_start[0] << 16) | (b2 << 8) | b1;

			v->bit_count = 16;
		}

		if (v->bit_buffer & 1)
			bits |= prev_bits;

		v->bit_buffer >>= 1;
		prev_bits <<= 1;
		v->bit_count--;
	}

	return bits;
}

int input_bits(vars_t *v, short count) {
	if (v->method != 2)
		return input_bits_m1(v, count);
	else
		return input_bits_m2(v, count);
}
uint32 decode_table_data(vars_t *v, huftable_t *data) {
	uint32 i = 0;

	while (1) {
		if (data[i].bit_depth && (data[i].l3 == (v->bit_buffer & ((1 << data[i].bit_depth) - 1)))) {
			input_bits_m1(v, data[i].bit_depth);

			if (i < 2)
				return i;

			return input_bits_m1(v, i - 1) | (1 << (i - 1));
		}

		i++;
	}
}
void clear_table(huftable_t *data, int count) {
	for (int i = 0; i < count; ++i) {
		data[i].l1 = 0;
		data[i].l2 = 0xFFFF;
		data[i].l3 = 0;
		data[i].bit_depth = 0;
	}
}
uint32 inverse_bits(uint32 value, int count) {
	int i = 0;
	while (count--) {
		i <<= 1;

		if (value & 1)
			i |= 1;

		value >>= 1;
	}

	return i;
}
void proc_20(huftable_t *data, int count) {
	int val = 0;
	uint32 div = 0x80000000;
	int bits_count = 1;

	while (bits_count <= 16) {
		int i = 0;

		while (1) {
			if (i >= count) {
				bits_count++;
				div >>= 1;
				break;
			}

			if (data[i].bit_depth == bits_count) {
				data[i].l3 = inverse_bits(val / div, bits_count);
				val += div;
			}

			i++;
		}
	}
}
void make_huftable(vars_t *v, huftable_t *data, int count) {
	clear_table(data, count);

	int leaf_nodes = input_bits_m1(v, 5);

	if (leaf_nodes) {
		if (leaf_nodes > 16)
			leaf_nodes = 16;

		for (int i = 0; i < leaf_nodes; ++i)
			data[i].bit_depth = input_bits_m1(v, 4);

		proc_20(data, leaf_nodes);
	}
}
void ror_w(uint16 *x) {
	if (*x & 1)
		*x = 0x8000 | (*x >> 1);
	else
		*x >>= 1;
}
void decode_match_count(vars_t *v) {
	v->match_count = input_bits_m2(v, 1) + 4;

	if (input_bits_m2(v, 1))
		v->match_count = ((v->match_count - 1) << 1) + input_bits_m2(v, 1);
}
void decode_match_offset(vars_t *v) {
	v->match_offset = 0;
	if (input_bits_m2(v, 1)) {
		v->match_offset = input_bits_m2(v, 1);

		if (input_bits_m2(v, 1)) {
			v->match_offset = ((v->match_offset << 1) | input_bits_m2(v, 1)) | 4;

			if (!input_bits_m2(v, 1))
				v->match_offset = (v->match_offset << 1) | input_bits_m2(v, 1);
		} else if (!v->match_offset)
			v->match_offset = input_bits_m2(v, 1) + 2;
	}

	v->match_offset = ((v->match_offset << 8) | read_source_byte(v)) + 1;
}
void write_buf(uint8 *dest, size_t *offset, uint8 *source, int size) {
	memmove(&dest[*offset], source, size);
	*offset += size;
}
void write_decoded_byte(vars_t *v, uint8 b) {
	if (&v->decoded[0xFFFF] == v->window) {
		write_buf(v->output, &v->output_offset, &v->decoded[v->dict_size], 0xFFFF - v->dict_size);
		memmove(v->decoded, &v->window[-v->dict_size], v->dict_size);
		v->window = &v->decoded[v->dict_size];
	}

	*v->window++ = b;
	v->unpacked_crc_real = crc_table[(v->unpacked_crc_real ^ b) & 0xFF] ^ (v->unpacked_crc_real >> 8);
}
int unpack_data_m1(vars_t *v) {
	while (v->processed_size < v->input_size) {
		make_huftable(v, v->raw_table, _countof(v->raw_table));
		make_huftable(v, v->len_table, _countof(v->len_table));
		make_huftable(v, v->pos_table, _countof(v->pos_table));

		int subchunks = input_bits_m1(v, 16);

		while (subchunks--) {
			uint32 data_length = decode_table_data(v, v->raw_table);
			v->processed_size += data_length;

			if (data_length) {
				while (data_length--)
					write_decoded_byte(v, (v->enc_key ^ read_source_byte(v)) & 0xFF);

				ror_w(&v->enc_key);

				v->bit_buffer = (((v->pack_block_start[2] << 16) | (v->pack_block_start[1] << 8) | v->pack_block_start[0]) << v->bit_count) | (v->bit_buffer & ((1 << v->bit_count) - 1));
			}

			if (subchunks) {
				v->match_offset = decode_table_data(v, v->len_table) + 1;
				v->match_count = decode_table_data(v, v->pos_table) + 2;
				v->processed_size += v->match_count;

				while (v->match_count--)
					write_decoded_byte(v, v->window[-v->match_offset]);
			}
		}
	}

	write_buf(v->output, &v->output_offset, &v->decoded[v->dict_size], v->window - &v->decoded[v->dict_size]);
	return 0;
}
int unpack_data_m2(vars_t *v) {
	while (v->processed_size < v->input_size) {
		while (1) {
			if (!input_bits_m2(v, 1)) {
				write_decoded_byte(v, (v->enc_key ^ read_source_byte(v)) & 0xFF);

				ror_w(&v->enc_key);

				v->processed_size++;
			} else {
				if (input_bits_m2(v, 1)) {
					if (input_bits_m2(v, 1)) {
						if (input_bits_m2(v, 1)) {
							v->match_count = read_source_byte(v) + 8;

							if (v->match_count == 8) {
								input_bits_m2(v, 1);
								break;
							}
						} else
							v->match_count = 3;

						decode_match_offset(v);
					} else {
						v->match_count = 2;
						v->match_offset = read_source_byte(v) + 1;
					}

					v->processed_size += v->match_count;

					while (v->match_count--)
						write_decoded_byte(v, v->window[-v->match_offset]);
				} else {
					decode_match_count(v);

					if (v->match_count != 9) {
						decode_match_offset(v);
						v->processed_size += v->match_count;

						while (v->match_count--)
							write_decoded_byte(v, v->window[-v->match_offset]);
					} else {
						uint32 data_length = (input_bits_m2(v, 4) << 2) + 12;
						v->processed_size += data_length;

						while (data_length--)
							write_decoded_byte(v, (v->enc_key ^ read_source_byte(v)) & 0xFF);

						ror_w(&v->enc_key);
					}
				}
			}
		}
	}

	write_buf(v->output, &v->output_offset, &v->decoded[v->dict_size], v->window - &v->decoded[v->dict_size]);
	return 0;
}
int do_unpack_data(vars_t *v) {
	int start_pos = v->input_offset;

	uint32 sign = read_dword_be(v->input, &v->input_offset);
	if ((sign >> 8) != RNC_SIGN)
		return 6;

	v->method = sign & 3;
	v->input_size = read_dword_be(v->input, &v->input_offset);
	v->packed_size = read_dword_be(v->input, &v->input_offset);
	if (v->file_size < v->packed_size)
		return 7;
	v->unpacked_crc = read_word_be(v->input, &v->input_offset);
	v->packed_crc = read_word_be(v->input, &v->input_offset);

	/*v->leeway = */ read_byte(v->input, &v->input_offset);
	/*v->chunks_count = */ read_byte(v->input, &v->input_offset);

	if (crc_block(v->input, v->input_offset, v->packed_size) != v->packed_crc)
		return 4;

	v->mem1 = (uint8 *)malloc(0xFFFF);
	v->decoded = (uint8 *)malloc(0xFFFF);
	v->pack_block_start = &v->mem1[0xFFFD];
	v->window = &v->decoded[v->dict_size];

	v->unpacked_crc_real = 0;
	v->bit_count = 0;
	v->bit_buffer = 0;
	v->processed_size = 0;

	uint16 specified_key = v->enc_key;

	int error_code = 0;
	if (input_bits(v, 1) && !v->pus_mode)
		error_code = 9;

	if (!error_code) {
		if (input_bits(v, 1) && !v->enc_key) // key is needed, but not specified as argument
			error_code = 10;
	}

	if (!error_code) {
		switch (v->method) {
			case 1:
				error_code = unpack_data_m1(v);
				break;
			case 2:
				error_code = unpack_data_m2(v);
				break;
		}
	}

	v->enc_key = specified_key;

	free(v->mem1);
	free(v->decoded);

	v->input_offset = start_pos + v->packed_size + RNC_HEADER_SIZE;

	if (error_code)
		return error_code;

	if (v->unpacked_crc != v->unpacked_crc_real)
		return 5;

	return 0;
}

int do_unpack(vars_t *v) {
	v->packed_size = v->file_size;

	if (v->file_size < RNC_HEADER_SIZE)
		return 6;

	int result = do_unpack_data(v); // data
	if (result == 0)
		return v->input_size;
	return -result;
}

vars_t *init_vars() {
	vars_t *v = (vars_t *)malloc(sizeof(vars_t));
	v->enc_key = 0;
	v->max_matches = 0x1000;
	v->unpacked_crc_real = 0;
	v->pack_block_size = 0x3000;
	v->dict_size = 0xFFFF;
	v->method = 1;
	v->pus_mode = 0;

	v->read_start_offset = 0;
	v->write_start_offset = 0;
	v->input_offset = 0;
	v->output_offset = 0;
	v->temp_offset = 0;

	memset(v->tmp_crc_data, 0, sizeof(v->tmp_crc_data));
	memset(v->raw_table, 0, sizeof(v->raw_table));
	memset(v->pos_table, 0, sizeof(v->pos_table));
	memset(v->len_table, 0, sizeof(v->len_table));

	return v;
}

int sub_9894C_decompress(Bit8u *a1, Bit8u *a2) {
	vars_t *v = init_vars();
	if (v->method == 1) {
		if (v->dict_size > 0x8000)
			v->dict_size = 0x8000;
		v->max_matches = 0x1000;
	} else if (v->method == 2) {
		if (v->dict_size > 0x1000)
			v->dict_size = 0x1000;
		v->max_matches = 0xFF;
	}

	v->file_size = MAX_BUF_SIZE;
	v->input = (uint8 *)malloc(MAX_BUF_SIZE);

	Bit32u signature = a1[0] + (a1[1] << 8) + (a1[2] << 16);
	if (signature == 0x434e52) {
		Bit32u inputsize = a1[11] + (a1[10] << 8) + (a1[9] << 16) + (a1[8] << 24);
		memmove(v->input, a1, inputsize + 0x12);
	} else {
		v->input[0] = 0;
	}
	//v->input = a1;

	v->output = a2;
	v->temp = (uint8 *)malloc(MAX_BUF_SIZE);

	int error_code = 0;
	/*switch (v->pus_mode)
	{
	case 0: error_code = do_pack(v); break;
	case 1: error_code = do_unpack(v); break;
	case 2: error_code = do_search(v); break;
	}*/
	error_code = do_unpack(v);

	if (v->pus_mode == 2) {
		free(v->input);
		free(v->output);
		free(v->temp);
		free(v->input);
		free(v);

		return error_code;
	}

	/*if (error_code)
	 {
		switch (error_code) {
		case 4: printf("Corrupted input data.\n"); break;
		case 5: printf("CRC check failed.\n"); break;
		case 6:
		case 7:
			printf("Wrong RNC header.\n"); break;
		case 10: printf("No RNC archives were found.\n"); break;
		default: printf("Cannot process file. Error code: %x\n", error_code); break;
		}
	}*/

	free(v->input);
	//free(v->output);
	free(v->temp);
	free(v);
	if (error_code == -6)
		return 0;
	//if (error_code == 0)return 0;
	return error_code;
}
