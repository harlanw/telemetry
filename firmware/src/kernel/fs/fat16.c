#include "fat16.h"

#include "kernel/sys/system.h"

struct fat_partition
{
	uint8_t state;
#ifndef USE_SMALL_FAT_STRUCTS
	uint8_t chs_start[3];
#endif
	uint8_t type;
#ifndef USE_SMALL_FAT_STRUCTS
	uint8_t chs_end[3];
#endif
	uint32_t start_sector;
	uint32_t n_sectors;
} ATTR_PACKED;

struct fat16_boot_record
{
#ifndef USE_SMALL_FAT_STRUCTS
	uint8_t		jmp[3];
#endif
	char		desc[8];
	uint16_t	bytes_per_block;
	uint8_t		blocks_per_cluster;
	uint16_t	n_reserved_blocks;
	uint8_t		n_fats;
	uint16_t	n_root_dirs;
	uint16_t	n_blocks;
#ifndef USE_SMALL_FAT_STRUCTS
	uint8_t		media_desc;
#endif
	uint16_t	blocks_per_fat;
#ifndef USE_SMALL_FAT_STRUCTS
	uint16_t	blocks_per_track;
	uint16_t	n_heads;
#endif
	uint32_t	n_hidden_blocks;
	uint32_t	n_blocks_ext;
#ifndef USE_SMALL_FAT_STRUCTS
	uint16_t	logical_drive;
	uint8_t		signature_ext;
	uint32_t	volume_serial;
	char		volume_label[11];
	char		fs_type[8];
	char		boot[448];
	uint16_t	signature;
#endif
} ATTR_PACKED;

struct fat16_entry
{
	char filename[8];
	char ext[3];
	uint8_t attrib;
#ifndef USE_SMALL_FAT_STRUCTS
	uint8_t reserved[10];
#endif
	uint16_t ;
	uint16_t ;
	uint16_t start_cluster;
	uint32_t file_sz;
} ATTR_PACKED;

enum fat_partition_type
{
	FAT_PARTITION_NONE	= 0x00,
	FAT_PARTITION_FAT12	= 0x01,
	FAT_PARTITION_FAT16S	= 0x04,
	FAT_PARTITION_MSDOS	= 0x05,
	FAT_PARTITION_FAT16	= 0x06,
	FAT_PARTITION_FAT32	= 0x0B,
	FAT_PARTITION_FAT32_13H = 0x0C,
	FAT_PARTITION_FAT16_13H = 0x0E,
	FAT_PARTITION_MSDOS_13H = 0x0F
};

typedef struct fat16_entry entry_t;

typedef struct fat_partition partition_t;
typedef struct fat16_boot_record bootrec16_t;

typedef enum fat_partition_type type_t;

#define MBR_MAX_PARTITIONS 4
#define MBR_PART0_OFFS	0x01BE
#define MBR_PART1_OFFS	0x01CE
#define MBR_PART2_OFFS	0x01DE
#define MBR_PART3_OFFS	0x01EE

#define VALID_TYPE16(type) ((type == FAT_PARTITION_FAT16S) || \
			    (type == FAT_PARTITION_FAT16) ||  \
			    (type == FAT_PARTITION_FAT16_13H))
#define SECTOR_TO_BYTES(sector) (sector*512)

#ifndef __AVR_ARCH__

#include <stdio.h>

static FILE *img = NULL;

static void ATTR(nonnull)
read(void *dst, uint16_t bytes, uint16_t n, uint32_t offs)
{
	if (!img)
		img = fopen("test.img", "rb");

	fseek(img, offs, SEEK_SET);
	fread(dst, bytes, n, img);
}

static inline uint16_t ATTR(nonnull, always_inline)
read_partition(partition_t *pt, uint8_t which)
{
	const uint16_t PART_OFFS_LUT[MBR_MAX_PARTITIONS] =
	{
		MBR_PART0_OFFS, MBR_PART1_OFFS, MBR_PART2_OFFS, MBR_PART3_OFFS
	};

	uint16_t offs = PART_OFFS_LUT[which];

	read(&pt->type, 1, 1, offs + 4);
	read(&pt->start_sector, 1, 4, offs + 8);

	return offs;
}

static inline void ATTR(nonnull, always_inline)
read_boot_record(bootrec16_t *bt, uint32_t offs)
{
#ifdef USE_SMALL_FAT_STRUCTS
	/* JMP */
	offs += 3;
	/* DESC */
	read(bt->desc, 8, 1, offs);
	offs += 8;
	/* BYTES_PER_BLOCK -- N_BLOCKS */
	read(&bt->bytes_per_block, 10, 1, offs);
	offs += 10;
	/* MEDIA_DISC */
	offs += 1;
	/* BLOCKS_PER_FAT */
	read(&bt->blocks_per_fat, 1, 1, offs);
	offs += 2;
	/* BLOCKS_PER_TRACK -- N_HEADS */
	offs += 4;
	/* N_HIDDEN_BLOCKS -- N_BLOCKS_EXT */
	read(&bt->n_hidden_blocks, 4, 1, offs);
#else
	read(bt, sizeof(*bt), 1, offs);
#endif
}

uint16_t read_root(uint8_t partno)
{
	partition_t pt;
	bootrec16_t bt;

	uint32_t offs = 0;

	(void) read_partition(&pt, partno);
	if (VALID_TYPE16(pt.type))
	{
		offs = SECTOR_TO_BYTES(pt.start_sector);
		read_boot_record(&bt, offs);

		/* Offset from beginning of boot record */
		offs += (bt.n_reserved_blocks + (bt.n_fats * bt.blocks_per_fat)) * bt.bytes_per_block;
	}

	return offs;
}

void fat16_init(void)
{
	(void) read_root(0);
	(void) read_root(1);
}

#else

void fat16_init(void) {}

#endif
