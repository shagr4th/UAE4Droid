#ifndef _LINETOSCR_DEFS
   #define _LINETOSCR_DEFS
   
   /* Lines are split into 32-pixel segments.
    * Only changed segments are redrawn.
    */
   #define LINETOSCR_NEXT_PIXEL \
      if (--segment_counter == 0) { \
         if ((changes >>= 1) == 0) \
            break; \
         segment_counter = 32 / SRC_INC; \
         do_redraw = 0; \
      }

   /* Skip unchanged line segment */
   #if HDOUBLE
      #define LINETOSCR_SKIP_SEGMENT \
         spix += 32; \
         dpix += 64 / SRC_INC; \
         if ((changes >>= 1) == 0) \
            break;
   #else
      #define LINETOSCR_SKIP_SEGMENT \
         spix += 32; \
         dpix += 32 / SRC_INC; \
         if ((changes >>= 1) == 0) \
            break;
   #endif
#endif

static int LNAME (int spix, int dpix, int stoppos)
{
   register TYPE *buf = ((TYPE *)xlinebuffer);
#if AGA
   uae_u8 xor_val;
#endif

#if AGA
   xor_val = (uae_u8)(dp_for_drawing->bplcon4 >> 8);
#endif
   if (dp_for_drawing->ham_seen) {
      /* HAM 6 / HAM 8 */
      while (dpix < stoppos) {
#if AGA
            TYPE d = CONVERT_RGB (ham_linebuf[spix]);
#else
            TYPE d = xcolors[ham_linebuf[spix]];
#endif
            spix += SRC_INC;
            buf[dpix++] = d;
#if HDOUBLE
            buf[dpix++] = d;
#endif
      }
   } else if (bpldualpf) {
#if AGA
      /* AGA Dual playfield */
      int *lookup = bpldualpfpri ? dblpf_ind2_aga : dblpf_ind1_aga;
      int *lookup_no = bpldualpfpri ? dblpf_2nd2 : dblpf_2nd1;
      while (dpix < stoppos) {
            int pixcol = pixdata.apixels[spix];
            TYPE d;
            if (spriteagadpfpixels[spix]) {
               d = colors_for_drawing.acolors[spriteagadpfpixels[spix]];
               spriteagadpfpixels[spix]=0;
            } else {
               int val = lookup[pixcol];
               if (lookup_no[pixcol] == 2)  val += dblpfofs[bpldualpf2of];
               /* val ^= xor; ??? */
               d = colors_for_drawing.acolors[val];
            }
            spix += SRC_INC;
            buf[dpix++] = d;
#if HDOUBLE
            buf[dpix++] = d;
#endif
      }
#else
      /* OCS/ECS Dual playfield  */
      int *lookup = bpldualpfpri ? dblpf_ind2 : dblpf_ind1;
      while (dpix < stoppos) {
            int pixcol = pixdata.apixels[spix];
            TYPE d = colors_for_drawing.acolors[lookup[pixcol]];
            spix += SRC_INC;
            buf[dpix++] = d;
#if HDOUBLE
            buf[dpix++] = d;
#endif
      }
#endif
   } else if (bplehb) {
      while (dpix < stoppos) {
            int p = pixdata.apixels[spix];
            TYPE d = colors_for_drawing.acolors[p];
            spix += SRC_INC;
#if AGA
            /* AGA EHB playfield */
            if (p>= 32 && p < 64) /* FIXME: what about sprite colors between 32 and 64? */
               d = (colors_for_drawing.color_regs_aga[(p-32)^xor_val] >> 1) & 0x7F7F7F;
#else
            /* OCS/ECS EHB playfield */
            if (p >= 32)
               d = xcolors[(colors_for_drawing.color_uae_regs_ecs[p-32] >> 1) & 0x777];
#endif
            buf[dpix++] = d;
#if HDOUBLE
            buf[dpix++] = d;
#endif
      }
   } else {
      while (dpix < stoppos) {
#if AGA
            TYPE d = colors_for_drawing.acolors[pixdata.apixels[spix]^xor_val];
#else
            TYPE d = colors_for_drawing.acolors[pixdata.apixels[spix]];
#endif
            spix += SRC_INC;
            buf[dpix++] = d;
#if HDOUBLE
            buf[dpix++] = d;
#endif
      }
   }
   return spix;
}

#undef LNAME
#undef HDOUBLE
#undef SRC_INC
#undef AGA
