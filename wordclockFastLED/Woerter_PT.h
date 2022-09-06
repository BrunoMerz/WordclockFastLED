/**
   Woerter_PT
   Definition der portugiesischen Woerter fuer die Zeitanzeige.
   Die Woerter sind Bitmasken fuer die Matrix.

     01234567890
   0 ESAOUMATRES
   1 MEIOLDIADEZ
   2 DUASEISETEY
   3 QUATROHNOVE
   4 CINCOITONZE
   5 ZMEIALNOITE
   6 HORASYMENOS
   7 VINTECAMEIA
   8 UMVQUARTOPM
   9 DEZOEYCINCO

   Author Sebastian Hanke
   @version  1.1
   @created  06.06.2016

*/

#pragma once

/**
   Definition der Woerter
*/

#define PT_MENOS	      matrix[6] |= 0b0000001111100000
#define PT_EEINS        matrix[6] |= 0b0000000100000000
#define PT_E            matrix[0] |= 0b1000000000000000
#define PT_SAO          matrix[0] |= 0b0111000000000000
#define PT_H_HORAS      matrix[6] |= 0b1111100000000000
#define PT_H_HORA       matrix[6] |= 0b1111000000000000

#define PT_CINCO        matrix[9] |= 0b0000001111100000
#define PT_DEZ          matrix[9] |= 0b1110000000000000
#define PT_UM_QUARTO    matrix[8] |= 0b1101111110000000
#define PT_VINTE        matrix[7] |= 0b1111100000000000
#define PT_MEIA         matrix[7] |= 0b0000000111100000
#define PT_E_CINCO      matrix[9] |= 0b0000101111100000

#define PT_H_UMA        matrix[0] |= 0b0000111000000000
#define PT_H_DUAS       matrix[2] |= 0b1111000000000000
#define PT_H_TRES       matrix[0] |= 0b0000000111100000
#define PT_H_QUATRO     matrix[3] |= 0b1111110000000000
#define PT_H_CINCO      matrix[4] |= 0b1111100000000000
#define PT_H_SEIS       matrix[2] |= 0b0001111000000000
#define PT_H_SETE       matrix[2] |= 0b0000001111000000
#define PT_H_OITO       matrix[4] |= 0b0000111100000000
#define PT_H_NOVE       matrix[3] |= 0b0000000111100000
#define PT_H_DEZ        matrix[1] |= 0b0000000011100000
#define PT_H_ONZE       matrix[4] |= 0b0000000111100000
#define PT_H_MEIODIA    matrix[1] |= 0b1111011100000000
#define PT_H_MEIANOITE  matrix[5] |= 0b0111101111100000
