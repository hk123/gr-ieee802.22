
#ifndef INCLUDED_IEEE802_22_CHUNKS_TO_SYMBOLS_H
#define INCLUDED_IEEE802_22_CHUNKS_TO_SYMBOLS_H

#include <ieee802-22/api.h>
#include <gnuradio/tagged_stream_block.h>

namespace gr {
namespace ieee802_22 {
    
class IEEE802_22_API chunks_to_symbols : virtual public tagged_stream_block
{
public:
        typedef boost::shared_ptr<chunks_to_symbols> sptr;
        static sptr make();
};

} /* namespace ieee802_22 */
} /* namespace gr */

#endif /* INCLUDED_IEEE802_22_CHUNKS_TO_SYMBOLS_H */
