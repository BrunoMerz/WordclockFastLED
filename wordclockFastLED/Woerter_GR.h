/**
   Woerter_GR
   Definition der deutschen Woerter fuer die Zeitansage.
   Die Woerter sind Bitmasken fuer die Matrix.

     01234567890
   0 HΧΩPATEINAI
   1 MIAΔYOTPEIΣ
   2 TEΣΣEPIΣEΞI
   3 ΠENTEPOΧTΩH
   4 EΦTAEENTEKA
   5 ΔΩΔEKAENNIA
   6 ΔEKAXΠAPAEP
   7 KAIETETAPTO
   8 EIKOΣIHΔEKA
   9 MIΣHEΠENTEP

   @autor    Bruno Merz
   @version  1.0
   @created  23.08.2021
*/

#pragma once

/**
   Definition der Woerter
*/
#define GR_PAPA         matrix[6] |= 0b0000011110000000	// VOR
#define GR_KAI          matrix[7] |= 0b1110000000000000 // NACH
#define GR_EINAI        matrix[0] |= 0b1011101111100000 // DIE UHRZEIT IST

#define GR_PENTE        matrix[9] |= 0b0000011111000000 // FÜNF
#define GR_DEKA         matrix[8] |= 0b0000000111100000 // ZEHN
#define GR_TETAPTO      matrix[7] |= 0b0000111111100000 // VIERTEL
#define GR_EIKOSI       matrix[8] |= 0b1111110000000000 // ZWANZIG
#define GR_MISH         matrix[9] |= 0b1111000000000000 // HALB

#define GR_H_MIA        matrix[1] |= 0b1110000000000000 // EINS
#define GR_H_DYO        matrix[1] |= 0b0001110000000000 // ZWEI
#define GR_H_TPEIS      matrix[1] |= 0b0000001111100000 // DREI
#define GR_H_TESSEPIS   matrix[2] |= 0b1111111100000000 // VIER
#define GR_H_PENTE      matrix[3] |= 0b1111100000000000 // FÜNF
#define GR_H_EXI        matrix[2] |= 0b0000000011100000 // SECHS
#define GR_H_EPHTA      matrix[4] |= 0b1111000000000000 // SIEBEN
#define GR_H_OXTOO      matrix[3] |= 0b0000001111000000 // ACHT
#define GR_H_ENNIA      matrix[5] |= 0b0000001111100000 // NEUN
#define GR_H_DEKA       matrix[6] |= 0b1111000000000000 // ZEHN
#define GR_H_ENTEKA     matrix[4] |= 0b0000011111100000 // ELF
#define GR_H_DOODEKA    matrix[5] |= 0b1111110000000000 // ZWÖLF
