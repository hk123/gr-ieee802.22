
#include <ieee802-22/ofdm_equalize_symbols.h>

#include "utils.h"
#include "equalizer/base.h"
#include "equalizer/linear_comb.h"
#include "equalizer/lms.h"
#include <gnuradio/io_signature.h>

using namespace gr::ieee802_22;


class ofdm_equalize_symbols_impl : public ofdm_equalize_symbols {

public:
ofdm_equalize_symbols_impl(Equalizer algo, bool debug) : block("ofdm_equalize_symbols",
			gr::io_signature::make(1, 1, 2048 * sizeof(gr_complex)),
			gr::io_signature::make(1, 1, 1440 * sizeof(gr_complex))),
			d_debug(debug), d_equalizer(NULL) {

	set_relative_rate(1);
	set_tag_propagation_policy(block::TPP_DONT);
	set_algorithm(algo);
}

~ofdm_equalize_symbols_impl(){
}

int general_work (int noutput_items, gr_vector_int& ninput_items,
		gr_vector_const_void_star& input_items,
		gr_vector_void_star& output_items) {

	gr::thread::scoped_lock lock(d_mutex);

	const gr_complex *in = (const gr_complex*)input_items[0];
	gr_complex *out = (gr_complex*)output_items[0];

	int i = 0;
	int o = 0;

	dout << "SYMBOLS: input " << ninput_items[0] << "  output " << noutput_items << std::endl;

	while((i < ninput_items[0]) && (o < noutput_items)) {

		get_tags_in_window(tags, 0, i, i + 1, pmt::string_to_symbol("ofdm_start"));

		// new WiFi frame
		if(tags.size()) {
			d_nsym = 0;
		}

		// first data symbol (= signal field)
		if(d_nsym == 1) {
			add_item_tag(0, nitems_written(0) + o,
				pmt::string_to_symbol("ofdm_start"),
				pmt::PMT_T,
				pmt::string_to_symbol(name()));
		}
		
		d_equalizer->equalize(in + (i * 2048), out + (o * 1440), d_nsym);
		
		if(d_nsym > 0) {
			o++;
		}
		i++;
		d_nsym++;
	}

	dout << "SYMBOLS: consumed " << i << "  produced " << o << std::endl;

	consume(0, i);
	return o;
}

void set_algorithm(Equalizer algo) {
	gr::thread::scoped_lock lock(d_mutex);
	delete d_equalizer;

	switch(algo) {
	case LMS:
		dout << "LMS" << std::endl;
		d_equalizer = new equalizer::lms();
		break;

	case LINEAR_COMB:
		dout << "Linear Comb" << std::endl;
		d_equalizer = new equalizer::linear_comb();
		break;
	}
}

private:
	int          d_nsym;
	const bool   d_debug;
	equalizer::base *d_equalizer;
	std::vector<gr::tag_t> tags;
	gr::thread::mutex d_mutex;
};


ofdm_equalize_symbols::sptr
ofdm_equalize_symbols::make(Equalizer algo, bool debug) {
	return gnuradio::get_initial_sptr(new ofdm_equalize_symbols_impl(algo, debug));
}

