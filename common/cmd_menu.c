/*
 * the menu of u-boot by ll 2012-11-01
 */

#include <common.h>
#include <command.h>
#include <def.h>
#include <nand.h>

extern char console_buffer[];
extern int readline (const char *const prompt);
extern int boot_zImage(ulong from, size_t size);
extern int bBootFrmNORFlash();

#define USE_TFTP_DOWN		1
#define USE_USB_DOWN		2

/**功能：等待键盘输入***/
static char awaitkey(unsigned long delay, int* error_p)
{
	int i;
	char c;
	if (delay == -1) {
		while (1) {
			if (tstc()) /* we got a key press */
				return getc();
		}
	} else {
		for (i = 0; i < delay; i++) {
			if (tstc()) /* we got a key press */
				return getc();
			udelay (10*1000);
		}
	}
	if (error_p)
		*error_p = -1;
	return 0;
}

void main_menu_usage(char menu_type)
{

	if (bBootFrmNORFlash())
		printf("\r\n#####	 Boot for Nor Flash Main Menu	#####\r\n");
	else
		printf("\r\n#####	 Boot for Nand Flash Main Menu	#####\r\n");

	if( menu_type == USE_USB_DOWN)
	{
		printf("#####     LL USB download mode     #####\r\n\n");
	}
	else if( menu_type == USE_TFTP_DOWN)
	{
		printf("#####     LL TFTP download mode     #####\r\n\n");
	}


	if( menu_type == USE_USB_DOWN)
	{
		printf("[1] Download u-boot or STEPLDR.nb1 or other bootloader to Nand Flash\r\n");
	}
	else if( menu_type == USE_TFTP_DOWN)
	{
		printf("[1] Download u-boot.bin to Nand Flash\r\n");
	}
	printf("[3] Download Linux Kernel (zImage.bin) to Nand Flash\r\n");
	if( menu_type == USE_USB_DOWN)
	{
	//	printf("[5] Download CRAMFS image to Nand Flash\r\n");
	}
	else if( menu_type == USE_TFTP_DOWN)
	{
		printf("[5] Set TFTP parameters(PC IP,TQ2440 IP,Mask IP...)\r\n");
	}
	printf("[6] Download YAFFS image (root.bin) to Nand Flash\r\n");
	printf("[7] Download Program to SDRAM and Run it\r\n");

	printf("[8] Boot the system\r\n");
	printf("[9] Format the Nand Flash\r\n");
	if( menu_type == USE_USB_DOWN)
	{
		// printf("[n] Enter TFTP download mode menu \r\n");
	}

	if (bBootFrmNORFlash())
		// printf("[o] Download u-boot to Nor Flash\r\n");

	if( menu_type == USE_TFTP_DOWN) {
		printf("[p] Test network (TQ2440 Ping PC's IP) \r\n");
		// printf("[u] Enter USB download mode menu \r\n");
	}

	printf("[r] Reboot u-boot\r\n");
	printf("[t] Test Linux Image (zImage)\r\n");
	printf("[q] quit from this menu\r\n");

	printf("Enter your selection: ");
}
void usb_menu_shell(void)
{
	char c;
	char cmd_buf[200];

	while (1)
	{
		main_menu_usage(USE_USB_DOWN);
		c = awaitkey(-1, NULL);
		printf("%c\n", c);
		switch (c)
		{
		case '1':
			{
				strcpy(cmd_buf, "usbslave 1 0x30000000; nand erase bios; nand write.jffs2 0x30000000 bios $(filesize)");
				printf("cmd:\"%s\"\n",cmd_buf);
				run_command(cmd_buf, 0);
				break;
			}
		case '3':
			{
				strcpy(cmd_buf, "usbslave 1 0x30000000; nand erase kernel; nand write.jffs2 0x30000000 kernel $(filesize)");
				printf("cmd:\"%s\"\n",cmd_buf);
				run_command(cmd_buf, 0);
				break;
			}
		case '6':
			{
				strcpy(cmd_buf, "usbslave 1 0x30000000; nand erase root; nand write.yaffs 0x30000000 root $(filesize)");
				printf("cmd:\"%s\"\n",cmd_buf);
				run_command(cmd_buf, 0);
				break;
			}
		case '7':
			{
				extern volatile U32 downloadAddress;
				extern int download_run;

				download_run = 1;
				strcpy(cmd_buf, "usbslave 1");
				printf("cmd:\"%s\"\n",cmd_buf);
				run_command(cmd_buf, 0);
				download_run = 0;
				sprintf(cmd_buf, "go %x", downloadAddress);
				run_command(cmd_buf, 0);
				break;
			}
		case '8':
			{
				printf("Start Linux ...\n");
				strcpy(cmd_buf, "boot_zImage");
				printf("cmd:\"%s\"\n",cmd_buf);
				run_command(cmd_buf, 0);
				break;
			}

		case '9':
			{
				strcpy(cmd_buf, "nand scrub ");
				printf("cmd:\"%s\"\n",cmd_buf);
				run_command(cmd_buf, 0);
				//erase_menu_shell();
				break;
			}
		case 'R':
		case 'r':
			{
				strcpy(cmd_buf, "reset");
				printf("cmd:\"%s\"\n",cmd_buf);
				run_command(cmd_buf, 0);
				break;
			}

		case 'T':
		case 't':
			{
				strcpy(cmd_buf, "usbslave 1 0x30008000; test_zImage");
				printf("cmd:\"%s\"\n",cmd_buf);
				run_command(cmd_buf, 0);
				break;
			}

		case 'Q':
		case 'q':
			{
				return;
				break;
			}

		}
	}
}

