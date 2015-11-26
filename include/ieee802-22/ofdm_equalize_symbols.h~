
#ifndef INCLUDED_IEEE802_22_OFDM_EQUALIZE_SYMBOLS_H
#define INCLUDED_IEEE802_22_OFDM_EQUALIZE_SYMBOLS_H

#include <ieee802-22/api.h>
#include <gnuradio/block.h>

enum Equalizer {
	LMS          = 0,
	LINEAR_COMB  = 1,
};

namespace gr {
namespace ieee802_22 {

class IEEE802_22_API ofdm_equalize_symbols : virtual public block
{
public:

	typedef boost::shared_ptr<ofdm_equalize_symbols> sptr;
	static sptr make(Equalizer algo, bool debug = false);
	virtual void set_algorithm(Equalizer algo) = 0;

};

}  // namespace ieee802_22
}  // namespace gr

#endif /* INCLUDED_IEEE802_22_OFDM_EQUALIZE_SYMBOLS_H*/

