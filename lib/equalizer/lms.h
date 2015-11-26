

#ifndef INCLUDED_IEEE802_22_EQUALIZER_LMS_H
#define INCLUDED_IEEE802_22_EQUALIZER_LMS_H

#include "base.h"
#include <vector>

namespace gr {
namespace ieee802_22 {
namespace equalizer {

class lms: public base {
public:
	virtual void equalize(const gr_complex *in, gr_complex *out, int n);
private:
	gr_complex carrier[2048];
	static const gr_complex ref[64];
	static const gr_complex POLARITY[127];
};

} /* namespace channel_estimation */
} /* namespace ieee802_22 */
} /* namespace gr */

#endif /* INCLUDED_IEEE802_22_EQUALIZER_LMS_H */

