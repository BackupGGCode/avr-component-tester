/*
With the DEFINE SWUART_INVERT it can be specified whether the software UART sends normally or inverted. 
In the normal mode of operation the UART in the usual logic (Low = 0, High = 1). 
That is the standard case and serves for the connection to another µC, a USB=> Serial e.g. MAX232.

In the inverted mode of operation the UART sends reverse logic (High = 0 and Low = 1 )
That corresponds to the logic of the RS232-  Interface of Standard-PCs. 

In most cases the TxD of the software UART can be connected thus directly with the RxD of the PC. 
According to specification that is inadmissible, because for a RS232- Interface levels are undefined of -3V? 3V.
At most PCs it functions however nevertheless without problems. 
That is a simple, but somewhat careless solution?

Is SWUART_INVERT defined, the UART in inverted mode of operation works.
*/
#define SWUART_INVERT

#define TxD PC3	//TxD-Pin the software UART; must be at port C!

