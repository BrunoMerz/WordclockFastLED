/**
   Woerter_AE
   Definition der arabischen Emirate Woerter fuer die Zeitansage.
   Die Woerter sind Bitmasken fuer die Matrix.

     01234567890
   0 ESKISTLFUNF
   1 ZEHNZWANZIG
   2 DREIVIERTEL
   3 TGNACHVORJM
   4 HALBQZWOLFP
   5 ZWEINSIEBEN
   6 KDREIRHFUNF
   7 ELFNEUNVIER
   8 WACHTZEHNRS
   9 BSECHSFMUHR

*/

#pragma once

/**
   Definition der Woerter
*/

#define AE_ES            matrix[0] |= 0b0000000001100000
#define AE_IST           matrix[0] |= 0b0000000110000000
#define AE_ESIST         matrix[0] |= 0b0000000111100000
#define AE_MINUTEN       matrix[9] |= 0b0000110000000000

#define AE_PLUS_FUENF    matrix[9] |= 0b0000000011100000
#define AE_PLUS_ZEHN     matrix[8] |= 0b0000000001100000
#define AE_PLUS_VIERTEL  matrix[8] |= 0b0000011000000000
#define AE_PLUS_DRITTEL  matrix[8] |= 0b0011000000000000
#define AE_HALB          matrix[6] |= 0b0000001110000000
#define AE_MINUS_FUENF   matrix[7] |= 0b0000000011100000
#define AE_MINUS_ZEHN    matrix[7] |= 0b1110000000000000
#define AE_MINUS_VIERTEL matrix[7] |= 0b0000111000000000
#define AE_MINUS_DRITTEL matrix[6] |= 0b0011100000000000

#define AE_H_EIN         matrix[1] |= 0b0000001110000000
#define AE_H_ZWEI        matrix[1] |= 0b0000000001100000
#define AE_H_DREI        matrix[2] |= 0b0000000001100000
#define AE_H_VIER        matrix[2] |= 0b1100000000000000
#define AE_H_FUENF       matrix[2] |= 0b0001110000000000
#define AE_H_SECHS       matrix[1] |= 0b0001110000000000
#define AE_H_SIEBEN      matrix[1] |= 0b1110000000000000
#define AE_H_ACHT        matrix[4] |= 0b0000001100000000
#define AE_H_NEUN        matrix[3] |= 0b0111000000000000
#define AE_H_ZEHN        matrix[2] |= 0b0000001110000000
#define AE_H_ELF         matrix[0] |= 0b1100011000000000
#define AE_H_ZWOELF      matrix[0] |= 0b1111000000000000
