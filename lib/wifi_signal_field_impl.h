

#ifndef INCLUDED_IEEE802_22_WIFI_SIGNAL_FIELD_IMPL_H
#define INCLUDED_IEEE802_22_WIFI_SIGNAL_FIELD_IMPL_H

#include <ieee802-22/wifi_signal_field.h>
#include "utils.h"

namespace gr {
namespace ieee802_22 {

class wifi_signal_field_impl : public wifi_signal_field
{
public:
	wifi_signal_field_impl();
	~wifi_signal_field_impl();

	bool header_formatter(long packet_len, unsigned char *out,
			const std::vector<tag_t> &tags);

	bool header_parser(const unsigned char *header,
			std::vector<tag_t> &tags);
private:
	int get_bit(int b, int i);
	void generate_signal_field(char *out, tx_param &tx, ofdm_param &ofdm);
};

} // namespace ieee802_22
} // namespace gr

#endif /* INCLUDED_IEEE802_22_WIFI_SIGNAL_FIELD_IMPL_H */

