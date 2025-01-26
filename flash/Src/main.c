#define STM32C011xx
#include <stm32c0xx.h>


// before the very first start, the flash memory should be erased, e.g. by using STM32CubeProgrammer
// in production code, counter variable validity should be checked by adding a checksum  
// the counter variable is stored in the last page of the flash memory
// the linker description (.ld) file was modified to keep the last page of the flash memory untouched


// the flash memory is divided into pages, each page has a size of 2KB
#define PAGE_SIZE   2048ul

#define MY_PAGE_NR     ((FLASH_SIZE_DEFAULT)/(PAGE_SIZE)-1u)      // last page of the flash memory


const uint32_t address_of_counter = FLASH_BASE + MY_PAGE_NR * PAGE_SIZE;


// the flash memory is write protected, to write to the flash memory, the flash memory must be unlocked
void flash_unlock(void) {
    if(FLASH->CR & FLASH_CR_LOCK) {
        FLASH->KEYR = 0x45670123;
        FLASH->KEYR = 0xCDEF89AB;
    }
}


// the flash memory is write protected, and should be locked after writing to the flash memory
void flash_lock(void) {
    FLASH->CR |= FLASH_CR_LOCK;
}


// Flash memory page erase, flash must be unlocked
void flash_erase_page(uint32_t page_nr) {
    while(FLASH->SR & FLASH_SR_BSY1);   // wait until flash is not busy
    // Check and clear all error programming flags, If not, PGSERR is set.
    FLASH->CR |= FLASH_CR_PER;  		// begin page erase
    FLASH->CR = (FLASH->CR &~FLASH_CR_PNB_Msk) | (page_nr << FLASH_CR_PNB_Pos);
    FLASH->CR |= FLASH_CR_STRT;
    while(FLASH->SR & FLASH_SR_BSY1);   	// wait until flash is not busy
    FLASH->CR &= ~FLASH_CR_PER; 		// end page erase
}


// Standard programming procedure, flash must be unlocked
void flash_programn_doubleword(uint32_t address, uint64_t data) {
    while(FLASH->SR & FLASH_SR_BSY1);   	// wait until flash is not busy
    // Check and clear all error programming flags, If not, PGSERR is set.
    FLASH->CR |= FLASH_CR_PG;   		    // begin programming
    *(volatile uint64_t*)address = data;    // write the data
    while(FLASH->SR & FLASH_SR_BSY1);   	// wait until flash is not busy
    // Check that the EOP flag is set in FLASH->SR
    // clear the PG flag after the last 64-bit doubleword has been programmed 
    FLASH->CR &=~FLASH_CR_PG;   		// end programming
}


// step thru the code and use cortex-debug variables/watch to watch the counter variable
int main(void)
{
    // reading from flash memory works like reading from RAM
    uint64_t counter = *(volatile uint64_t*)address_of_counter;
    counter++;

    // writing to flash memory is a bit more complicated
    flash_unlock();
    flash_erase_page(MY_PAGE_NR);
    flash_programn_doubleword(address_of_counter, counter);
    flash_lock();
    
    /* Loop forever */
    for(;;);
}
