
#ifndef INCLUDED_IEEE802_22_OFDM_MAPPER_H
#define INCLUDED_IEEE802_22_OFDM_MAPPER_H

#include <ieee802-22/api.h>
#include <gnuradio/block.h>

enum Encoding {
	BPSK_1_2  = 0,
	BPSK_3_4  = 1,
	QPSK_1_2  = 2,
	QPSK_3_4  = 3,
	QAM16_1_2 = 4,
	QAM16_3_4 = 5,
	QAM64_2_3 = 6,
	QAM64_3_4 = 7,
};

namespace gr {
namespace ieee802_22 {

class IEEE802_22_API ofdm_mapper : virtual public block
{
public:
	
	typedef boost::shared_ptr<ofdm_mapper> sptr;
	static sptr make(Encoding mcs, bool debug = false);
	virtual void set_encoding(Encoding mcs) = 0;
};

}  // namespace ieee802_22
}  // namespace gr

#endif /* INCLUDED_IEEE802_22_OFDM_MAPPER_H */
