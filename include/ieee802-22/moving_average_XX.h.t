
#ifndef @GUARD_NAME@
#define @GUARD_NAME@

#include <ieee802-22/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace ieee802_22 {

class IEEE802_22_API @NAME@ : virtual public sync_block
{
public: 
	typedef boost::shared_ptr<@NAME@> sptr;
	static sptr make(int length);

	virtual int length() const = 0;
	virtual void set_length(int length) = 0;

};

} /* namespace blocks */
} /* namespace gr */

#endif /* @GUARD_NAME@ */
