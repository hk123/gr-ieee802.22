
#ifndef @GUARD_NAME_IMPL@
#define @GUARD_NAME_IMPL@

#include <ieee802-22/@NAME@.h>

namespace gr {
namespace ieee802_22 {

class @NAME_IMPL@ : public @NAME@
{
private:
	int d_length;
	int d_new_length;
	bool d_updated;

public:
	@NAME_IMPL@(int length);

	int length() const { return d_new_length; }
	void set_length(int length);

	int work(int noutput_items,
		gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items);

};

} /* namespace blocks */
} /* namespace gr */

#endif /* @GUARD_NAME_IMPL@ */
