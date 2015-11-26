

#ifndef INCLUDED_IEEE802_22_EQUALIZER_LINEAR_COMB_H
#define INCLUDED_IEEE802_22_EQUALIZER_LINEAR_COMB_H

#include "base.h"

namespace gr {
namespace ieee802_22 {
namespace equalizer {

class linear_comb: public base {
public:
	virtual void equalize(const gr_complex *in, gr_complex *out, int n);
private:
	static const gr_complex POLARITY[1680];
};

} /* namespace channel_estimation */
} /* namespace ieee802_11 */
} /* namespace gr */

#endif /* INCLUDED_IEEE802_11_EQUALIZER_LINEAR_COMB_H */

