
#include "IRremote.h"
#include "IRremoteInt.h"

//==============================================================================
//
//
//                              J A P A N E S E
//
//
//==============================================================================

#define BITS          48  // The number of bits in the command

#define HDR_MARK    3380  // 3.38ms
#define HDR_SPACE   1690  // 1.69ms

#define BIT_MARK    420   // 0.42ms
#define ONE_SPACE   1270  // 1.27ms
#define ZERO_SPACE  420   // 0.42ms

//+=============================================================================
//
#if SEND_JAPANESE
void  IRsend::sendJapanese (uint64_t data,  int nbits)
{
	// Set IR carrier frequency
	enableIROut(38);

	// Header
	mark (HDR_MARK);
	space(HDR_SPACE);

	// Data
	for (uint64_t  mask = 1ULL << (nbits - 1);  mask;  mask >>= 1) {
		if (data & mask) {
			mark (BIT_MARK);
			space(ONE_SPACE);
		} else {
			mark (BIT_MARK);
			space(ZERO_SPACE);
		}
	}

	// Footer
	mark(BIT_MARK);
    space(0);  // Always end with the LED off
}
#endif

//+=============================================================================
//
#if DECODE_JAPANESE
bool  IRrecv::decodeJapanese (decode_results *results)
{
	uint64_t  data   = 0;  // Somewhere to build our code
	int            offset = 1;  // Skip the Gap reading

	// Check we have the right amount of data
	if (irparams.rawlen != 1 + 2 + (2 * BITS) + 1)  return false ;

	// Check initial Mark+Space match
	if (!MATCH_MARK (results->rawbuf[offset++], HDR_MARK ))  return false ;
	if (!MATCH_SPACE(results->rawbuf[offset++], HDR_SPACE))  return false ;

	// Read the bits in
	for (int i = 0;  i < BITS;  i++) {
		// Each bit looks like: MARK + SPACE_1 -> 1
		//                 or : MARK + SPACE_0 -> 0
		if (!MATCH_MARK(results->rawbuf[offset++], BIT_MARK))  return false ;

		// IR data is big-endian, so we shuffle it in from the right:
		if      (MATCH_SPACE(results->rawbuf[offset], ONE_SPACE))   data = (data << 1) | 1 ;
		else if (MATCH_SPACE(results->rawbuf[offset], ZERO_SPACE))  data = (data << 1) | 0 ;
		else                                                        return false ;
		offset++;
	}

	// Success
	results->bits        = BITS;
	results->value       = data;
	results->decode_type = JAPANESE;
	return true;
}
#endif
