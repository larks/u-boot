/*
** Command for programming SF2 fabric
*/

#include <common.h> /* includes m2s.h */
#include <spi_flash.h>

#include <asm/io.h>

#ifndef CONFIG_SF_DEFAULT_SPEED
# define CONFIG_SF_DEFAULT_SPEED	1000000
#endif
#ifndef CONFIG_SF_DEFAULT_MODE
# define CONFIG_SF_DEFAULT_MODE		SPI_MODE_3
#endif

//#include <mss_sys_services.h>
//#include <mss_comblk.h>
#define BUFFER_SIZE 4096

/* ISP related globals */
uint8_t g_page_buffer[BUFFER_SIZE];
uint32_t page_read_handler(uint8_t const ** pp_next_page);
void isp_completion_handler(uint32_t value);
static struct spi_flash *flash;
static long g_src_image_target_address = 0;
static long g_src_image_size = 0;
static uint32_t g_error_flag = 1;
volatile uint8_t g_isp_operation_busy = 1;



static int do_sf2prog_flash_probe(int argc, char *argv[])
{
	unsigned int bus = 0;
	unsigned int cs;
	unsigned int speed = CONFIG_SF_DEFAULT_SPEED;
	unsigned int mode = CONFIG_SF_DEFAULT_MODE;
	char *endp;
	struct spi_flash *new;

	if (argc < 2)
		goto usage;

	cs = simple_strtoul(argv[1], &endp, 0);
	if (*argv[1] == 0 || (*endp != 0 && *endp != ':'))
		goto usage;
	if (*endp == ':') {
		if (endp[1] == 0)
			goto usage;

		bus = cs;
		cs = simple_strtoul(endp + 1, &endp, 0);
		if (*endp != 0)
			goto usage;
	}

	if (argc >= 3) {
		speed = simple_strtoul(argv[2], &endp, 0);
		if (*argv[2] == 0 || *endp != 0)
			goto usage;
	}
	if (argc >= 4) {
		mode = simple_strtoul(argv[3], &endp, 16);
		if (*argv[3] == 0 || *endp != 0)
			goto usage;
	}

	new = spi_flash_probe(bus, cs, speed, mode);
	if (!new) {
		printf("Failed to initialize SPI flash at %u:%u\n", bus, cs);
		return 1;
	}

	if (flash)
		spi_flash_free(flash);
	flash = new;

	printf("%u KiB %s at %u:%u is now current device\n",
			flash->size >> 10, flash->name, bus, cs);

	return 0;

usage:
	puts("Usage: sf2isp probe [bus:]cs [hz] [mode]\n");
	return 1;
}

