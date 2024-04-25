#include "rp2040/gpio/gpio.h"
#include "rp2040/ssi/ssi.h"
#include "rp2040/rp2040.h"
#include "rp2040/m0plus.h"


#define CMD_READ             0xEB
#define MODE_CONTINUOUS_READ 0xA0
#define ADDR_L               8
#define WAIT_CYCLES          4


void __attribute__((section(".boot_stage2"))) boot_stage2_init_xip(void) {
#ifndef EDU_PRUNE_1
    // Set pad configuration:
    // - SCLK 8mA drive, no slew limiting
    *RP2040_GPIO_PADS_QSPI_SCLK = RP2040_GPIO_PADS_QSPI_DRIVE_BITS(RP2040_GPIO_PADS_QSPI_DRIVE_8MA) |
                                  RP2040_GPIO_PADS_QSPI_SLEWFAST_BITS(RP2040_GPIO_PADS_QSPI_SLEWFAST_FAST);
    // - SDx disable input Schmitt to reduce delay. A Schmitt trigger adds hysteresis to the analog signal
    *RP2040_CLEAR(RP2040_GPIO_PADS_QSPI_SD0) = RP2040_GPIO_PADS_QSPI_SCHMITT_BITS(1);
    *RP2040_CLEAR(RP2040_GPIO_PADS_QSPI_SD1) = RP2040_GPIO_PADS_QSPI_SCHMITT_BITS(1);
    *RP2040_CLEAR(RP2040_GPIO_PADS_QSPI_SD2) = RP2040_GPIO_PADS_QSPI_SCHMITT_BITS(1);
    *RP2040_CLEAR(RP2040_GPIO_PADS_QSPI_SD3) = RP2040_GPIO_PADS_QSPI_SCHMITT_BITS(1);
#endif

    // Unnecessary, it is 0 at reset
    *RP2040_SSI_SSIENR = 0;


// The serial flash interface will run at clk_sys/PICO_FLASH_SPI_CLKDIV.
// This must be a positive, even integer.
// The bootrom is very conservative with SPI frequency, but here we should be
// as aggressive as possible.
#define PICO_FLASH_SPI_CLKDIV 4
    *RP2040_SSI_BAUDR = PICO_FLASH_SPI_CLKDIV;

#ifndef EDU_PRUNE_1
    // Set 1-cycle sample delay. If PICO_FLASH_SPI_CLKDIV == 2 then this means,
    // if the flash launches data on SCLK posedge, we capture it at the time that
    // the next SCLK posedge is launched. This is shortly before that posedge
    // arrives at the flash, so data hold time should be ok. For
    // PICO_FLASH_SPI_CLKDIV > 2 this pretty much has no effect.
    *RP2040_SSI_RX_SAMPLE_DLY = 1;
#endif

#ifndef EDU_PRUNE_2
    // Currently the flash expects an 8 bit serial command prefix on every
    // transfer, which is a waste of cycles. Perform a dummy Fast Read Quad I/O
    // command, with mode bits set such that the flash will not expect a serial
    // command prefix on *subsequent* transfers. We don't care about the results
    // of the read, the important part is the mode bits.
    // dummy_read:
    *RP2040_SSI_CTRLR0 = RP2040_SSI_CTRLR0_SPI_FRF_BITS(RP2040_SSI_SPI_CTRLR0_SPI_FRF_QUAD) |
                         RP2040_SSI_CTRLR0_DFS_32_BITS(31) |
                         RP2040_SSI_CTRLR0_TMOD_BITS(RP2040_SSI_CTRLR0_TMOD_EEPROM_READ);
    *RP2040_SSI_CTRLR1 = 0;

    *RP2040_SSI_SPI_CTRLR0 =
        RP2040_SSI_SPI_CTRLR0_ADDR_L_BITS(ADDR_L) | RP2040_SSI_SPI_CTRLR0_WAIT_CYCLES_BITS(WAIT_CYCLES) |
        RP2040_SSI_SPI_CTRLR0_INST_L_BITS(RP2040_SSI_SPI_CTRLR0_INST_L_8_BITS) |
        RP2040_SSI_SPI_CTRLR0_TRANS_TYPE_BITS(RP2040_SSI_SPI_CTRLR0_TRANS_TYPE_CMD_STANDARD_ADDR_FRF);
    *RP2040_SSI_SSIENR = 1;

    // Push SPI command into TX FIFO
    *RP2040_SSI_DR0 = CMD_READ;
    // Push Address into TX FIFO - this will trigger the transaction
    *RP2040_SSI_DR0 = MODE_CONTINUOUS_READ;     // 32-bit: 24 address bits (we don't care, so 0) and M[7:4]=1010

    // wait_ssi_ready
    for (;;) {
        if ((*RP2040_SSI_SR & RP2040_SSI_SR_TFE_BITS(1)) > 0) {
            if ((*RP2040_SSI_SR & RP2040_SSI_SR_BUSY_BITS(1)) == 0) {
                break;
            }
        }
    }

    // The flash is in a state where we can blast addresses in parallel, and get
    // parallel data back. Now configure the SSI to translate XIP bus accesses
    // into QSPI transfers of this form.
    *RP2040_SSI_SSIENR = 0;     // Disable SSI (and clear FIFO) to allow further config
#endif

    *RP2040_SSI_SPI_CTRLR0 = RP2040_SSI_SPI_CTRLR0_XIP_CMD_BITS(MODE_CONTINUOUS_READ) |
                             RP2040_SSI_SPI_CTRLR0_ADDR_L_BITS(ADDR_L) |
                             RP2040_SSI_SPI_CTRLR0_WAIT_CYCLES_BITS(WAIT_CYCLES) |
                             RP2040_SSI_SPI_CTRLR0_INST_L_BITS(RP2040_SSI_SPI_CTRLR0_INST_L_NO_INSTRUCTION) |
                             RP2040_SSI_SPI_CTRLR0_TRANS_TYPE_BITS(RP2040_SSI_SPI_CTRLR0_TRANS_TYPE_CMD_ADDR_FRF);
    *RP2040_SSI_SSIENR = 1;
    // Bus accesses to the XIP window will now be transparently serviced by the
    // external flash on cache miss. We are ready to run code from flash.

    uint32_t app_vector_table_address           = 0x10000100;
    *RP2040_M0PLUS_VTOR                         = app_vector_table_address;
    volatile uint32_t *app_vector_table_pointer = (volatile uint32_t *)(uintptr_t)app_vector_table_address;

    __asm("msr msp, %0" : : "r"(app_vector_table_pointer[0]));
    __asm("bx %0" : : "r"(app_vector_table_pointer[1]));
}
