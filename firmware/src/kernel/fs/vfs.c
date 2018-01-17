#include "vfs.h"

#include "kernel/kernel.h"
#include "utils/logging.h"

#include <stdlib.h>
#include <string.h>

#define MAX_NAMES KERNEL_MAX_FILES 
#define MAX_NAME_LENGTH 20

static VFS_NODE *root_node = NULL;

static char name_db[MAX_NAMES][MAX_NAME_LENGTH];
static uint8_t n_names = 0;
#if (MAX_NAMES > 255)
#error "File count not supported"
#endif

static char *add_name(char *name, uint8_t size)
{
	if ((n_names >= MAX_NAMES) || (size > MAX_NAME_LENGTH))
		return NULL;

	for (uint8_t i = 0; i < size; i++)
		if (!((name[i] <= '9' && name[i] >= '0') ||
		      (name[i] <= 'Z' && name[i] >= 'A') ||
		      (name[i] <= 'z' && name[i] >= 'a') ||
		      (name[i] == '-')))
		    return NULL;

	memcpy(&name_db[n_names], name, size);

	return name_db[n_names++];
}

VFS_NODE *
vfs_node(char *name, uint8_t size)
{
	VFS_NODE *node = calloc(1, sizeof(VFS_NODE));

	if (node)
		node->name = add_name(name, size);

	return node;
}

VFS_NODE *
vfs_root(void)
{
	if (!root_node)
		root_node = vfs_node(PATH_SEPARATOR_STR, 1);

	return root_node;
}
