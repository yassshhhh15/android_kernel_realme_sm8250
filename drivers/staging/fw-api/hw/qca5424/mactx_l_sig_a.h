
/*
 * Copyright (c) 2024, Qualcomm Innovation Center, Inc. All rights reserved.
 * SPDX-License-Identifier: ISC
 */

 

 
 
 
 
 
 
 
 


#ifndef _MACTX_L_SIG_A_H_
#define _MACTX_L_SIG_A_H_
#if !defined(__ASSEMBLER__)
#endif

#include "l_sig_a_info.h"
#define NUM_OF_DWORDS_MACTX_L_SIG_A 2

#define NUM_OF_QWORDS_MACTX_L_SIG_A 1


struct mactx_l_sig_a {
#ifndef WIFI_BIT_ORDER_BIG_ENDIAN
             struct   l_sig_a_info                                              mactx_l_sig_a_info_details;
             uint32_t tlv64_padding                                           : 32;  
#else
             struct   l_sig_a_info                                              mactx_l_sig_a_info_details;
             uint32_t tlv64_padding                                           : 32;  
#endif
};


 


 

#define MACTX_L_SIG_A_MACTX_L_SIG_A_INFO_DETAILS_RATE_OFFSET                        0x0000000000000000
#define MACTX_L_SIG_A_MACTX_L_SIG_A_INFO_DETAILS_RATE_LSB                           0
#define MACTX_L_SIG_A_MACTX_L_SIG_A_INFO_DETAILS_RATE_MSB                           3
#define MACTX_L_SIG_A_MACTX_L_SIG_A_INFO_DETAILS_RATE_MASK                          0x000000000000000f


 

#define MACTX_L_SIG_A_MACTX_L_SIG_A_INFO_DETAILS_LSIG_RESERVED_OFFSET               0x0000000000000000
#define MACTX_L_SIG_A_MACTX_L_SIG_A_INFO_DETAILS_LSIG_RESERVED_LSB                  4
#define MACTX_L_SIG_A_MACTX_L_SIG_A_INFO_DETAILS_LSIG_RESERVED_MSB                  4
#define MACTX_L_SIG_A_MACTX_L_SIG_A_INFO_DETAILS_LSIG_RESERVED_MASK                 0x0000000000000010


 

#define MACTX_L_SIG_A_MACTX_L_SIG_A_INFO_DETAILS_LENGTH_OFFSET                      0x0000000000000000
#define MACTX_L_SIG_A_MACTX_L_SIG_A_INFO_DETAILS_LENGTH_LSB                         5
#define MACTX_L_SIG_A_MACTX_L_SIG_A_INFO_DETAILS_LENGTH_MSB                         16
#define MACTX_L_SIG_A_MACTX_L_SIG_A_INFO_DETAILS_LENGTH_MASK                        0x000000000001ffe0


 

#define MACTX_L_SIG_A_MACTX_L_SIG_A_INFO_DETAILS_PARITY_OFFSET                      0x0000000000000000
#define MACTX_L_SIG_A_MACTX_L_SIG_A_INFO_DETAILS_PARITY_LSB                         17
#define MACTX_L_SIG_A_MACTX_L_SIG_A_INFO_DETAILS_PARITY_MSB                         17
#define MACTX_L_SIG_A_MACTX_L_SIG_A_INFO_DETAILS_PARITY_MASK                        0x0000000000020000


 

#define MACTX_L_SIG_A_MACTX_L_SIG_A_INFO_DETAILS_TAIL_OFFSET                        0x0000000000000000
#define MACTX_L_SIG_A_MACTX_L_SIG_A_INFO_DETAILS_TAIL_LSB                           18
#define MACTX_L_SIG_A_MACTX_L_SIG_A_INFO_DETAILS_TAIL_MSB                           23
#define MACTX_L_SIG_A_MACTX_L_SIG_A_INFO_DETAILS_TAIL_MASK                          0x0000000000fc0000


 

#define MACTX_L_SIG_A_MACTX_L_SIG_A_INFO_DETAILS_PKT_TYPE_OFFSET                    0x0000000000000000
#define MACTX_L_SIG_A_MACTX_L_SIG_A_INFO_DETAILS_PKT_TYPE_LSB                       24
#define MACTX_L_SIG_A_MACTX_L_SIG_A_INFO_DETAILS_PKT_TYPE_MSB                       27
#define MACTX_L_SIG_A_MACTX_L_SIG_A_INFO_DETAILS_PKT_TYPE_MASK                      0x000000000f000000


 

#define MACTX_L_SIG_A_MACTX_L_SIG_A_INFO_DETAILS_CAPTURED_IMPLICIT_SOUNDING_OFFSET  0x0000000000000000
#define MACTX_L_SIG_A_MACTX_L_SIG_A_INFO_DETAILS_CAPTURED_IMPLICIT_SOUNDING_LSB     28
#define MACTX_L_SIG_A_MACTX_L_SIG_A_INFO_DETAILS_CAPTURED_IMPLICIT_SOUNDING_MSB     28
#define MACTX_L_SIG_A_MACTX_L_SIG_A_INFO_DETAILS_CAPTURED_IMPLICIT_SOUNDING_MASK    0x0000000010000000


 

#define MACTX_L_SIG_A_MACTX_L_SIG_A_INFO_DETAILS_RESERVED_OFFSET                    0x0000000000000000
#define MACTX_L_SIG_A_MACTX_L_SIG_A_INFO_DETAILS_RESERVED_LSB                       29
#define MACTX_L_SIG_A_MACTX_L_SIG_A_INFO_DETAILS_RESERVED_MSB                       30
#define MACTX_L_SIG_A_MACTX_L_SIG_A_INFO_DETAILS_RESERVED_MASK                      0x0000000060000000


 

#define MACTX_L_SIG_A_MACTX_L_SIG_A_INFO_DETAILS_RX_INTEGRITY_CHECK_PASSED_OFFSET   0x0000000000000000
#define MACTX_L_SIG_A_MACTX_L_SIG_A_INFO_DETAILS_RX_INTEGRITY_CHECK_PASSED_LSB      31
#define MACTX_L_SIG_A_MACTX_L_SIG_A_INFO_DETAILS_RX_INTEGRITY_CHECK_PASSED_MSB      31
#define MACTX_L_SIG_A_MACTX_L_SIG_A_INFO_DETAILS_RX_INTEGRITY_CHECK_PASSED_MASK     0x0000000080000000


 

#define MACTX_L_SIG_A_TLV64_PADDING_OFFSET                                          0x0000000000000000
#define MACTX_L_SIG_A_TLV64_PADDING_LSB                                             32
#define MACTX_L_SIG_A_TLV64_PADDING_MSB                                             63
#define MACTX_L_SIG_A_TLV64_PADDING_MASK                                            0xffffffff00000000



#endif    
