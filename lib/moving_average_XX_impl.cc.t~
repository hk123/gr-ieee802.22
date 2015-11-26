
#include "@NAME_IMPL@.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace ieee802_22 {

@NAME_IMPL@::@NAME_IMPL@(int length) :
		sync_block("@NAME@",
			io_signature::make(1, 1, sizeof(@I_TYPE@)),
			io_signature::make(1, 1, sizeof(@O_TYPE@))),
		d_length(length),
		d_new_length(length),
		d_updated(false) {
	set_history(length);
}

void
@NAME_IMPL@::set_length(int length) {
	d_new_length = length;
	d_updated = true;
}

int
@NAME_IMPL@::work(int noutput_items, gr_vector_const_void_star
		&input_items, gr_vector_void_star &output_items) {

	if(d_updated) {
		d_length = d_new_length;
		set_history(d_length);
		d_updated = false;
		return 0; // history requirements might have changed
	}

	const @I_TYPE@ *in = (const @I_TYPE@ *)input_items[0];
	@O_TYPE@ *out = (@O_TYPE@ *)output_items[0];

	@I_TYPE@ sum = 0;
	for(int i = 0; i < d_length-1; i++) {
		sum += in[i];
	}

	for(int i = 0; i < noutput_items; i++) {
		sum += in[i + d_length - 1];
		out[i] = sum;
		sum -= in[i];
	}

	return noutput_items;
}

@NAME@::sptr
@NAME@::make(int length) {
        return gnuradio::get_initial_sptr(new @NAME_IMPL@(length));
}


} /* namespace blocks */
} /* namespace gr */