static int do_sf2isp_prog(int argc, char *argv[])
{
	//unsigned int isp_operation_busy = 0;
	char programming_mode[4];
	int error_flag;
	char *endptr;
	//unsigned long offset = 0;
	
	
	if(argc < 3)
		goto usage;
	/* Set programming mode */
	programming_mode = argv[0];
	if( strcmp(programming_mode, "auth") || strcmp(programming_mode, "prog") || strcmp(programming_mode, "verf") )
		goto usage;
	
	
	/* page read handler needs to use spi_flash_read? no? cannot use physical memory...*/
	
	
	MSS_SYS_init(MSS_SYS_NO_EVENT_HANDLER);
	
	switch((int)programming_mode[0])
	{
	case 'a':
		puts("Authentication...\n");
		g_isp_operation_busy = 1;
		g_src_image_target_address = strtoul(argv[1], &endptr, 0);
		g_src_image_size = strtoul(argv[2], &endptr, 0);	
		MSS_SYS_start_isp(MSS_SYS_PROG_AUTHENTICATE,page_read_handler,isp_completion_handler);
		while(g_isp_operation_busy){;}
		if(!g_isp_operation_busy){
			if(g_error_flag == MSS_SYS_SUCCESS){
				puts("ISP Authentication completed successfully\n");
				return 0;
			}
			else{
				puts("ISP Authentication failed with error ");
				putc(g_error_flag);
				return 1;
			}
		}
		break;
	case 'p':
		puts("Program...\n Checking data integrity...\n");
		g_isp_operation_busy = 1;
		g_src_image_target_address = strtoul(argv[1], &endptr, 0);
		g_src_image_size = strtoul(argv[2], &endptr, 0);
		MSS_SYS_start_isp(MSS_SYS_PROG_AUTHENTICATE,page_read_handler,isp_completion_handler);
		while(g_isp_operation_busy){;}
		if(!g_isp_operation_busy){
			if(g_error_flag == MSS_SYS_SUCCESS){
				puts("Authentication completed successfully. Performing Program operation...\n");
				MSS_SYS_init(MSS_SYS_NO_EVENT_HANDLER);
				g_isp_operation_busy = 1;
				g_src_image_target_address = strtoul(argv[1], &endptr, 0);
				g_src_image_size = strtoul(argv[2], &endptr, 0);
				MSS_SYS_start_isp(MSS_SYS_PROG_PROGRAM,page_read_handler,isp_completion_handler);
				while(g_isp_operation_busy){;}
				if(!g_isp_operation_busy){
					if(g_error_flag == MSS_SYS_SUCCESS){
						puts("Programming completed successfully. You should not be able to see this\n");
						return 0;
					}
					else{
						puts("Programming failed. You should still not be able to see this\n");
						return 1;
					}
				
				}
				
			}
		
		}
		else{
			puts("ISP Authentication failed, Program operation cannot be performed\n");
			return 1;
		}
		break;
	case 'v':
		puts("Verification...\n");
		g_isp_operation_busy = 1;
		g_src_image_target_address = strtoul(argv[1], &endptr, 0);
		g_src_image_size = strtoul(argv[2], &endptr, 0);
		MSS_SYS_start_isp(MSS_SYS_PROG_VERIFY,page_read_handler,isp_completion_handler);
		while(g_isp_operation_busy){;}
		if(!g_isp_operation_busy){
			if(g_error_flag == MSS_SYS_SUCCESS){
				puts("ISP verification completed successfully\n");
			}
			else{
				puts("ISP verification failed with error ");
				putc(g_error_flag);
				return 1;
			}
		}
		break;
	default:
		puts("ISP command is not recognized\n");
		break;
	}
	return 0;
	
usage:
	puts("Usage: sf2isp MODE ADDRESS IMAGESIZE\n");
	return 1;
}

void isp_completion_handler(uint32_t value)
{
	g_error_flag = value;
	g_isp_operation_busy = 0;
}

static uint32_t read_page_from_flash(uint8_t * g_buffer, uint32_t length)
{
	uint32_t num_bytes;
	int ret;
	num_bytes = length;
	
	g_buffer = map_physmem(addr, num_bytes, MAP_WRBACK);
	if (!g_buffer) {
		puts("Failed to map physical memory\n");
		return 1;
	}
	ret = spi_flash_read(flash, g_src_image_target_address, num_bytes, g_buffer);
	unmap_physmem(g_buffer, num_bytes);
	if (ret) {
		printf("SPI flash %s failed\n", argv[0]);
	return 1;
	}
	g_src_image_target_address += num_bytes;
	
	return num_bytes;

}

/* function called by COMM_BLK for input data bit stream*/
uint32_t page_read_handler
(
    uint8_t const ** pp_next_page
)
{
    uint32_t length;

    length = read_page_from_flash(g_page_buffer, BUFFER_SIZE);
    *pp_next_page = g_page_buffer;

    return length;
}


/* main function for checking commands */
static int do_sf2isp(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	const char *cmd;
	
	/* Need at least two arguments */
	if(argc<2)
		goto usage;
	
	cmd = arv[1];
	
	if(!strcmp(cmd, "probe")){
		return do_spi_flash_probe(argc-1, argv+1);
	}
	
	/* Remaining commands requires a selected device */
	if(!flash){
		puts("No SPI flash selected. Please run `sf2isp probe'\n");
		return 1;
	}
	if( (!strcmp(cmd, "auth")) || (!strcmp(cmd, "prog")) || (!strcmp(cmd, "verf")) )
		return do_sf2isp_prog(argc-1, argv+1);
		
usage:
	cmd_usage(cmdtp);
	return 1;

}



/* U-boot command definition */
U_BOOT_CMD(
	sf2isp, 4, 1, do_sf2isp,
	"SmartFusion2 In-System Programming",
	"probe [bus:]cs [hz] [mode]	- init flash device on given SPI bus\n"
	"				  and chip select\n"
	"sf2isp auth offset len 	- Authenticate bitstream starting at\n"
	"                     	  `offset' with length `len'\n"
	"sf2isp prog offset len 	- Program fabric with bitstream starting at\n"
	"                     	  `offset' with length `len'\n"
	"sf2isp verf offset len 	- Verify bitstream starting at `offset'\n"
	"                     	  with length `len'\n"
);


