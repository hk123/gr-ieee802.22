
#ifndef INCLUDED_IEEE802_22_OFDM_MAC_H
#define INCLUDED_IEEE802_22_OFDM_MAC_H

#include <ieee802-22/api.h>
#include <gnuradio/block.h>

namespace gr {
namespace ieee802_22 {

class IEEE802_22_API ofdm_mac : virtual public block
{
public:

	typedef boost::shared_ptr<ofdm_mac> sptr;
	static sptr make(std::vector<uint8_t> src_mac,
			std::vector<uint8_t> dst_mac,
			std::vector<uint8_t> bss_mac);
};

}  // namespace ieee802_22
}  // namespace gr

#endif /* INCLUDED_IEEE802_22_OFDM_MAC_H */
