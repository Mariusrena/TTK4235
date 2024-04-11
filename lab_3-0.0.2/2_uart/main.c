#include <stdio.h>
#include <sys/types.h>



#include "gpio.h"
#include "uart.h"


ssize_t _write (int fd, const void *buf, size_t count) {
    char * letter = (char *)(buf);
    for (int i = 0; i < count; i++) {
        uart_send(*letter);
        letter++;
    }
    return count;
}

int main () {
    uart_init();

    // Configure buttons
	GPIO->PIN_CNF[13] = (3 << 2);
	GPIO->PIN_CNF[14] = (3 << 2);
    GPIO->PIN_CNF[15] = (3 << 2);

	int sleep = 0;
	while(1){
        
		/* Check if button 1 is pressed */
		if (!(GPIO->IN & (1 << 13))) {
			uart_send('A');
		}

		/* Check if button 2 is pressed */
		if (!(GPIO->IN & (1 << 14))) {
			uart_send('B');
		}

		sleep = 10000;
		while(--sleep); // Delay
        
        if (uart_read() != '\0'){
            if (GPIO->PIN_CNF[17]==1)
            {
                for (int i = 17; i < 21; i++) {
				GPIO->PIN_CNF[i] = 0;
			}
            }else{
                for (int i = 17; i < 21; i++) {
				GPIO->PIN_CNF[i] = 1;
			}}
        }

        if (!(GPIO->IN & (1 << 15))) {
			iprintf("The average grade in TTK%d was in %d was: %c\n\r", 4235, 2022, 'B');
		}
        
	}
	return 0;
}