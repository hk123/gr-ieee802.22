
#ifndef INCLUDED_IEEE802_22_EQUALIZER_BASE_H
#define INCLUDED_IEEE802_22_EQUALIZER_BASE_H

#include <gnuradio/gr_complex.h>

namespace gr {
namespace ieee802_22 {
namespace equalizer {

class base {
public:
	virtual ~base() {};
	virtual void equalize(const gr_complex *in, gr_complex *out, int n) = 0;
};

} /* namespace channel_estimation */
} /* namespace ieee802_22 */
} /* namespace gr */

#endif /* INCLUDED_IEEE802_22_EQUALIZER_BASE_H */
