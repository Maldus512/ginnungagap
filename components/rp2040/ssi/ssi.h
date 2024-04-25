#ifndef SSI_H_INCLUDED
#define SSI_H_INCLUDED


#include "../_common.h"


#define RP2040_SSI_BASE          (0x18000000)
#define RP2040_SSI_CTRLR0        MMIO32(RP2040_SSI_BASE + 0x00)
#define RP2040_SSI_CTRLR1        MMIO32(RP2040_SSI_BASE + 0x04)
#define RP2040_SSI_SSIENR        MMIO32(RP2040_SSI_BASE + 0x08)
#define RP2040_SSI_BAUDR         MMIO32(RP2040_SSI_BASE + 0x14)
#define RP2040_SSI_SR            MMIO32(RP2040_SSI_BASE + 0x28)
#define RP2040_SSI_DR0           MMIO32(RP2040_SSI_BASE + 0x60)
#define RP2040_SSI_RX_SAMPLE_DLY MMIO32(RP2040_SSI_BASE + 0xF0)
#define RP2040_SSI_SPI_CTRLR0    MMIO32(RP2040_SSI_BASE + 0xF4)


#define RP2040_SSI_CTRLR0_FRF_BITS(Value)     (((Value)&0x3) << 4)
#define RP2040_SSI_CTRLR0_TMOD_BITS(Value)    (((Value)&0x03) << 8)
#define RP2040_SSI_CTRLR0_DFS_32_BITS(Value)  (((Value)&0x1F) << 16)
#define RP2040_SSI_CTRLR0_SPI_FRF_BITS(Value) (((Value)&0x3) << 21)

#define RP2040_SSI_SPI_CTRLR0_TRANS_TYPE_BITS(Value)  (((Value)&0x03) << 0)
#define RP2040_SSI_SPI_CTRLR0_ADDR_L_BITS(Value)      (((Value)&0x0F) << 2)
#define RP2040_SSI_SPI_CTRLR0_INST_L_BITS(Value)      (((Value)&0x03) << 8)
#define RP2040_SSI_SPI_CTRLR0_WAIT_CYCLES_BITS(Value) (((Value)&0x1F) << 11)
#define RP2040_SSI_SPI_CTRLR0_XIP_CMD_BITS(Value)     (((Value)&0xFF) << 24)

#define RP2040_SSI_SR_BUSY_BITS(Value) (((Value)&0x1) << 0)
#define RP2040_SSI_SR_TFE_BITS(Value)  (((Value)&0x1) << 2)



typedef enum {
    RP2040_SSI_CTRLR0_TMOD_TX_AND_RX   = 0x00,
    RP2040_SSI_CTRLR0_TMOD_TX_ONLY     = 0x01,
    RP2040_SSI_CTRLR0_TMOD_RX_ONLY     = 0x02,
    RP2040_SSI_CTRLR0_TMOD_EEPROM_READ = 0x03,
} rp2040_ssi_ctrlr0_tmod_t;

typedef enum {
    RP2040_SSI_SPI_CTRLR0_INST_L_NO_INSTRUCTION = 0x00,
    RP2040_SSI_SPI_CTRLR0_INST_L_4_BITS         = 0x01,
    RP2040_SSI_SPI_CTRLR0_INST_L_8_BITS         = 0x02,
    RP2040_SSI_SPI_CTRLR0_INST_L_16_BITS        = 0x03,
} rp2040_ssi_spi_ctrlr0_inst_l_t;

typedef enum {
    RP2040_SSI_SPI_CTRLR0_SPI_FRF_SINGLE = 0x0,
    RP2040_SSI_SPI_CTRLR0_SPI_FRF_DUAL   = 0x1,
    RP2040_SSI_SPI_CTRLR0_SPI_FRF_QUAD   = 0x2,
} rp2040_ssi_spi_ctrlr0_spi_frf_t;

typedef enum {
    RP2040_SSI_SPI_CTRLR0_TRANS_TYPE_CMD_ADDR_STANDARD     = 0x0,
    RP2040_SSI_SPI_CTRLR0_TRANS_TYPE_CMD_STANDARD_ADDR_FRF = 0x1,
    RP2040_SSI_SPI_CTRLR0_TRANS_TYPE_CMD_ADDR_FRF          = 0x2,
} rp2040_ssi_spi_ctrlr0_trans_type_t;


#endif
