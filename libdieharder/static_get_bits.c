/*
 *========================================================================
 * See copyright in copyright.h and the accompanying file COPYING
 *========================================================================
 */

/*
 *========================================================================
 * This should be a nice, big case switch where we add EACH test
 * we might want to do and either just configure and do it or
 * prompt for input (if absolutely necessary) and then do it.
 *========================================================================
 */

/*
 * This is a not-quite-drop-in replacement for my old get_rand_bits()
 * routine contributed by John E. Davis.
 *
 * It should give us the "next N bits" from the random number stream,
 * without dropping any.  The return can even be of arbitrary size --
 * we make the return a void pointer whose size is specified by the
 * caller (and guaranteed to be big enough to hold the result).
 */
inline static uint get_rand_bits_uint (uint nbits, uint mask, gsl_rng *rng)
{
   static uint bit_buffer;
   static uint bits_left_in_bit_buffer = 0;
   uint bits;

   if (bits_left_in_bit_buffer >= nbits)
     {
	bits_left_in_bit_buffer -= nbits;
	bits = (bit_buffer >> bits_left_in_bit_buffer);
	return bits & mask;
     }

   nbits = nbits - bits_left_in_bit_buffer;
   bits = bit_buffer << nbits;
   while (1)
     {
	bit_buffer = gsl_rng_get (rng);
	bits_left_in_bit_buffer = rmax_bits;
	if (bits_left_in_bit_buffer >= nbits)
	  {
	     bits_left_in_bit_buffer -= nbits;
	     bits |= (bit_buffer >> bits_left_in_bit_buffer);
	     return bits & mask;
	  }
	nbits -= bits_left_in_bit_buffer;
	bits |= (bit_buffer << nbits);
     }
}

/*
 * This is a drop-in-replacement for get_bit_ntuple() contributed by
 * John E. Davis.  It speeds up this code substantially but may
 * require work if/when rngs that generate 64-bit rands come along.
 * But then, so will other programs.
 */
inline static uint get_bit_ntuple_from_uint (uint bitstr, uint nbits, 
                                      uint mask, uint boffset)
{
   uint result;
   uint len;
   
   /* Only rmax_bits in bitstr are meaningful */
   boffset = boffset % rmax_bits;
   result = bitstr >> boffset;
   
   if (boffset + nbits <= rmax_bits)
     return result & mask;
   
   /* Need to wrap */
   len = rmax_bits - boffset;
   while (len < nbits)
     {
	result |= (bitstr << len);
	len += rmax_bits;
     }
   return result & mask;
}


