
#define IEEE802_22_API
#define DIGITAL_API

%include "gnuradio.i"

%include "ieee802_22_swig_doc.i"

%{
#include "gnuradio/digital/packet_header_default.h"

#include "ieee802-22/chunks_to_symbols.h"
#include "ieee802-22/ether_encap.h"
#include "ieee802-22/moving_average_cc.h"
#include "ieee802-22/moving_average_ff.h"
#include "ieee802-22/ofdm_decode_mac.h"
#include "ieee802-22/ofdm_decode_signal.h"
#include "ieee802-22/ofdm_equalize_symbols.h"
#include "ieee802-22/ofdm_mac.h"
#include "ieee802-22/ofdm_mapper.h"
#include "ieee802-22/ofdm_parse_mac.h"
#include "ieee802-22/ofdm_sync_long.h"
#include "ieee802-22/ofdm_sync_short.h"
#include "ieee802-22/wifi_signal_field.h"
%}

%include "gnuradio/digital/packet_header_default.h"

%include "ieee802-22/chunks_to_symbols.h"
%include "ieee802-22/ether_encap.h"
%include "ieee802-22/moving_average_cc.h"
%include "ieee802-22/moving_average_ff.h"
%include "ieee802-22/ofdm_decode_mac.h"
%include "ieee802-22/ofdm_decode_signal.h"
%include "ieee802-22/ofdm_equalize_symbols.h"
%include "ieee802-22/ofdm_mac.h"
%include "ieee802-22/ofdm_mapper.h"
%include "ieee802-22/ofdm_parse_mac.h"
%include "ieee802-22/ofdm_sync_long.h"
%include "ieee802-22/ofdm_sync_short.h"
%include "ieee802-22/wifi_signal_field.h"

GR_SWIG_BLOCK_MAGIC2(ieee802_22, chunks_to_symbols);
GR_SWIG_BLOCK_MAGIC2(ieee802_22, ether_encap);
GR_SWIG_BLOCK_MAGIC2(ieee802_22, moving_average_cc);
GR_SWIG_BLOCK_MAGIC2(ieee802_22, moving_average_ff);
GR_SWIG_BLOCK_MAGIC2(ieee802_22, ofdm_decode_mac);
GR_SWIG_BLOCK_MAGIC2(ieee802_22, ofdm_decode_signal);
GR_SWIG_BLOCK_MAGIC2(ieee802_22, ofdm_equalize_symbols);
GR_SWIG_BLOCK_MAGIC2(ieee802_22, ofdm_mac);
GR_SWIG_BLOCK_MAGIC2(ieee802_22, ofdm_mapper);
GR_SWIG_BLOCK_MAGIC2(ieee802_22, ofdm_parse_mac);
GR_SWIG_BLOCK_MAGIC2(ieee802_22, ofdm_sync_long);
GR_SWIG_BLOCK_MAGIC2(ieee802_22, ofdm_sync_short);

%template(wifi_signal_field_sptr) boost::shared_ptr<gr::ieee802_22::wifi_signal_field>;
%pythoncode %{
wifi_signal_field_sptr.__repr__ = lambda self: "<wifi_signal_field>"
wifi_signal_field = wifi_signal_field .make;
%}
