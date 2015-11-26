
#include <ieee802-22/ofdm_sync_long.h>
#include "utils.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/filter/fir_filter.h>
#include <gnuradio/fft/fft.h>

#include <list>
#include <tuple>

using namespace gr::ieee802_22;
using namespace std;


bool compare_abs(const std::pair<gr_complex, int>& first, const std::pair<gr_complex, int>& second) {
	return abs(get<0>(first)) > abs(get<0>(second));
}

class ofdm_sync_long_impl : public ofdm_sync_long {

public:
ofdm_sync_long_impl(unsigned int sync_length, bool log, bool debug) : block("ofdm_sync_long",
		gr::io_signature::make2(2, 2, sizeof(gr_complex), sizeof(gr_complex)),
		gr::io_signature::make(1, 1, sizeof(gr_complex))),
		d_fir(gr::filter::kernel::fir_filter_ccc(1, LONG)),
		d_log(log),
		d_debug(debug),
		d_offset(0),
		d_state(SYNC),
		SYNC_LENGTH(sync_length) {

	set_tag_propagation_policy(block::TPP_DONT);
	d_correlation = gr::fft::malloc_complex(8192);
}

~ofdm_sync_long_impl(){
	gr::fft::free(d_correlation);
}

int general_work (int noutput, gr_vector_int& ninput_items,
		gr_vector_const_void_star& input_items,
		gr_vector_void_star& output_items) {

	const gr_complex *in = (const gr_complex*)input_items[0];
	const gr_complex *in_delayed = (const gr_complex*)input_items[1];
	gr_complex *out = (gr_complex*)output_items[0];

	dout << "LONG ninput[0] " << ninput_items[0] << "   ninput[1] " <<
			ninput_items[1] << "  noutput " << noutput <<
			"   state " << d_state << "    nitem_read(0)" << nitems_read(0) << std::endl;

	int ninput = std::min(std::min(ninput_items[0], ninput_items[1]), 8192);

	const uint64_t nread = nitems_read(0);
	get_tags_in_range(d_tags, 0, nread, nread + ninput);
	if (d_tags.size()) {
		std::sort(d_tags.begin(), d_tags.end(), gr::tag_t::offset_compare);

		const uint64_t offset = d_tags.front().offset;
		dout << "offset" << offset << "     ninput" << ninput << std::endl;
		if(offset > nread) {
			ninput = offset - nread;
		} else {
			if(d_offset && (d_state == SYNC)) {
				throw std::runtime_error("wtf");
			}
			if(d_state == COPY) {
				d_state = RESET;
			}
		}
	}


	int i = 0;
	int o = 0;

	switch(d_state) {

	case SYNC:
		d_fir.filterN(d_correlation, in, std::min(SYNC_LENGTH, std::max(ninput - 1023, 0)));

		while(i + 1023 < ninput) {

			d_cor.push_back(pair<gr_complex, int>(d_correlation[i], d_offset));

			i++;
			d_offset++;

			if(d_offset == SYNC_LENGTH) {
				search_frame_start();
				d_offset = 0;
				d_count = 0;
				d_state = COPY;

				break;
			}
		}

		break;

	case COPY:
		while(i < ninput && o < noutput) {

			int rel = d_offset - d_frame_start;

			if(!rel)  {
				add_item_tag(0, nitems_written(0),
					pmt::string_to_symbol("ofdm_start"),
					pmt::PMT_T,
					pmt::string_to_symbol(name()));
			}

			if(rel >= 0 && (rel < 2048 || ((rel - 2048) % 2560) > 511)) {
				out[o] = in_delayed[i] * exp(gr_complex(0, d_offset * d_freq_offset));
				o++;
			}

			i++;
			d_offset++;
		}

		break;

	case RESET: {
		while(o < noutput) {
			if(((d_count + o) % 2048) == 0) {
				d_offset = 0;
				d_state = SYNC;
				break;
			} else {
				out[o] = 0;
				o++;
			}
		}

		break;
	}
	}

	dout << "produced : " << o << " consumed: " << i << std::endl;

	d_count += o;
	consume(0, i);
	consume(1, i);
	return o;
}

void forecast (int noutput_items, gr_vector_int &ninput_items_required) {

	// in sync state we need at least a symbol to correlate
	// with the pattern
	if(d_state == SYNC) {
		ninput_items_required[0] = 1024;
		ninput_items_required[1] = 1024;

	} else {
		ninput_items_required[0] = noutput_items;
		ninput_items_required[1] = noutput_items;
	}
}

void search_frame_start() {

	// sort list (highest correlation first)
	assert(d_cor.size() == SYNC_LENGTH);
	d_cor.sort(compare_abs);

	// copy list in vector for nicer access
	vector<pair<gr_complex, int> > vec(d_cor.begin(), d_cor.end());
	d_cor.clear();

	// in case we don't find anything use SYNC_LENGTH
	d_frame_start = SYNC_LENGTH;

	for(int i = 0; i < 3; i++) {
		for(int k = i + 1; k < 4; k++) {
			gr_complex first;
			gr_complex second;
			if(get<1>(vec[i]) > get<1>(vec[k])) {
				first = get<0>(vec[k]);
				second = get<0>(vec[i]);
			} else {
				first = get<0>(vec[i]);
				second = get<0>(vec[k]);
			}
			int diff  = abs(get<1>(vec[i]) - get<1>(vec[k]));
			if(diff == 1024) {
				d_frame_start =  max(get<1>(vec[i]), get<1>(vec[k])) + 1024 - 2048 - 1;
				d_freq_offset = arg(first * conj(second)) / 1024;
				//std::cout << "frequency offset " << d_freq_offset << std::endl;
				// nice match found, return immediately
				return;

			// TODO: check if these offsets make sense
			} else if(diff == 1023) {
				d_frame_start = max(get<1>(vec[i]), get<1>(vec[k])) + 1023 - 2048 - 1;
				d_freq_offset = arg(first * conj(second)) / 1024;
			} else if(diff == 1025) {
				d_frame_start = max(get<1>(vec[i]), get<1>(vec[k])) + 1024 - 2048 - 1;
				d_freq_offset = arg(first * conj(second)) / 1024;
			}
		}
	}
}

private:
	enum {SYNC, COPY, RESET} d_state;
	int         d_count;
	int         d_offset;
	int         d_frame_start;
	float       d_freq_offset;

	gr_complex *d_correlation;
	list<pair<gr_complex, int> > d_cor;
	std::vector<gr::tag_t> d_tags;
	gr::filter::kernel::fir_filter_ccc d_fir;

	const bool d_log;
	const bool d_debug;
	const int  SYNC_LENGTH;

	static const std::vector<gr_complex> LONG;
};

