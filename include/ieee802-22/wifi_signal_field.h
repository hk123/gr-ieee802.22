

#ifndef INCLUDED_IEEE802_22_WIFI_SIGNAL_FIELD_H
#define INCLUDED_IEEE802_22_WIFI_SIGNAL_FIELD_H

#include <ieee802-22/api.h>
#include <gnuradio/digital/packet_header_default.h>

namespace gr {
namespace ieee802_22 {

class IEEE802_22_API wifi_signal_field : virtual public digital::packet_header_default
{
public:
	typedef boost::shared_ptr<wifi_signal_field> sptr;
	static sptr make();

protected:
	wifi_signal_field();
};

} // namespace ieee802_22
} // namespace gr

#endif /* INCLUDED_IEEE802_22_WIFI_SIGNAL_FIELD_H */

