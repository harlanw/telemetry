#include <getopt.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <libftdi1/ftdi.h>

#define VENDOR_ID	0x403
#define PRODUCT_ID	0x6001

#define ERROR(fmt, ...)	fprintf(stderr, "error: " fmt, ##__VA_ARGS__)

int
main(int argc, char *argv[])
{
	(void) argc;
	(void) argv;

	struct ftdi_context *ftdi;
	struct ftdi_version_info version;
	int status;

	enum ftdi_interface iface;
	int vid, pid;
	int baudrate;

	char buffer[1024];

	iface = INTERFACE_ANY;
	vid = VENDOR_ID;
	pid = PRODUCT_ID;
	//baudrate = 1200; 51
	baudrate = 9600;

	int i;
	while ((i = getopt(argc, argv, "i:v:p:b:")) != -1)
	{
		switch (i)
		{
			case 'i':
				iface = atoi(optarg);
				break;
			case 'v':
				vid = atoi(optarg);
				break;
			case 'p':
				pid = atoi(optarg);
				break;
			default:
				ERROR("usage: %s [-i interface] [-v vendor] [-p product] [-b baudrate]\n", *argv);

				return EXIT_FAILURE;
		}
	}

	ftdi = ftdi_new();
	if (ftdi == NULL)
	{
		ERROR("error: ftdi_new() failed\n");

		return EXIT_FAILURE;
	}

	version = ftdi_get_library_version();
	printf("initialized libftdi %d.%d.%d-%s\n",
		version.major,
		version.minor,
		version.micro,
		version.snapshot_str);

	ftdi_set_interface(ftdi, iface);
	status = ftdi_usb_open(ftdi, vid, pid);
	if (status < 0)
	{
		ERROR("error: unable to open device\nreason: %s\nstatus: %d\n",
			ftdi_get_error_string(ftdi),
			status);
		ftdi_free(ftdi);

		return EXIT_FAILURE;
	}

	if (ftdi->type == TYPE_R)
	{
		unsigned int chipid;

		ftdi_read_chipid(ftdi, &chipid);
		printf("Chip ID: %X\n", chipid);
	}

	status = ftdi_set_baudrate(ftdi, baudrate);
	if (status < 0)
	{
		ERROR("unable to set baudrate:\nreason: %s\nstatus: %d\n",
			ftdi_get_error_string(ftdi),
			status);
		ftdi_free(ftdi);

		return EXIT_FAILURE;
	}

	status = ftdi_set_line_property(ftdi, BITS_8, STOP_BIT_1, NONE);
	if (status < 0)
	{
		ERROR("unaable to set line paremeters:\nreason %s\nstatus: %d\n",
			ftdi_get_error_string(ftdi),
			status);
		ftdi_free(ftdi);

		return EXIT_FAILURE;
	}

	while (1)
	{
		status = ftdi_read_data(ftdi, buffer, sizeof(buffer)); 

		if (status < 0)
			sleep(1);
		else if (status > 0)
		{
			fwrite(buffer, status, 1, stdout);
			fflush(stdout);
		}
	}

	status = ftdi_usb_close(ftdi);
	if (status < 0)
	{
		ERROR("failed to close device\n");
		ftdi_free(ftdi);

		return EXIT_FAILURE;
	}

	ftdi_free(ftdi);

	return EXIT_SUCCESS;
	/*
	int ret;
	struct ftdi_context *ftdi;
	struct ftdi_version_info version;

	ftdi = ftdi_new();
	if (ftdi == 0)
	{
		fprintf(stderr, "ftdi_new failed\n");
	}

	version = ftdi_get_library_version();

	ret = ftdi_usb_open(ftdi, 0x403, 0x6001);
	if (ret < 0)
	{
		fprintf(stderr, "unable to open %d %s\n", ret, ftdi_error_return(ret, );
		ftdi_free(ftdi);

		return EXIT_FAILURE;
	}

	if (ftdi->type == TYPE_R)
	{
		unsigned int chipid;
	
		ftdi_read_chipid(ftdi, &chipid);
		printf("FTDI chipid: %X\n", chipid);
	}

	ret = ftdi_usb_close(ftdi);
	if (ret < 0)
	{
		fprintf(stderr, "Failed to close\n");
		ftdi_free(ftdi);
		
		return EXIT_FAILURE;
	}

	ftdi_free(ftdi);

	return EXIT_SUCCESS;
	*/
}
