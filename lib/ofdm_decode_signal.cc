
#include <ieee802-22/ofdm_decode_signal.h>
#include <gnuradio/io_signature.h>
#include "utils.h"

#include <itpp/itcomm.h>

using namespace gr::ieee802_22;
using namespace itpp;


class ofdm_decode_signal_impl : public ofdm_decode_signal {

public:
ofdm_decode_signal_impl(bool log, bool debug) : block("ofdm_decode_signal",
			gr::io_signature::make(1, 1, 1440 * sizeof(gr_complex)),
			gr::io_signature::make(1, 1, 1440 * sizeof(gr_complex))),
			d_log(log),
			d_debug(debug),
			d_copy_symbols(0) {

	decoded_bits.set_size(24);
	set_relative_rate(1);
	set_tag_propagation_policy(block::TPP_DONT);
}

~ofdm_decode_signal_impl(){
}

int general_work (int noutput_items, gr_vector_int& ninput_items,
		gr_vector_const_void_star& input_items,
		gr_vector_void_star& output_items) {

	const gr_complex *in = (const gr_complex*)input_items[0];
	gr_complex *out = (gr_complex*)output_items[0];

	int i = 0;
	int o = 0;

	std::vector<gr::tag_t> tags;
	const uint64_t nread = nitems_read(0);

	dout << "Decode Signal: input " << ninput_items[0]
		<< "  output " << noutput_items << std::endl;

	while((i < ninput_items[0]) && (o < noutput_items)) {

		get_tags_in_range(tags, 0, nread + i, nread + i + 1,
			pmt::string_to_symbol("ofdm_start"));

		if(tags.size()) {
		d_copy_symbols = 2;
		//	for(int n = 0; n < 48; n++) {
		//		bits[n] = -real(in[n]);
		//	}

		//	deinterleave();

		//	decode();

		//	if(print_signal()) {

		//		add_item_tag(0, nitems_written(0) + o,
		//			pmt::string_to_symbol("ofdm_start"),
		//			pmt::cons(pmt::from_uint64(d_len),
		//				pmt::from_uint64(d_encoding)),
		//			pmt::string_to_symbol(name()));
		
		//	}

		} 
		 if(d_copy_symbols) {

			std::memcpy(out, in, 1440 * sizeof(gr_complex));
			o++;
			out += 1440;
			d_copy_symbols--;
		}

		in += 1440;
		i++;
	}

	dout << "Decode Signal consumed " << i <<
		"   produced " << o << std::endl;

	consume(0, i);
	return o;
}

void deinterleave() {
	double tmp[48];
	for(int i = 0; i < 48; i++) {
		tmp[i] = bits[inter[i]];
	}
	for(int i = 0; i < 48; i++) {
		bits[i] = tmp[i];
	}
	for(int i = 0; i < 48; i++) {
		dout << bits[i] << " ";
	}
	dout << std::endl;

}

void decode() {

	Convolutional_Code code;
	ivec generator(2);
	generator(0)=0133;
	generator(1)=0171;
	code.set_generator_polynomials(generator, 7);
	code.set_truncation_length(30);

	vec rx_signal(bits, 48);
	code.reset();
	code.decode_tail(rx_signal, decoded_bits);

	dout << "length rx " << rx_signal.size() << std::endl;
	dout << rx_signal << std::endl;
	dout << "length decoded " << decoded_bits.size() << std::endl;
	dout << decoded_bits << std::endl;

}

bool print_signal() {

	int r = 0;
	d_len = 0;
	bool parity = false;
	for(int i = 0; i < 17; i++) {
		parity ^= (bool)decoded_bits[i];

		if((i < 4) && decoded_bits[i]) {
			r = r | (1 << i);
		}

		if(decoded_bits[i] && (i > 4) && (i < 17)) {
			d_len = d_len | (1 << (i-5));
		}
	}

	if(parity != (bool)decoded_bits[17]) {
		dout << "SIGNAL: wrong parity" << std::endl;
		return false;
	}

	if(r == 11) {
		d_encoding = 0;
		d_copy_symbols = (int) ceil((16 + 8 * d_len + 6) / (double) 24);
		dout << "Encoding: 3 Mbit/s   ";
	} else if(r == 15) {
		d_encoding = 1;
		d_copy_symbols = (int) ceil((16 + 8 * d_len + 6) / (double) 36);
		dout << "Encoding: 4.5 Mbit/s   ";
	} else if(r == 10) {
		d_encoding = 2;
		d_copy_symbols = (int) ceil((16 + 8 * d_len + 6) / (double) 48);
		dout << "Encoding: 6 Mbit/s   ";
	} else if(r == 14) {
		d_encoding = 3;
		d_copy_symbols = (int) ceil((16 + 8 * d_len + 6) / (double) 72);
		dout << "Encoding: 9 Mbit/s   ";
	} else if(r ==  9) {
		d_encoding = 4;
		d_copy_symbols = (int) ceil((16 + 8 * d_len + 6) / (double) 96);
		dout << "Encoding: 12 Mbit/s   ";
	} else if(r == 13) {
		d_encoding = 5;
		d_copy_symbols = (int) ceil((16 + 8 * d_len + 6) / (double) 144);
		dout << "Encoding: 18 Mbit/s   ";
	} else if(r ==  8) {
		d_encoding = 6;
		d_copy_symbols = (int) ceil((16 + 8 * d_len + 6) / (double) 192);
		dout << "Encoding: 24 Mbit/s   ";
	} else if(r ==  12) {
		d_encoding = 7;
		d_copy_symbols = (int) ceil((16 + 8 * d_len + 6) / (double) 216);
		dout << "Encoding: 27 Mbit/s   ";
	} else {
		dout << "unknown encoding" << std::endl;
		return false;
	}

	mylog(boost::format("encoding: %1% - length: %2% - symbols: %3%")
			% d_encoding % d_len % d_copy_symbols);

	dout << "d_copy_symbols: " << d_copy_symbols << std::endl;
	dout << "length: " << d_len << std::endl;
	return true;
}

private:
	int    d_len;
	int    d_encoding;
	bool   d_log;
	bool   d_debug;
	double bits[48];
	int    d_copy_symbols;
	bvec decoded_bits;
	static int inter[48];
};

ofdm_decode_signal::sptr
ofdm_decode_signal::make(bool log, bool debug) {
	return gnuradio::get_initial_sptr(new ofdm_decode_signal_impl(log, debug));
}

int ofdm_decode_signal_impl::inter[48]={0,3,6,9,12,15,18,21,24,27,30,33,36,39,42,45,1,4,7,10,13,16,19,22,25,28,31,34,37,40,43,46,2,5,8,11,14,17,20,23,26,29,32,35,38,41,44,47};

