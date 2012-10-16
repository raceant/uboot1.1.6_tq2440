#include <common.h>
#include <s3c2410.h>

#define GSTATUS1        (*(volatile unsigned int *)0x560000B0)

static inline void delay (unsigned long loops)
{
	__asm__ volatile ("1:\n"
			"subs %0, %1, #1\n"
			"bne 1b":"=r" (loops):"0" (loops));
}

/* S3C2440: Mpll = (2*m * Fin) / (p * 2^s), UPLL = (m * Fin) / (p * 2^s)
 * m = M (the value for divider M)+ 8, p = P (the value for divider P) + 2
 */
/* Fin = 12.0000MHz */
#define S3C2440_MPLL_400MHZ	((0x5c<<12)|(0x01<<4)|(0x01))						//HJ 400MHz
#define S3C2440_MPLL_405MHZ	((0x7f<<12)|(0x02<<4)|(0x01))						//HJ 405MHz
#define S3C2440_MPLL_440MHZ	((0x66<<12)|(0x01<<4)|(0x01))						//HJ 440MHz
#define S3C2440_MPLL_480MHZ	((0x98<<12)|(0x02<<4)|(0x01))						//HJ 480MHz
#define S3C2440_MPLL_200MHZ	((0x5c<<12)|(0x01<<4)|(0x02))
#define S3C2440_MPLL_100MHZ	((0x5c<<12)|(0x01<<4)|(0x03))

#define S3C2440_UPLL_48MHZ	((0x38<<12)|(0x02<<4)|(0x02))						//HJ 100MHz

#define S3C2440_CLKDIV		0x05    /* FCLK:HCLK:PCLK = 1:4:8, UCLK = UPLL */		//HJ 100MHz
#define S3C2440_CLKDIV136	0x07    /* FCLK:HCLK:PCLK = 1:3:6, UCLK = UPLL */		//HJ 133MHz
#define S3C2440_CLKDIV188	0x04    /* FCLK:HCLK:PCLK = 1:8:8 */
#define S3C2440_CAMDIVN188	((0<<8)|(1<<9)) /* FCLK:HCLK:PCLK = 1:8:8 */

/* Fin = 16.9344MHz */
#define S3C2440_MPLL_399MHz     		((0x6e<<12)|(0x03<<4)|(0x01))
#define S3C2440_UPLL_48MHZ_Fin16MHz		((60<<12)|(4<<4)|(2))

void clock_init(void)
{
	S3C24X0_CLOCK_POWER *clk_power = (S3C24X0_CLOCK_POWER *)0x4C000000;

	/* FCLK:HCLK:PCLK = ?:?:? */
#if CONFIG_133MHZ_SDRAM
	clk_power->CLKDIVN = S3C2440_CLKDIV136;			//HJ 1:3:6
#else
	clk_power->CLKDIVN = S3C2440_CLKDIV;				//HJ 1:4:8
#endif
	/* change to asynchronous bus mod */
	__asm__(    "mrc    p15, 0, r1, c1, c0, 0\n"    /* read ctrl register   */
                    "orr    r1, r1, #0xc0000000\n"      /* Asynchronous         */
                    "mcr    p15, 0, r1, c1, c0, 0\n"    /* write ctrl register  */
                    :::"r1"
                    );

	/* to reduce PLL lock time, adjust the LOCKTIME register */
	clk_power->LOCKTIME = 0xFFFFFF;

	/* configure UPLL */
	clk_power->UPLLCON = S3C2440_UPLL_48MHZ;		//fin=12.000MHz
//	clk_power->UPLLCON = S3C2440_UPLL_48MHZ_Fin16MHz;	//fin=16.934MHz

	/* some delay between MPLL and UPLL */
	delay (4000);

	/* configure MPLL */
	clk_power->MPLLCON = S3C2440_MPLL_400MHZ;		//fin=12.000MHz
//	clk_power->MPLLCON = S3C2440_MPLL_405MHZ;				//HJ 405MHz
//	clk_power->MPLLCON = S3C2440_MPLL_440MHZ;				//HJ 440MHz
//	clk_power->MPLLCON = S3C2440_MPLL_480MHZ;				//HJ 480MHz
//	clk_power->MPLLCON = S3C2440_MPLL_399MHz;		//fin=16.934MHz
	/* some delay between MPLL and UPLL */
	delay (8000);
}

