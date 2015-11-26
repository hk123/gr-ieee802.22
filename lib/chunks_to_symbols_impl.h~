
#ifndef INCLUDED_IEEE802_22_CHUNKS_TO_SYMBOLS_IMPL_H
#define INCLUDED_IEEE802_22_CHUNKS_TO_SYMBOLS_IMPL_H

#include <ieee802-22/chunks_to_symbols.h>

namespace gr {
namespace ieee802_22 {

class chunks_to_symbols_impl : public chunks_to_symbols
{
public:
	chunks_to_symbols_impl();
	~chunks_to_symbols_impl();

	int work(int noutput_items,
			gr_vector_int &ninput_itmes,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items);

protected:
	static const gr_complex BPSK[2];
	static const gr_complex QPSK[4];
	static const gr_complex QAM16[16];
	static const gr_complex QAM64[64];
};

} /* namespace ieee802_22 */
} /* namespace gr */

#endif /* INCLUDED_IEEE802_22_CHUNKS_TO_SYMBOLS_IMPL_H */
