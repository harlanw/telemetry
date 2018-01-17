#ifndef VFS_H
#define VFS_

#include <stdint.h>

#define PATH_SEPARATOR '/'
#define PATH_SEPARATOR_STR "/"

struct vfs_node
{
	char *name;
	uint8_t flags;
	uint16_t size;
	uint32_t offs;
};

struct vfs_entry
{
	char *name;

}

typedef struct vfs_node VFS_NODE;

VFS_NODE *
vfs_node(char *name, uint8_t size);

VFS_NODE *
vfs_root(void);

#endif