ofdm_sync_long::sptr
ofdm_sync_long::make(unsigned int sync_length, bool log, bool debug) {
	return gnuradio::get_initial_sptr(new ofdm_sync_long_impl(sync_length, log, debug));
}

const std::vector<gr_complex> ofdm_sync_long_impl::LONG = {

gr_complex(-14.1421,   0.0000), gr_complex(  6.7335, -33.8111), gr_complex( 13.2063, -53.4774), gr_complex(-9.6213, -17.0889), 
gr_complex( 17.1061,  16.5385), gr_complex( 34.9364,  50.8340), gr_complex( 29.6668,  22.6597), gr_complex( 24.5492, -53.1897), 
gr_complex(-18.2095, -24.8050), gr_complex( 12.0787,  18.6956), gr_complex( 24.8585, -0.4655), gr_complex(-3.2273,  15.0270), 
gr_complex( 31.9900,   7.9842), gr_complex( -2.7032, -56.6902), gr_complex( 3.7144, -34.5898), gr_complex( 54.9472, -17.6738), 
gr_complex( 23.2400, -30.3293), gr_complex( -6.1963,  35.4362), gr_complex(-48.8560,  0.0831), gr_complex(-37.6590, -11.3282), 
gr_complex(-24.9288,  59.5845), gr_complex(-35.9171, -5.3922), gr_complex( 29.2519, -9.3508), gr_complex( 12.1814,  27.3961), 
gr_complex(-22.6444, -25.0785), gr_complex(-22.3324, -47.3541), gr_complex(-37.7791, -12.7393), gr_complex( 25.5951,  28.5378), 
gr_complex( 31.7174, -38.8620), gr_complex( 12.0102, -39.8786), gr_complex( 51.1676,  28.2852), gr_complex( 31.6344, -10.2578), 
gr_complex( 34.6012,  31.5539), gr_complex( 40.8833,  21.9067), gr_complex( 2.4173, -62.2882), gr_complex( 14.5016, -21.4031), 
gr_complex( 22.6635,   1.3479), gr_complex( 12.9059, -9.4269), gr_complex(-1.1290,  6.1686), gr_complex(-29.5171,  43.2803), 
gr_complex(  4.4287,  17.8897), gr_complex(  2.9082, -25.5641), gr_complex(-43.2535, -1.7997), gr_complex( 16.9710, -33.5992), 
gr_complex( 52.3476,  10.2955), gr_complex(  6.5156,  44.8948), gr_complex(-33.5577, -11.3357), gr_complex(-35.3063,  36.4838), 
gr_complex( 33.9272,  25.3183), gr_complex( 46.8780,  21.7099), gr_complex( 0.2753,  34.4171), gr_complex(-27.7844, -17.4159), 
gr_complex(-17.5292,  31.2500), gr_complex( 47.4222,  12.2084), gr_complex( 46.6816, -4.9973), gr_complex( 43.5484,  11.3895), 
gr_complex( 31.3117, -35.3378), gr_complex( -8.4840,  36.2026), gr_complex( 51.5558,  14.7858), gr_complex( 15.1778, -49.1050), 
gr_complex(-30.4105,  25.1740), gr_complex( 15.6590,  29.4556), gr_complex(-24.0762,  37.1278), gr_complex(-12.3070,  28.6640), 
gr_complex( 13.4029, -19.2935), gr_complex(  7.5731,  11.9839), gr_complex( 11.9540, -9.8185), gr_complex(-27.8417, -60.6073), 
gr_complex(-31.3816, -17.6516), gr_complex(-32.7234,  13.9549), gr_complex( 6.9695, -38.8870), gr_complex( 24.3218, -37.1536), 
gr_complex(-20.4424,  23.0560), gr_complex( 44.7268,  8.9846), gr_complex( 45.0654, -3.2759), gr_complex(-20.7015, -21.0977), 
gr_complex(-20.9055, -44.9346), gr_complex( -4.6966,  30.4904), gr_complex( 42.3736,  42.4614), gr_complex( 44.3468, -19.8677), 
gr_complex( 47.3360,  10.2432), gr_complex( 29.5769,  11.3787), gr_complex(-22.4513, -13.0501), gr_complex(-9.6209,  32.1999), 
gr_complex(-29.4350,   5.7862), gr_complex(  2.9164, -31.3969), gr_complex( 61.9953, -11.6406), gr_complex( 11.3919, -22.2582), 
gr_complex(-20.1319,   9.4100), gr_complex(-13.7227, -17.4394), gr_complex(-17.9522, -61.9569), gr_complex(-30.6739,  32.2419), 
gr_complex( 10.6058,  30.1034), gr_complex( 47.2462, -22.1051), gr_complex( 19.9931,  13.6631), gr_complex( 55.8439,  9.5930), 
gr_complex( 42.9282,  -9.1997), gr_complex(-10.3430, -25.5998), gr_complex( 33.9038,  2.9807), gr_complex( 37.5130,  43.3023), 
gr_complex(-32.6334,  14.7756), gr_complex(-49.0947,  1.6593), gr_complex( 33.2832, -8.3816), gr_complex( 57.2123, -11.0440), 
gr_complex(  1.2086, -26.9820), gr_complex(-20.9716, -47.5315), gr_complex(-39.0731, -17.3392), gr_complex(-11.0549, -51.8106), 
gr_complex(  3.9123, -43.0284), gr_complex( 13.6906,  30.2231), gr_complex( 53.2855, -5.7386), gr_complex( 24.8746, -33.1906), 
gr_complex( 42.4425, -29.9560), gr_complex( 24.6986, -38.2455), gr_complex(-39.8017,  2.2605), gr_complex(-15.4160,  44.9581), 
gr_complex(-33.1708,  33.6719), gr_complex(-30.5067,  8.1884), gr_complex(-25.3898,  33.0105), gr_complex(-31.3252,  12.5277), 
gr_complex( 27.2286,  -2.5828), gr_complex( 13.7900,  52.6291), gr_complex( 30.4301, -15.3432), gr_complex( 29.8424, -44.6852), 
gr_complex(-20.6870,  32.9948), gr_complex( 41.6700,  16.8574), gr_complex(-3.2784,  32.24), gr_complex(-47.64,  26.2085), 
gr_complex( 37.5147, -21.3137), gr_complex(-25.8804,  11.4333), gr_complex(-54.0578, -18.9555), gr_complex( 29.9037, -21.7934), 
gr_complex( 12.6306,  29.2741), gr_complex( 23.9742,  4.5216), gr_complex( 27.7447,  23.3451), gr_complex(-58.2406,  29.8352), 
gr_complex(-28.2541,   6.5822), gr_complex( 17.2738,  46.70), gr_complex(-12.2140,  34.7116), gr_complex( 30.1715, -13.21), 
gr_complex( 32.7883, -36.0434), gr_complex( 16.5154, -45.9087), gr_complex( 40.2803, -3.55), gr_complex(-20.7518,  17.1097), 
gr_complex(-34.3277, -19.0841), gr_complex(-11.6841,  14.1012), gr_complex(-32.2768,  45.8658), gr_complex( 20.0338, -26.8776), 
gr_complex( 17.4169, -29.9404), gr_complex(-44.7205,  19.9987), gr_complex(-20.1302,  26.93), gr_complex(-4.7851,  42.8433), 
gr_complex(-15.2300, -18.8426), gr_complex( -1.2678, -38.1221), gr_complex(-0.5443,  21.7096), gr_complex(-6.3241, -16.8246), 
gr_complex(  3.2322,   7.4273), gr_complex(  3.2091,  69.0316), gr_complex(-21.9908,  22.4588), gr_complex(-33.4891, -13.4340), 
gr_complex(-41.3893,   2.3351), gr_complex(-40.4836, -3.1583), gr_complex(-0.5541, -47.4949), gr_complex(-20.6882,  11.5067), 
gr_complex(-46.7427,  37.7565), gr_complex( 24.8160, -31.0687), gr_complex( 33.9166,  28.3683), gr_complex(-22.4517, -11.8792), 
gr_complex(-31.3839, -50.5711), gr_complex( -6.0816,  58.9932), gr_complex( 48.1554,  4.8044), gr_complex( 34.1488, -18.9639), 
gr_complex(-25.6238,  10.6487), gr_complex(-24.9547, -13.4961), gr_complex(-25.6429,  20.7707), gr_complex( 2.1291, -21.4208), 
gr_complex( 39.7693, -16.5485), gr_complex( -0.7103,  44.8443), gr_complex(-40.2774,  9.5243), gr_complex(-38.2815, -34.3346), 
gr_complex(-20.7336, -34.4043), gr_complex(-13.1471,  35.7387), gr_complex(-19.1634,  16.83), gr_complex(-17.8312, -41.1302), 
gr_complex(-11.0972,  12.3806), gr_complex( 18.7075,  35.1246), gr_complex( 24.9250,  34.8020), gr_complex(-16.9250, -16.7015), 
gr_complex(-33.0663, -46.0294), gr_complex(-30.1320,  17.0900), gr_complex(-40.0199, -10.5151), gr_complex(-25.4645, -48.7292), 
gr_complex(  5.0984, -26.8880), gr_complex(-20.2423, -14.8198), gr_complex(-48.1922, -17.5114), gr_complex(-8.4778,  18.8332), 
gr_complex( 12.2627,  51.8733), gr_complex(-10.0845, -21.2213), gr_complex(-35.6863,  7.6493), gr_complex(-29.7037,  53.9547), 
gr_complex(  4.5756, -43.0365), gr_complex(  0.4836, -45.3278), gr_complex( 27.6705, -3.8687), gr_complex( 55.5825,  14.7016), 
gr_complex(-25.8366,   1.2615), gr_complex(-47.2843, -39.3100), gr_complex( 6.7608, -11.0982), gr_complex(-24.1891, -23.4879), 
gr_complex(-30.5835, -12.4477), gr_complex( 18.5710,  11.1825), gr_complex(-13.4554, -11.8724), gr_complex(-30.6360,  3.1928), 
gr_complex( 13.9331, -18.5679), gr_complex( -1.9342,  19.4722), gr_complex( 14.6244,  34.1181), gr_complex( 58.1191, -20.2440), 
gr_complex( -3.5567,  28.6568), gr_complex(-18.0882, -9.5361), gr_complex( 18.7855, -57.5542), gr_complex(-16.72, -24.6383), 
gr_complex(-11.2868, -47.1945), gr_complex( 22.6092, -42.1995), gr_complex( 21.3722, -36.8309), gr_complex( 41.74, -20.4243), 
gr_complex( 47.7077,   1.4493), gr_complex( 26.7838,  21.8243), gr_complex( 28.3928,  41.0479), gr_complex( 38.1501, -38.2487), 
gr_complex( 12.9923, -41.6887), gr_complex(-16.0784, -20.8598), gr_complex( 0.3334, -41.5955), gr_complex( 10.1937,  29.5191), 
gr_complex( 22.2187,  18.9600), gr_complex( 44.2554,  17.5916), gr_complex(-5.0956,  55.7216), gr_complex(-45.3417,  9.2360), 
gr_complex( -4.2704,  43.7330), gr_complex( 18.7085,  45.1226), gr_complex(-17.3090, -14.1409), gr_complex(-34.8473,  13.6728), 
gr_complex( -6.7393,  51.5606), gr_complex(-10.1481,  46.7047), gr_complex(-25.2266, -17.7156), gr_complex(-27.2286, -39.0674), 
gr_complex(-41.4364,  -0.0916), gr_complex(-39.0190,  2.5814), gr_complex(-48.9638, -10.30), gr_complex(-11.0211, -27.5284), 
gr_complex( 29.0933,   4.5311), gr_complex(-23.3493,  14.7344), gr_complex( 11.6136, -18.5215), gr_complex( 24.3038,  33.8823), 
gr_complex(-55.0863,  40.2730), gr_complex(-23.2619, -28.2193), gr_complex(-15.2766, -40.4937), gr_complex(-34.2629, -1.65), 
gr_complex(-22.6274,  42.4264), gr_complex(-41.8339,  32.8953), gr_complex( 29.8322,  9.9825), gr_complex( 56.1437,  12.1429), 
gr_complex(  6.5266,  31.8582), gr_complex( 42.5946,  13.4887), gr_complex( 5.3416, -55.1929), gr_complex(-40.1337, -5.9558), 
gr_complex( -2.6236,  47.8489), gr_complex(-29.7795,  1.9042), gr_complex( 11.7132,  16.8582), gr_complex( 56.4808,  21.7897), 
gr_complex(-32.4243,  11.0125), gr_complex( 10.6658,  5.5921), gr_complex( 57.1296, -22.4879), gr_complex(-20.7089,  18.5978), 
gr_complex( 22.3553,   9.6290), gr_complex( 28.4565, -43.0428), gr_complex(-11.4557, -41.9426), gr_complex( 25.9990, -45.5162), 
gr_complex( -5.4747,  12.6251), gr_complex( -6.0352,  39.7013), gr_complex( 3.4706, -23.0548), gr_complex(-13.4188, -32.9561), 
gr_complex( 22.6610,  -7.0518), gr_complex( -4.2353,  22.6182), gr_complex( 14.4476,  24.85), gr_complex( 48.1461,  25.7835), 
gr_complex(-30.9775,  40.5677), gr_complex(-32.3019, -20.1952), gr_complex( 23.7648, -30.2122), gr_complex( 15.5140,  36.16), 
gr_complex(-32.0664,  40.0447), gr_complex(-30.2729,  33.9688), gr_complex( 22.6275,  34.6144), gr_complex(-6.2921,  33.9941), 
gr_complex(-39.4132,  32.6055), gr_complex( 15.4908,  6.8648), gr_complex( 55.9214,  16.8405), gr_complex( 38.7670,  23.5266), 
gr_complex( 40.6617,  -7.2988), gr_complex( 46.8889, -26.2515), gr_complex(-7.1174, -47.9634), gr_complex( 5.0887, -46.1233), 
gr_complex( 34.6400, -17.7032), gr_complex(-17.5507,  6.8409), gr_complex( 6.4883, -9.3192), gr_complex( 30.3661, -55.3401), 
gr_complex(-24.3449,   3.5606), gr_complex( -8.8745,  41.1579), gr_complex( 39.7323, -24.7340), gr_complex( 10.3432,  9.2159), 
gr_complex(-27.1527,  35.7211), gr_complex(  8.3333,  15.3921), gr_complex( 44.5323,  30.7065), gr_complex( 27.6310, -31.4239), 
gr_complex(-20.8488, -51.0701), gr_complex(-21.0790, -29.4934), gr_complex( 5.6138, -52.9996), gr_complex(-27.1327,  8.5339), 
gr_complex( 18.9735,  34.6314), gr_complex( 49.4294, -19.9102), gr_complex(-23.0773, -5.9516), gr_complex( 5.4210,  30.2029), 
gr_complex( -0.1278,  23.4551), gr_complex(-55.6916, -4.8205), gr_complex(-23.3311, -33.7915), gr_complex( 12.5660, -51.4033), 
gr_complex( 34.5222,  -2.9487), gr_complex(  9.4303,  42.9571), gr_complex(-3.1773, -11.5186), gr_complex( 3.8310, -51.8353), 
gr_complex( 12.2462, -20.3287), gr_complex( 43.9983,  33.3869), gr_complex(-29.8429,  6.4599), gr_complex(-17.6112, -54.0930), 
gr_complex( 57.3567,  -2.7043), gr_complex(-20.7826, -4.2853), gr_complex(-29.0673, -41.5484), gr_complex(-32.6090, -6.3896), 
gr_complex(-34.4382, -24.2628), gr_complex( 43.7905, -16.5606), gr_complex(-12.7956,  16.1726), gr_complex(-53.2283,  8.4893), 
gr_complex(-33.5756,  11.0829), gr_complex(-51.0681,  4.9325), gr_complex(-34.9892,  8.6674), gr_complex(-36.1984, -8.6573), 
gr_complex( 21.9623, -14.1237), gr_complex( 47.2653,  3.02), gr_complex(-31.8899, -26.7796), gr_complex( 9.9180, -8.50), 
gr_complex( 22.6394,  25.1612), gr_complex(-55.4692,  8.8372), gr_complex( 7.9867, -17.5321), gr_complex( 63.6291, -23.7323), 
gr_complex( -3.4328,  40.0693), gr_complex(-25.3944,  41.7377), gr_complex(-15.0906, -28.6443), gr_complex(-13.9816,  4.8417), 
gr_complex( 20.0411,  40.6106), gr_complex(-3.1728, -9.9540), gr_complex(-43.3895,  1.2352), gr_complex(-8.6269,  60.1274), 
gr_complex(-19.5720,  17.9734), gr_complex(-47.41, -29.8143), gr_complex(-0.3520,  15.3224), gr_complex( 37.2224,  26.7576), 
gr_complex(  3.4814, -29.4358), gr_complex(-48.4816, -27.7580), gr_complex( 3.0895,  25.9378), gr_complex( 61.6521, -15.0146), 
gr_complex( 14.7544, -44.4683), gr_complex(-4.5314,  12.8632), gr_complex(-21.8964, -4.25), gr_complex(-34.6799, -2.0563), 
gr_complex( 31.3570,  24.4599), gr_complex( 48.2715, -21.0667), gr_complex(-1.9537, -11.3235), gr_complex(-50.5988,  18.6639), 
gr_complex(-30.5998,  -6.3529), gr_complex( 7.2286, -48.4315), gr_complex(-33.8249, -33.8702), gr_complex( 13.8973,  8.5932), 
gr_complex( 57.7431, -15.2027), gr_complex(-6.0426,  15.6596), gr_complex( 23.2314,  40.97), gr_complex( 1.4630, -15.8138), 
gr_complex(-54.4853,   1.3137), gr_complex( 18.1276,  29.6833), gr_complex( 33.6777,  25.9064), gr_complex( 14.6380,  6.6734), 
gr_complex( -7.5773, -32.2383), gr_complex(-54.8298, -4.1242), gr_complex(-26.1947,  22.4388), gr_complex(-26.4713,  6.9454), 
gr_complex(-42.9014,  25.8804), gr_complex(-13.4147,  37.6413), gr_complex( 5.4874,  9.9256), gr_complex( 54.7755, -1.1716), 
gr_complex( 29.2865,  -7.8962), gr_complex(-27.7055, -8.7722), gr_complex( 25.3309,  21.6355), gr_complex( 0.8619,  2.5942), 
gr_complex(-59.2154, -11.2964), gr_complex(-31.6208, -3.2643), gr_complex(-44.3979, -34.1093), gr_complex(-38.3468,  7.5573), 
gr_complex( 15.1484,  25.5827), gr_complex( 3.3653, -6.4175), gr_complex(-33.3817,  37.6614), gr_complex(-4.2319,  47.83), 
gr_complex( 28.4111,   7.6077), gr_complex(-0.9616, -42.1077), gr_complex( 24.6993, -29.9795), gr_complex( 32.1525,  42.6817), 
gr_complex(-28.3344,  28.7069), gr_complex( 9.1723,  15.3642), gr_complex( 38.6247,  28.0221), gr_complex( 23.0984,  5.6126), 
gr_complex( -0.8512, -32.1518), gr_complex(-39.2168, -46.12), gr_complex(-4.5574,  23.2225), gr_complex(-5.3724, -0.4467), 
gr_complex( 11.2949, -57.7560), gr_complex( 26.9060,  25.85), gr_complex(-27.5923,  3.9687), gr_complex( 32.52, -58.6526), 
gr_complex( 11.3128,  24.5790), gr_complex(-32.9141,  50.45), gr_complex( 37.8993,  4.0715), gr_complex(-7.8840,  1.1823), 
gr_complex(-12.5794, -11.7159), gr_complex(-0.8367, -18.3541), gr_complex(-26.4552, -15.7514), gr_complex( 2.0498, -28.7805), 
gr_complex(-42.2983,  24.8540), gr_complex(-30.6516,  29.8721), gr_complex( 6.0743, -49.3245), gr_complex(-2.7640, -30.2720), 
gr_complex( 12.5391,   0.8389), gr_complex( 19.4065,  2.2803), gr_complex( 53.9902,  2.73), gr_complex(-8.3276, -14.7914), 
gr_complex(-42.1675,  17.8307), gr_complex( 18.0984, -9.7533), gr_complex(-17.9876, -37.86), gr_complex(-5.8964,  20.8074), 
gr_complex(  4.3510,  -2.0562), gr_complex( 13.2167, -16.3248), gr_complex( 56.4308,  23.6739), gr_complex( 7.3455,  25.3407), 
gr_complex( 15.5677,  42.3633), gr_complex(-7.5444,  33.2079), gr_complex(-59.5168, -5.5254), gr_complex( 12.0953, -8.8521), 
gr_complex(  9.8774, -17.2181), gr_complex(-54.8927,  3.0417), gr_complex(-39.80,  38.22), gr_complex(-4.2165, -24.2354), 
gr_complex(-19.3000, -45.2841), gr_complex(-1.0123,  18.9973), gr_complex(-1.5754,  20.2673), gr_complex(-62.1141,  14.9145), 
gr_complex(  9.4235,  -9.5403), gr_complex( 32.3332,  4.6225), gr_complex(-42.3165,  46.0488), gr_complex(-9.7450, -42.8434), 
gr_complex( -5.3675,  -6.9758), gr_complex(-24.7421,  64.8109), gr_complex(-25.5536, -18.9899), gr_complex(-7.6481,  11.6796), 
gr_complex( 42.5749,  19.3173), gr_complex( 25.1315, -46.4452), gr_complex( 3.99, -2.0216), gr_complex( 17.3048,  55.1896), 
gr_complex( 26.7387,  44.8491), gr_complex( 15.4765, -17.8605), gr_complex( 5.5581,  4.7598), gr_complex( 10.5202,  10.1633), 
gr_complex( 11.6457, -60.0902), gr_complex( 41.2667, -9.2896), gr_complex( 8.0678, -5.3437), gr_complex( 8.7639, -40.0465), 
gr_complex( 43.0123, -13.1644), gr_complex(-17.9668, -55.2110), gr_complex( 27.7396, -38.3012), gr_complex( 19.1797,  21.2762), 
gr_complex(-48.5727,  26.4880), gr_complex(-10.2810,  41.1278), gr_complex(-40.9851,  14.1724), gr_complex( 9.2942, -17.8887), 
gr_complex( 15.6605,  -1.5292), gr_complex(-67.5401, -3.1747), gr_complex(-9.5783, -27.2752), gr_complex(-3.6366, -45.6788), 
gr_complex(  8.9248, -49.1515), gr_complex( 2.6560, -5.8346), gr_complex(-36.9630,  42.8946), gr_complex( 33.8686,  9.8380), 
gr_complex( 13.7278, -53.5896), gr_complex(-0.4024, -50.8398), gr_complex(-0.6552,  5.8180), gr_complex(-43.5309,  8.9508), 
gr_complex( 21.9561, -35.1153), gr_complex( 3.7675, -24.2720), gr_complex(-48.9648,  10.6554), gr_complex(-26.7754,  33.02), 
gr_complex(-41.2658,  -1.4856), gr_complex(-16.7897, -49.5367), gr_complex( 25.9871, -23.3058), gr_complex( 46.3064, -14.7260), 
gr_complex( 31.3438,   5.2600), gr_complex(-13.5780,  13.5944), gr_complex( 16.5052, -27.5729), gr_complex( 42.7702, -2.2447), 
gr_complex( 25.4559,   0.0000), gr_complex( 42.7702,  2.2447), gr_complex( 16.5052,  27.5729), gr_complex(-13.5780, -13.5944), 
gr_complex( 31.3438,  -5.2600), gr_complex( 46.3064,  14.7260), gr_complex( 25.9871,  23.3058), gr_complex(-16.7897,  49.5367), 
gr_complex(-41.2658,   1.4856), gr_complex(-26.7754, -33.02), gr_complex(-48.9648, -10.6554), gr_complex( 3.7675,  24.2720), 
gr_complex( 21.9561,  35.1153), gr_complex(-43.5309, -8.9508), gr_complex(-0.6552, -5.8180), gr_complex(-0.4024,  50.8398), 
gr_complex( 13.7278,  53.5897), gr_complex( 33.8686, -9.8380), gr_complex(-36.9630, -42.8946), gr_complex( 2.6560,  5.8346), 
gr_complex(  8.9248,  49.1515), gr_complex(-3.6366,  45.6788), gr_complex(-9.5783,  27.2752), gr_complex(-67.5401,  3.1747), 
gr_complex( 15.6605,   1.5292), gr_complex( 9.2942,  17.8887), gr_complex(-40.9851, -14.1724), gr_complex(-10.2810, -41.1278), 
gr_complex(-48.5727, -26.4880), gr_complex( 19.1797, -21.2762), gr_complex( 27.7396,  38.3012), gr_complex(-17.9668,  55.2110), 
gr_complex( 43.0123,  13.1644), gr_complex( 8.7639,  40.0465), gr_complex( 8.0678,  5.3437), gr_complex( 41.2667,  9.2896), 
gr_complex( 11.6457,  60.0902), gr_complex( 10.5202, -10.1633), gr_complex( 5.5581, -4.7598), gr_complex( 15.4765,  17.8605), 
gr_complex( 26.7387, -44.8491), gr_complex( 17.3048, -55.1896), gr_complex( 3.99,  2.0216), gr_complex( 25.1315,  46.4452), 
gr_complex( 42.5749, -19.3173), gr_complex(-7.6481, -11.6796), gr_complex(-25.5536,  18.9899), gr_complex(-24.7421, -64.8109), 
gr_complex( -5.3675,   6.9758), gr_complex(-9.7450,  42.8434), gr_complex(-42.3165, -46.0488), gr_complex( 32.3332, -4.6225), 
gr_complex(  9.4235,   9.5403), gr_complex(-62.1141, -14.9145), gr_complex(-1.5754, -20.2673), gr_complex(-1.0123, -18.9973), 
gr_complex(-19.3000,  45.2841), gr_complex(-4.2165,  24.2354), gr_complex(-39.80, -38.22), gr_complex(-54.8928, -3.0417), 
gr_complex(  9.8774,  17.2181), gr_complex( 12.0953,  8.8521), gr_complex(-59.5168,  5.5254), gr_complex(-7.5444, -33.2079), 
gr_complex( 15.5677, -42.3633), gr_complex( 7.3455, -25.3407), gr_complex( 56.4308, -23.6738), gr_complex( 13.2167,  16.3248), 
gr_complex(  4.3510,   2.0562), gr_complex(-5.8964, -20.8074), gr_complex(-17.9876,  37.86), gr_complex( 18.0984,  9.7533), 
gr_complex(-42.1675, -17.8307), gr_complex(-8.3276,  14.7914), gr_complex( 53.9902, -2.73), gr_complex( 19.4065, -2.2803), 
gr_complex( 12.5391,  -0.8389), gr_complex(-2.7640,  30.2720), gr_complex( 6.0743,  49.3245), gr_complex(-30.6516, -29.8721), 
gr_complex(-42.2983, -24.8540), gr_complex( 2.0498,  28.7805), gr_complex(-26.4552,  15.7514), gr_complex(-0.8367,  18.3541), 
gr_complex(-12.5794,  11.7159), gr_complex(-7.8840, -1.1823), gr_complex( 37.8993, -4.0715), gr_complex(-32.9141, -50.45), 
gr_complex( 11.3128, -24.5790), gr_complex( 32.52,  58.6526), gr_complex(-27.5923, -3.9687), gr_complex( 26.9060, -25.85), 
gr_complex( 11.2949,  57.7560), gr_complex(-5.3724,  0.4467), gr_complex(-4.5574, -23.2225), gr_complex(-39.2168,  46.12), 
gr_complex( -0.8512,  32.1518), gr_complex( 23.0984, -5.6126), gr_complex( 38.6247, -28.0221), gr_complex( 9.1723, -15.3642), 
gr_complex(-28.3344, -28.7069), gr_complex( 32.1525, -42.6817), gr_complex( 24.6993,  29.9795), gr_complex(-0.9616,  42.1077), 
gr_complex( 28.4111,  -7.6077), gr_complex(-4.2319, -47.83), gr_complex(-33.3817, -37.6614), gr_complex( 3.3653,  6.4175), 
gr_complex( 15.1484, -25.5827), gr_complex(-38.3468, -7.5573), gr_complex(-44.3979,  34.1093), gr_complex(-31.6208,  3.2643), 
gr_complex(-59.2154,  11.2964), gr_complex( 0.8619, -2.5942), gr_complex( 25.3309, -21.6355), gr_complex(-27.7055,  8.7722), 
gr_complex( 29.2865,   7.8962), gr_complex( 54.7755,  1.1716), gr_complex( 5.4874, -9.9256), gr_complex(-13.4147, -37.6413), 
gr_complex(-42.9014, -25.8804), gr_complex(-26.4713, -6.9455), gr_complex(-26.1947, -22.4388), gr_complex(-54.8298,  4.1242), 
gr_complex( -7.5773,  32.2383), gr_complex( 14.6380, -6.6734), gr_complex( 33.6777, -25.9064), gr_complex( 18.1276, -29.6833), 
gr_complex(-54.4853,  -1.3137), gr_complex( 1.4630,  15.8138), gr_complex( 23.2314, -40.97), gr_complex(-6.0426, -15.6596), 
gr_complex( 57.7431,  15.2027), gr_complex( 13.8973, -8.5932), gr_complex(-33.8249,  33.8702), gr_complex( 7.2286,  48.4315), 
gr_complex(-30.5998,   6.3529), gr_complex(-50.5988, -18.6639), gr_complex(-1.9537,  11.3235), gr_complex( 48.2715,  21.0667), 
gr_complex( 31.3570, -24.4599), gr_complex(-34.6799,  2.0563), gr_complex(-21.8964,  4.25), gr_complex(-4.5314, -12.8632), 
gr_complex( 14.7544,  44.4683), gr_complex( 61.6521,  15.0146), gr_complex( 3.0895, -25.9378), gr_complex(-48.4816,  27.7580), 
gr_complex(  3.4814,  29.4358), gr_complex( 37.2224, -26.7576), gr_complex(-0.3520, -15.3224), gr_complex(-47.41,  29.8143), 
gr_complex(-19.5720, -17.9734), gr_complex(-8.6269, -60.1274), gr_complex(-43.3895, -1.2352), gr_complex(-3.1728,  9.9540), 
gr_complex( 20.0411, -40.6106), gr_complex(-13.9816, -4.8417), gr_complex(-15.0906,  28.6443), gr_complex(-25.3944, -41.7377), 
gr_complex( -3.4328, -40.0693), gr_complex( 63.6291,  23.7323), gr_complex( 7.9867,  17.5321), gr_complex(-55.4693, -8.8372), 
gr_complex( 22.6394, -25.1612), gr_complex( 9.9180,  8.50), gr_complex(-31.8899,  26.7796), gr_complex( 47.2653, -3.0200), 
gr_complex( 21.9623,  14.1237), gr_complex(-36.1984,  8.6573), gr_complex(-34.9892, -8.6674), gr_complex(-51.0681, -4.9325), 
gr_complex(-33.5756, -11.0829), gr_complex(-53.2283, -8.4893), gr_complex(-12.7956, -16.1725), gr_complex( 43.7905,  16.5606), 
gr_complex(-34.4382,  24.2628), gr_complex(-32.6090,  6.3896), gr_complex(-29.0673,  41.5484), gr_complex(-20.7826,  4.2853), 
gr_complex( 57.3567,   2.7043), gr_complex(-17.6112,  54.0930), gr_complex(-29.8429, -6.4599), gr_complex( 43.9983, -33.3869), 
gr_complex( 12.2462,  20.3287), gr_complex( 3.8310,  51.8353), gr_complex(-3.1773,  11.5186), gr_complex( 9.4303, -42.9571), 
gr_complex( 34.5222,   2.9487), gr_complex( 12.5660,  51.4033), gr_complex(-23.3311,  33.7915), gr_complex(-55.6916,  4.8205), 
gr_complex( -0.1278, -23.4551), gr_complex( 5.4210, -30.2029), gr_complex(-23.0773,  5.9516), gr_complex( 49.4294,  19.9102), 
gr_complex( 18.9735, -34.6314), gr_complex(-27.1327, -8.5339), gr_complex( 5.6138,  52.9996), gr_complex(-21.0791,  29.4934), 
gr_complex(-20.8488,  51.0701), gr_complex( 27.6310,  31.4239), gr_complex( 44.5323, -30.7065), gr_complex( 8.3333, -15.3921), 
gr_complex(-27.1527, -35.7211), gr_complex( 10.3432, -9.2159), gr_complex( 39.7323,  24.7340), gr_complex(-8.8745, -41.1579), 
gr_complex(-24.3449,  -3.5606), gr_complex( 30.3661,  55.3401), gr_complex( 6.4883,  9.3192), gr_complex(-17.5507, -6.8409), 
gr_complex( 34.6400,  17.7032), gr_complex( 5.0887,  46.1233), gr_complex(-7.1174,  47.9634), gr_complex( 46.8889,  26.2515), 
gr_complex( 40.6617,  7.2988), gr_complex( 38.7670, -23.5266), gr_complex( 55.9214, -16.8405), gr_complex( 15.4908, -6.8648), 
gr_complex(-39.4132, -32.6055), gr_complex(-6.2921, -33.9941), gr_complex( 22.6276, -34.6144), gr_complex(-30.2729, -33.9688), 
gr_complex(-32.0664, -40.0447), gr_complex( 15.5140, -36.16), gr_complex( 23.7648,  30.2122), gr_complex(-32.3019,  20.1952), 
gr_complex(-30.9775, -40.5677), gr_complex( 48.1461, -25.7835), gr_complex( 14.4476, -24.85), gr_complex(-4.2352, -22.6182), 
gr_complex( 22.6610,   7.0518), gr_complex(-13.4188,  32.9561), gr_complex( 3.4706,  23.0548), gr_complex(-6.0352, -39.7013), 
gr_complex( -5.4747, -12.6251), gr_complex( 25.9990,  45.5162), gr_complex(-11.4557,  41.9426), gr_complex( 28.4565,  43.0428), 
gr_complex( 22.3553, -9.6290), gr_complex(-20.7089, -18.5978), gr_complex( 57.1296,  22.4879), gr_complex( 10.6658, -5.5921), 
gr_complex(-32.4243, -11.0125), gr_complex( 56.4808, -21.7897), gr_complex( 11.7132, -16.8582), gr_complex(-29.7795, -1.9042), 
gr_complex( -2.6237, -47.8489), gr_complex(-40.1337,  5.9558), gr_complex( 5.3416,  55.1929), gr_complex( 42.5946, -13.4887), 
gr_complex(  6.5266, -31.8582), gr_complex( 56.1437, -12.1429), gr_complex( 29.8322, -9.9825), gr_complex(-41.8339, -32.8953), 
gr_complex(-22.6274, -42.4264), gr_complex(-34.2629,  1.6500), gr_complex(-15.2766,  40.4937), gr_complex(-23.2619,  28.2193), 
gr_complex(-55.0863, -40.2730), gr_complex( 24.3038, -33.8823), gr_complex( 11.6136,  18.5215), gr_complex(-23.3493, -14.7344), 
gr_complex( 29.0933,  -4.5311), gr_complex(-11.0211,  27.5284), gr_complex(-48.9638,  10.30), gr_complex(-39.0190, -2.5814), 
gr_complex(-41.4364,   0.0916), gr_complex(-27.2286,  39.0674), gr_complex(-25.2266,  17.7156), gr_complex(-10.1481, -46.7047), 
gr_complex( -6.7393, -51.5606), gr_complex(-34.8473, -13.6728), gr_complex(-17.3090,  14.1409), gr_complex( 18.7085, -45.1226), 
gr_complex( -4.2704, -43.7330), gr_complex(-45.3417, -9.2360), gr_complex(-5.0956, -55.7216), gr_complex( 44.2554, -17.5916), 
gr_complex( 22.2187, -18.9600), gr_complex( 10.1937, -29.5191), gr_complex( 0.3334,  41.5955), gr_complex(-16.0784,  20.8598), 
gr_complex( 12.9923,  41.6887), gr_complex( 38.1501,  38.2487), gr_complex( 28.3928, -41.0479), gr_complex( 26.7838, -21.8243), 
gr_complex( 47.7077,  -1.4493), gr_complex( 41.74,  20.4243), gr_complex( 21.3722,  36.8309), gr_complex( 22.6092,  42.1995), 
gr_complex(-11.2868,  47.1945), gr_complex(-16.72,  24.6383), gr_complex( 18.7855,  57.5542), gr_complex(-18.0882,  9.5361), 
gr_complex( -3.5567, -28.6568), gr_complex( 58.1191,  20.2440), gr_complex( 14.6244, -34.1181), gr_complex(-1.9342, -19.4722), 
gr_complex( 13.9331,  18.5679), gr_complex(-30.6360, -3.1928), gr_complex(-13.4554,  11.8724), gr_complex( 18.5710, -11.1825), 
gr_complex(-30.5835,  12.4477), gr_complex(-24.1891,  23.4879), gr_complex( 6.7608,  11.0982), gr_complex(-47.2843,  39.31), 
gr_complex(-25.8366,  -1.2615), gr_complex( 55.5825, -14.7016), gr_complex( 27.6705,  3.8687), gr_complex( 0.4836,  45.3278), 
gr_complex(  4.5756,  43.0365), gr_complex(-29.7037, -53.9547), gr_complex(-35.6863, -7.6493), gr_complex(-10.0845,  21.2213), 
gr_complex( 12.2627, -51.8733), gr_complex(-8.4778, -18.8332), gr_complex(-48.1922,  17.5114), gr_complex(-20.2423,  14.8198), 
gr_complex(  5.0984,  26.8880), gr_complex(-25.4645,  48.7292), gr_complex(-40.0199,  10.5151), gr_complex(-30.1320, -17.09), 
gr_complex(-33.0663,  46.0294), gr_complex(-16.9250,  16.7015), gr_complex( 24.9250, -34.8020), gr_complex( 18.7075, -35.1246), 
gr_complex(-11.0972, -12.3806), gr_complex(-17.8312,  41.1302), gr_complex(-19.1634, -16.83), gr_complex(-13.1471, -35.7387), 
gr_complex(-20.7336,  34.4043), gr_complex(-38.2815,  34.3346), gr_complex(-40.2774, -9.5243), gr_complex(-0.7103, -44.8443), 
gr_complex( 39.7693,  16.5485), gr_complex( 2.1291,  21.4208), gr_complex(-25.6429, -20.7707), gr_complex(-24.9547,  13.4961), 
gr_complex(-25.6238, -10.6487), gr_complex( 34.1487,  18.9640), gr_complex( 48.1554, -4.8044), gr_complex(-6.0816, -58.9932), 
gr_complex(-31.3839,  50.5711), gr_complex(-22.4517,  11.8792), gr_complex( 33.9166, -28.3683), gr_complex( 24.8160,  31.0687), 
gr_complex(-46.7427, -37.7565), gr_complex(-20.6882, -11.5067), gr_complex(-0.5541,  47.4949), gr_complex(-40.4836,  3.1583), 
gr_complex(-41.3893,  -2.3351), gr_complex(-33.4891,  13.4340), gr_complex(-21.9908, -22.4588), gr_complex( 3.2091, -69.0316), 
gr_complex(  3.2322,  -7.4273), gr_complex(-6.3241,  16.8246), gr_complex(-0.5443, -21.7096), gr_complex(-1.2678,  38.1221), 
gr_complex(-15.2300,  18.8426), gr_complex(-4.7851, -42.8433), gr_complex(-20.1302, -26.93), gr_complex(-44.7205, -19.9987), 
gr_complex( 17.4169,  29.9404), gr_complex( 20.0338,  26.8776), gr_complex(-32.2768, -45.8658), gr_complex(-11.6841, -14.1012), 
gr_complex(-34.3277,  19.0841), gr_complex(-20.7518, -17.1098), gr_complex( 40.2803,  3.55), gr_complex( 16.5154,  45.9087), 
gr_complex( 32.7883,  36.0434), gr_complex( 30.1715,  13.21), gr_complex(-12.2140, -34.7116), gr_complex( 17.2738, -46.70), 
gr_complex(-28.2541,  -6.5822), gr_complex(-58.2406, -29.8352), gr_complex( 27.7447, -23.3451), gr_complex( 23.9742, -4.5216), 
gr_complex( 12.6306, -29.2741), gr_complex( 29.9037,  21.7934), gr_complex(-54.0578,  18.9555), gr_complex(-25.8804, -11.4334), 
gr_complex( 37.5147,  21.3137), gr_complex(-47.6400, -26.2085), gr_complex(-3.2784, -32.24), gr_complex( 41.67, -16.8574), 
gr_complex(-20.6870, -32.9948), gr_complex( 29.8424,  44.6852), gr_complex( 30.4301,  15.3432), gr_complex( 13.79, -52.6291), 
gr_complex( 27.2286,   2.5828), gr_complex(-31.3252, -12.5277), gr_complex(-25.3898, -33.0105), gr_complex(-30.5067, -8.1884), 
gr_complex(-33.1708, -33.6719), gr_complex(-15.4160, -44.9581), gr_complex(-39.8017, -2.2605), gr_complex( 24.6986,  38.2455), 
gr_complex( 42.4425,  29.9560), gr_complex( 24.8745,  33.1906), gr_complex( 53.2855,  5.7386), gr_complex( 13.6906, -30.2231), 
gr_complex(  3.9123,  43.0284), gr_complex(-11.0549,  51.8106), gr_complex(-39.0731,  17.3392), gr_complex(-20.9715,  47.5315), 
gr_complex(  1.2086,  26.9820), gr_complex( 57.2123,  11.0440), gr_complex( 33.2832,  8.3816), gr_complex(-49.0947, -1.6593), 
gr_complex(-32.6334, -14.7756), gr_complex( 37.5130, -43.3023), gr_complex( 33.9038, -2.9807), gr_complex(-10.3430,  25.5998), 
gr_complex( 42.9282,   9.1997), gr_complex( 55.8439, -9.5930), gr_complex( 19.9931, -13.6631), gr_complex( 47.2462,  22.1051), 
gr_complex( 10.6058, -30.1034), gr_complex(-30.6739, -32.2419), gr_complex(-17.9522,  61.9569), gr_complex(-13.7227,  17.4394), 
gr_complex(-20.1319,  -9.4100), gr_complex( 11.3919,  22.2582), gr_complex( 61.9953,  11.6406), gr_complex( 2.9164,  31.3969), 
gr_complex(-29.4350,  -5.7862), gr_complex(-9.6209, -32.1999), gr_complex(-22.4513,  13.0501), gr_complex( 29.5769, -11.3787), 
gr_complex( 47.3360, -10.2432), gr_complex( 44.3468,  19.8677), gr_complex( 42.3736, -42.4614), gr_complex(-4.6966, -30.4904), 
gr_complex(-20.9055,  44.9346), gr_complex(-20.7015,  21.0977), gr_complex( 45.0654,  3.2759), gr_complex( 44.7268, -8.9846), 
gr_complex(-20.4424, -23.0560), gr_complex( 24.3218,  37.1536), gr_complex( 6.9695,  38.8870), gr_complex(-32.7235, -13.9549), 
gr_complex(-31.3816,  17.6516), gr_complex(-27.8417,  60.6073), gr_complex( 11.9540,  9.8185), gr_complex( 7.5731, -11.9839), 
gr_complex( 13.4029,  19.2935), gr_complex(-12.3070, -28.6640), gr_complex(-24.0762, -37.1278), gr_complex( 15.6590, -29.4556), 
gr_complex(-30.4105, -25.1740), gr_complex( 15.1778,  49.1050), gr_complex( 51.5558, -14.7858), gr_complex(-8.4840, -36.2026), 
gr_complex( 31.3117,  35.3378), gr_complex( 43.5484, -11.3895), gr_complex( 46.6817,  4.9973), gr_complex( 47.4222, -12.2084), 
gr_complex(-17.5292, -31.2500), gr_complex(-27.7844,  17.4159), gr_complex( 0.2753, -34.4171), gr_complex( 46.8780, -21.7099), 
gr_complex( 33.9272, -25.3183), gr_complex(-35.3063, -36.4838), gr_complex(-33.5577,  11.3357), gr_complex( 6.5156, -44.8948), 
gr_complex( 52.3476, -10.2955), gr_complex( 16.9710,  33.5992), gr_complex(-43.2535,  1.7997), gr_complex( 2.9082,  25.5641), 
gr_complex(  4.4287, -17.8897), gr_complex(-29.5171, -43.2804), gr_complex(-1.1290, -6.1686), gr_complex( 12.9059,  9.4269), 
gr_complex( 22.6635,  -1.3479), gr_complex( 14.5016,  21.4031), gr_complex( 2.4173,  62.2882), gr_complex( 40.8833, -21.9067), 
gr_complex( 34.6012, -31.5539), gr_complex( 31.6344,  10.2578), gr_complex( 51.1676, -28.2852), gr_complex( 12.0102,  39.8786), 
gr_complex( 31.7174,  38.8620), gr_complex( 25.5951, -28.5378), gr_complex(-37.7791,  12.7393), gr_complex(-22.3324,  47.3541), 
gr_complex(-22.6444,  25.0785), gr_complex( 12.1814, -27.3961), gr_complex( 29.2519,  9.3508), gr_complex(-35.9171,  5.3922), 
gr_complex(-24.9288, -59.5845), gr_complex(-37.6590,  11.3282), gr_complex(-48.8560, -0.0831), gr_complex(-6.1963, -35.4362), 
gr_complex( 23.2400,  30.3293), gr_complex( 54.9472,  17.6738), gr_complex( 3.7144,  34.5898), gr_complex(-2.7032,  56.6902), 
gr_complex( 31.9900,  -7.9842), gr_complex(-3.2273, -15.0270), gr_complex( 24.8585,  0.4655), gr_complex( 12.0787, -18.6956), 
gr_complex(-18.2095,  24.8050), gr_complex( 24.5492,  53.1897), gr_complex( 29.6668, -22.6597), gr_complex( 34.9364, -50.8340), 
gr_complex( 17.1061, -16.5385), gr_complex(-9.6213,  17.0889), gr_complex( 13.2063,  53.4774), gr_complex( 6.7335,  33.8111) 
};
