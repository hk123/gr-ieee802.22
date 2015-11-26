
#ifndef INCLUDED_IEEE802_22_OFDM_SYNC_SHORT_H
#define INCLUDED_IEEE802_22_OFDM_SYNC_SHORT_H

#include <ieee802-22/api.h>
#include <gnuradio/block.h>

namespace gr {
namespace ieee802_22 {

class IEEE802_22_API ofdm_sync_short : virtual public block
{
public:

	typedef boost::shared_ptr<ofdm_sync_short> sptr;
	static sptr make(double threshold, unsigned int min_plateau, bool log = false, bool debug = false);

};

}  // namespace ieee802_22
}  // namespace gr

#endif /* INCLUDED_IEEE802_22_OFDM_SYNC_SHORT_H */
