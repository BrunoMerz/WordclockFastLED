/**
   Woerter_CH
   Definition der schweizerischen Woerter fuer die Zeitansage.
   Die Woerter sind Bitmasken fuer die Matrix.

     01234567890
   0 ESKISCHAFÜF
   1 VIERTUBFZÄÄ
   2 ZWÄNZGSIVOR
   3 ABOHAUBIEGE
   4 EISZWÖISDRÜ
   5 VIERIFÜFIQT
   6 SÄCHSISIBNI
   7 ACHTINÜNIEL
   8 ZÄNIERBEUFI
   9 ZWÖUFINAUHR

   Version 1.1: - Layoutanpassung
*/

#pragma once

/**
   Definition der Woerter
*/
#define CH_VOR          matrix[2] |= 0b0000000011100000
#define CH_AB           matrix[3] |= 0b1100000000000000
#define CH_ESISCH       matrix[0] |= 0b1101111000000000

#define CH_FUEF         matrix[0] |= 0b0000000011100000
#define CH_ZAEAE        matrix[1] |= 0b0000000011100000
#define CH_VIERTU       matrix[1] |= 0b1111110000000000
#define CH_ZWAENZG      matrix[2] |= 0b1111110000000000
#define CH_HAUBI        matrix[3] |= 0b0001111100000000

#define CH_H_EIS        matrix[4] |= 0b1110000000000000
#define CH_H_ZWOEI      matrix[4] |= 0b0001111000000000
#define CH_H_DRUE       matrix[4] |= 0b0000000011100000
#define CH_H_VIER       matrix[5] |= 0b1111100000000000
#define CH_H_FUEFI      matrix[5] |= 0b0000011110000000
#define CH_H_SAECHSI    matrix[6] |= 0b1111110000000000
#define CH_H_SIEBNI     matrix[6] |= 0b0000001111100000
#define CH_H_ACHTI      matrix[7] |= 0b1111100000000000
#define CH_H_NUENI      matrix[7] |= 0b0000011110000000
#define CH_H_ZAENI      matrix[8] |= 0b1111000000000000
#define CH_H_EUFI       matrix[8] |= 0b0000000111100000
#define CH_H_ZWOEUFI    matrix[9] |= 0b1111110000000000
