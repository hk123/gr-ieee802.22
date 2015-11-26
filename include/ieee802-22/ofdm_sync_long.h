
#ifndef INCLUDED_IEEE802_22_OFDM_SYNC_LONG_H
#define INCLUDED_IEEE802_22_OFDM_SYNC_LONG_H

#include <ieee802-22/api.h>
#include <gnuradio/block.h>

namespace gr {
namespace ieee802_22 {

class IEEE802_22_API ofdm_sync_long : virtual public block
{
public:

	typedef boost::shared_ptr<ofdm_sync_long> sptr;
	static sptr make(unsigned int sync_length,
			bool log = false, bool debug = false);

};

}  // namespace ieee802_22
}  // namespace gr

#endif /* INCLUDED_IEEE802_22_OFDM_SYNC_LONG_H */
