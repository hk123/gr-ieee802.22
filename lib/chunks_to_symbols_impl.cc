
#include "chunks_to_symbols_impl.h"
#include "utils.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/tag_checker.h>
#include <assert.h>

using namespace gr::ieee802_22;

chunks_to_symbols::sptr
chunks_to_symbols::make()
{
	return gnuradio::get_initial_sptr(new chunks_to_symbols_impl());
}

chunks_to_symbols_impl::chunks_to_symbols_impl()
	: tagged_stream_block("wifi_chunks_to_symbols",
			   io_signature::make(1, 1, sizeof(char)),
			   io_signature::make(1, 1, sizeof(gr_complex)), "packet_len") {
}

chunks_to_symbols_impl::~chunks_to_symbols_impl() { }


int
chunks_to_symbols_impl::work(int noutput_items,
		gr_vector_int &ninput_items,
		gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items) {

	const unsigned char *in = (unsigned char*)input_items[0];
	gr_complex *out = (gr_complex*)output_items[0];

	std::vector<tag_t> tags;
	get_tags_in_range(tags, 0, nitems_read(0),
			nitems_read(0) + ninput_items[0],
			pmt::mp("encoding"));
	if(tags.size() != 1) {
		throw std::runtime_error("no encoding in input stream");
	}

	Encoding encoding = (Encoding)pmt::to_long(tags[0].value);

	const gr_complex *mapping;

	switch (encoding) {
	case BPSK_1_2:
	case BPSK_3_4:
		mapping = BPSK;
		break;

	case QPSK_1_2:
	case QPSK_3_4:
		mapping = QPSK;
		break;

	case QAM16_1_2:
	case QAM16_3_4:
		mapping = QAM16;
		break;

	case QAM64_2_3:
	case QAM64_3_4:
		mapping = QAM64;
		break;

	default:
		throw std::invalid_argument("wrong encoding");
		break;
	}

	for(int i = 0; i < ninput_items[0]; i++) {
		out[i] = mapping[in[i]];
	}

	return ninput_items[0];
}

const gr_complex chunks_to_symbols_impl::BPSK[2] = {
		gr_complex(-1.0, 0.0), gr_complex(1.0, 0.0)};

const gr_complex chunks_to_symbols_impl::QPSK[4] = {
		gr_complex(-0.7071, -0.7071), gr_complex(-0.7071, 0.7071),
		gr_complex(+0.7071, -0.7071), gr_complex(+0.7071, 0.7071)};

const gr_complex chunks_to_symbols_impl::QAM16[16] = {
		gr_complex(-0.9487, -0.9487), gr_complex(-0.9487, -0.3162),
		gr_complex(-0.9487, 0.9487), gr_complex(-0.9487, 0.3162),
		gr_complex(-0.3162, -0.9487), gr_complex(-0.3162, -0.3162),
		gr_complex(-0.3162, 0.9487), gr_complex(-0.3162, 0.3162),
		gr_complex(0.9487, -0.9487), gr_complex(0.9487, -0.3162),
		gr_complex(0.9487, 0.9487), gr_complex(0.9487, 0.3162),
		gr_complex(0.3162, -0.9487), gr_complex(0.3162, -0.3162),
		gr_complex(0.3162, 0.9487), gr_complex(0.3162, 0.3162)};

const gr_complex chunks_to_symbols_impl::QAM64[64] = {
		gr_complex(-1.0801, -1.0801), gr_complex(-1.0801, -0.7715),
		gr_complex(-1.0801, -0.1543), gr_complex(-1.0801, -0.4629),
		gr_complex(-1.0801, 1.0801), gr_complex(-1.0801, 0.7715),
		gr_complex(-1.0801, 0.1543), gr_complex(-1.0801, 0.4629),
		gr_complex(-0.7715, -1.0801), gr_complex(-0.7715, -0.7715),
		gr_complex(-0.7715, -0.1543), gr_complex(-0.7715, -0.4629),
		gr_complex(-0.7715, 1.0801), gr_complex(-0.7715, 0.7715),
		gr_complex(-0.7715, 0.1543), gr_complex(-0.7715, 0.4629),
		gr_complex(-0.1543, -1.0801), gr_complex(-0.1543, -0.7715),
		gr_complex(-0.1543, -0.1543), gr_complex(-0.1543, -0.4629),
		gr_complex(-0.1543, 1.0801), gr_complex(-0.1543, 0.7715),
		gr_complex(-0.1543, 0.1543), gr_complex(-0.1543, 0.4629),
		gr_complex(-0.4629, -1.0801), gr_complex(-0.4629, -0.7715),
		gr_complex(-0.4629, -0.1543), gr_complex(-0.4629, -0.4629),
		gr_complex(-0.4629, 1.0801), gr_complex(-0.4629, 0.7715),
		gr_complex(-0.4629, 0.1543), gr_complex(-0.4629, 0.4629),
		gr_complex(1.0801, -1.0801), gr_complex(1.0801, -0.7715),
		gr_complex(1.0801, -0.1543), gr_complex(1.0801, -0.4629),
		gr_complex(1.0801, 1.0801), gr_complex(1.0801, 0.7715),
		gr_complex(1.0801, 0.1543), gr_complex(1.0801, 0.4629),
		gr_complex(0.7715, -1.0801), gr_complex(0.7715, -0.7715),
		gr_complex(0.7715, -0.1543), gr_complex(0.7715, -0.4629),
		gr_complex(0.7715, 1.0801), gr_complex(0.7715, 0.7715),
		gr_complex(0.7715, 0.1543), gr_complex(0.7715, 0.4629),
		gr_complex(0.1543, -1.0801), gr_complex(0.1543, -0.7715),
		gr_complex(0.1543, -0.1543), gr_complex(0.1543, -0.4629),
		gr_complex(0.1543, 1.0801), gr_complex(0.1543, 0.7715),
		gr_complex(0.1543, 0.1543), gr_complex(0.1543, 0.4629),
		gr_complex(0.4629, -1.0801), gr_complex(0.4629, -0.7715),
		gr_complex(0.4629, -0.1543), gr_complex(0.4629, -0.4629),
		gr_complex(0.4629, 1.0801), gr_complex(0.4629, 0.7715),
		gr_complex(0.4629, 0.1543), gr_complex(0.4629, 0.4629)};

