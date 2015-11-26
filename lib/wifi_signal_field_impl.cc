

#include "wifi_signal_field_impl.h"
#include "utils.h"
#include <gnuradio/digital/lfsr.h>

using namespace gr::ieee802_22;

wifi_signal_field::sptr
wifi_signal_field::make() {
	return wifi_signal_field::sptr(new wifi_signal_field_impl());
}

wifi_signal_field::wifi_signal_field() : packet_header_default(48, "packet_len") {};


wifi_signal_field_impl::wifi_signal_field_impl() : packet_header_default(48, "packet_len") {}


wifi_signal_field_impl::~wifi_signal_field_impl() {}


int wifi_signal_field_impl::get_bit(int b, int i) {
	return (b & (1 << i) ? 1 : 0);
}


void wifi_signal_field_impl::generate_signal_field(char *out, tx_param &tx, ofdm_param &ofdm) {

	//data bits of the signal header
	char *signal_header = (char *) malloc(sizeof(char) * 24);

	//signal header after...
	//convolutional encoding
	char *encoded_signal_header = (char *) malloc(sizeof(char) * 48);
	//interleaving
	char *interleaved_signal_header = (char *) malloc(sizeof(char) * 48);

	int length = tx.psdu_size;

	// first 4 bits represent the modulation and coding scheme
	signal_header[ 0] = get_bit(ofdm.rate_field, 3);
	signal_header[ 1] = get_bit(ofdm.rate_field, 2);
	signal_header[ 2] = get_bit(ofdm.rate_field, 1);
	signal_header[ 3] = get_bit(ofdm.rate_field, 0);
	// 5th bit is reserved and must be set to 0
	signal_header[ 4] = 0;
	// then 12 bits represent the length
	signal_header[ 5] = get_bit(length,  0);
	signal_header[ 6] = get_bit(length,  1);
	signal_header[ 7] = get_bit(length,  2);
	signal_header[ 8] = get_bit(length,  3);
	signal_header[ 9] = get_bit(length,  4);
	signal_header[10] = get_bit(length,  5);
	signal_header[11] = get_bit(length,  6);
	signal_header[12] = get_bit(length,  7);
	signal_header[13] = get_bit(length,  8);
	signal_header[14] = get_bit(length,  9);
	signal_header[15] = get_bit(length, 10);
	signal_header[16] = get_bit(length, 11);
	//18-th bit is the parity bit for the first 17 bits
	int sum = 0;
	for(int i = 0; i < 17; i++) {
		if(signal_header[i]) {
			sum++;
		}
	}
	signal_header[17] = sum % 2;

	// last 6 bits must be set to 0
	for (int i = 0; i < 6; i++) {
		signal_header[18 + i] = 0;
	}

	ofdm_param signal_ofdm(BPSK_1_2);
	tx_param signal_tx(signal_ofdm, 3);
	signal_tx.n_data = 24;
	signal_tx.n_sym = 1;

	// convolutional encoding (scrambling is not needed)
	convolutional_encoding(signal_header, encoded_signal_header, signal_tx);
	// interleaving
	//interleave(encoded_signal_header, interleaved_signal_header, signal_tx, signal_ofdm);
	interleave(encoded_signal_header, out, signal_tx, signal_ofdm);

	free(signal_header);
	free(encoded_signal_header);
	free(interleaved_signal_header);
}

bool wifi_signal_field_impl::header_formatter(long packet_len, unsigned char *out, const std::vector<tag_t> &tags)
{

	bool encoding_found = false;
	bool len_found = false;
	int encoding = 0;
	int len = 0;

	// read tags
	for(int i = 0; i < tags.size(); i++) {
		if(pmt::eq(tags[i].key, pmt::mp("encoding"))) {
			encoding_found = true;
			encoding = pmt::to_long(tags[i].value);
		} else if(pmt::eq(tags[i].key, pmt::mp("psdu_len"))) {
			len_found = true;
			len = pmt::to_long(tags[i].value);
		}
	}

	// check if all tags are present
	if( (!encoding_found) || (!len_found)) {
		return false;
	}

	ofdm_param ofdm((Encoding)encoding);
	tx_param tx(ofdm, len);

	generate_signal_field((char*)out, tx, ofdm);
	return true;
}

bool wifi_signal_field_impl::header_parser(
		const unsigned char *in, std::vector<tag_t> &tags) {

	throw std::runtime_error("not implemented yet");
	return false;
}