void tftp_menu_shell(void)
{
	char c;
	char cmd_buf[200];

	while (1)
	{
		main_menu_usage(USE_TFTP_DOWN);
		c = awaitkey(-1, NULL);
		printf("%c\n", c);
		switch (c)
		{
		case '1':
			{
				strcpy(cmd_buf, "tftp 0x30000000 u-boot.bin; nand erase bios; nand write.jffs2 0x30000000 bios $(filesize)");
				printf("cmd:\"%s\"\n",cmd_buf);
				run_command(cmd_buf, 0);
				break;
			}

		case '3':
			{
				strcpy(cmd_buf, "tftp 0x30000000 zImage.bin; nand erase kernel; nand write.jffs2 0x30000000 kernel $(filesize)");
				printf("cmd:\"%s\"\n",cmd_buf);
				run_command(cmd_buf, 0);
				break;
			}

		case '5':
			{
				char param_buf1[25];
				char param_buf2[25];
				char param_buf3[25];

				printf("Enter the TFTP Server(PC) IP address:(xxx.xxx.xxx.xxx)\n");
				readline(NULL);
				strcpy(param_buf1,console_buffer);
				sprintf(cmd_buf, "setenv serverip %s",param_buf1);
				run_command(cmd_buf, 0);

				printf("Enter the SKY2440/TQ2440 IP address:(xxx.xxx.xxx.xxx)\n");
				readline(NULL);
				strcpy(param_buf2,console_buffer);
				sprintf(cmd_buf, "setenv ipaddr %s",param_buf2);
				run_command(cmd_buf, 0);

				printf("Enter the Mask IP address:(xxx.xxx.xxx.xxx)\n");
				readline(NULL);
				strcpy(param_buf3,console_buffer);
				sprintf(cmd_buf, "setenv netmask %s",param_buf3);
				run_command(cmd_buf, 0);

				printf("Save TFTP IP parameters?(y/n)\n");
				if (getc() == 'y' )
				{
					printf("y");
					getc() == '\r';
					printf("\n");
					sprintf(cmd_buf, "saveenv");
					run_command(cmd_buf, 0);
				}
				else
				{
					printf("Not Save it!!!\n");
				}
				break;
			}

		case '6':
			{
				strcpy(cmd_buf, "tftp 0x30000000 root.bin; nand erase root; nand write.yaffs 0x30000000 root $(filesize)");
				printf("cmd:\"%s\"\n",cmd_buf);
				run_command(cmd_buf, 0);
				break;
			}

		case '7':
			{
				char tftpaddress[12];
				char filename[32];

				printf("Enter downloads to SDRAM address:\n");
				readline(NULL);
				strcpy(tftpaddress, console_buffer);

				printf("Enter program name:\n");
				readline(NULL);
				strcpy(filename, console_buffer);

				sprintf(cmd_buf, "tftp %s %s", tftpaddress, filename);
				printf("tftp %s %s\n", tftpaddress, filename);
				run_command(cmd_buf, 0);

				sprintf(cmd_buf, "go %s", tftpaddress);
				run_command(cmd_buf, 0);
				break;
			}
			case '8':
			{
				printf("Start Linux ...\n");
				strcpy(cmd_buf, "boot_zImage");
				printf("cmd:\"%s\"\n",cmd_buf);
				run_command(cmd_buf, 0);
				break;
			}

			case '9':
			{
				strcpy(cmd_buf, "nand scrub ");
				printf("cmd:\"%s\"\n",cmd_buf);
				run_command(cmd_buf, 0);
				//erase_menu_shell();
				break;
			}

			case 'P':
			case 'p':
			{
				char *serverip;
				serverip=getenv("serverip");
				printf("TQ2440 ping PC IP:ping %s\n",serverip);
				sprintf(cmd_buf, "ping %s",serverip);
				run_command(cmd_buf, 0);
				break;
			}

		case 'R':
		case 'r':
			{
				strcpy(cmd_buf, "reset");
				run_command(cmd_buf, 0);
				break;
			}

		case 'T':
		case 't':
			{
				strcpy(cmd_buf, "tftp 0x30008000 zImage.bin; test_zImage");
				printf("cmd:\"%s\"\n",cmd_buf);
				run_command(cmd_buf, 0);
				break;
			}

		case 'Q':
		case 'q':
			{
				return;
				break;
			}
		}
	}

}
void menu_shell(void)
{
	char c;
	while (1)
	{
		printf("\r\n#####		Main Menu	#####\r\n\n");
		printf("[u] Enter USB download mode menu \r\n");
		printf("[n] Enter TFTP download mode menu \r\n");
		printf("[q] quit from menu \r\n\n");
		printf("Enter your selection: ");

		c = awaitkey(-1, NULL);
		printf("%c\n", c);
		switch (c)
		{
		case 'u':
		case 'U':
			{
				usb_menu_shell();
				break;
			}
		case 'n':
		case 'N':
			{
				tftp_menu_shell();
				break;
			}
		case 'Q':
		case 'q':
			{
				return;
				break;
			}
		}
	}
}

int do_menu (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	menu_shell();
	return 0;
}

U_BOOT_CMD(
		   menu,	3,	0,	do_menu,
		   "menu - display a menu, to select the items to do something\n",
		   " - display a menu, to select the items to do something"
		  );

