
#ifndef INCLUDED_IEEE802_22_ETHER_ENCAP_H
#define INCLUDED_IEEE802_22_ETHER_ENCAP_H

#include <ieee802-22/api.h>
#include <gnuradio/block.h>

namespace gr {
namespace ieee802_22 {

class IEEE802_22_API ether_encap : virtual public block
{
public:

	typedef boost::shared_ptr<ether_encap> sptr;
	static sptr make(bool debug);

};

}  // namespace ieee802_22
}  // namespace gr

#endif /* INCLUDED_IEEE802_22_ETHER_ENCAP_H */
