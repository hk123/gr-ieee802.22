
#ifndef INCLUDED_IEEE802_22_ETHER_ENCAP_IMPL_H
#define INCLUDED_IEEE802_22_ETHER_ENCAP_IMPL_H

#include <ieee802-22/ether_encap.h>

namespace gr {
namespace ieee802_22 {

	struct ethernet_header {
		uint8_t   dest[6];
		uint8_t   src[6];
		uint16_t  type;
	}__attribute__((packed));

	class ether_encap_impl : public ether_encap {

		public:
			ether_encap_impl(bool debug);

		private:
			void from_tap(pmt::pmt_t msg);
			void from_wifi(pmt::pmt_t msg);

			bool d_debug;
			uint16_t d_last_seq;
	};

} // namespace ieee802_22
} // namespace gr

#endif /* INCLUDED_IEEE802_22_ETHER_ENCAP_IMPL_H */

