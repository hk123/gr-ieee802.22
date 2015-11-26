
#ifndef INCLUDED_IEEE802_22_OFDM_PARSE_MAC_H
#define INCLUDED_IEEE802_22_OFDM_PARSE_MAC_H

#include <ieee802-22/api.h>
#include <gnuradio/block.h>

namespace gr {
namespace ieee802_22 {

class IEEE802_22_API ofdm_parse_mac : virtual public block
{
public:

	typedef boost::shared_ptr<ofdm_parse_mac> sptr;
	static sptr make(bool log = false, bool debug = false);

};

}  // namespace ieee802_22
}  // namespace gr

#endif /* INCLUDED_IEEE802_22_OFDM_PARSE_MAC_H */
