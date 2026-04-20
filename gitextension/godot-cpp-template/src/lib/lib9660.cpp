/* lib9660: a simple ISO9660 reader library especially suited to embedded
 * systems
 *
 * SPDX-License-Identifier: LicenseRef-ISC1
 * SPDX-FileCopyrightText: © 2014 Erin Shepherd
 */

#include "lib9660.h"
#include <cstring>

#ifdef DEBUG
#include <cstdlib>
#endif

#ifdef L9660_HAVE_STDIO
#include <cstdio>
#else
#ifndef SEEK_SET
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#endif
#endif

#define DENT_EXISTS (1 << 0)
#define DENT_ISDIR (1 << 1)
#define DENT_ASSOCIATED (1 << 2)
#define DENT_RECORD (1 << 3)
#define DENT_PROTECTION (1 << 4)
#define DENT_MULTIEXTENT (1 << 5)

// OPRAVA: Pro mapování struktur v C++ používáme reinterpret_cast
#define PVD(vdesc) (reinterpret_cast<l9660_vdesc_primary *>(vdesc))

#ifdef L9660_BIG_ENDIAN
#define READ16(v) (((v).be[1]) | ((v).be[0] << 8))
#define READ32(v) (((v).be[3]) | ((v).be[2] << 8) | ((v).be[1]) << 16 | ((v).be[0] << 24))
#else
#define READ16(v) (((v).le[0]) | ((v).le[1] << 8))
#define READ32(v) (((v).le[0]) | ((v).le[1] << 8) | ((v).le[2]) << 16 | ((v).le[3] << 24))
#endif

#ifndef L9660_SINGLEBUFFER
#define HAVEBUFFER(f) (true)
#define BUF(f) ((f)->buf)
#else
#define HAVEBUFFER(f) ((f) == last_file)
#define BUF(f) (gbuf)
static l9660_file *last_file;
static char gbuf[2048];
#endif

#ifdef _WIN32
static char *strchrnul(const char *s, int c) {
	while (*s) {
		if ((*s) == c)
			break;
		s++;
	}
	return const_cast<char *>(s);
}
#endif

static inline uint16_t fsectoff(l9660_file *f) {
	return f->position % 2048;
}

static inline uint32_t fsector(l9660_file *f) {
	return f->position / 2048;
}

static inline uint32_t fnextsectpos(l9660_file *f) {
	return (f->position + 2047) & ~2047;
}

l9660_status l9660_openfs(
		l9660_fs *fs,
		bool (*read_sector)(l9660_fs *fs, void *buf, uint32_t sector)) {
	fs->read_sector = read_sector;

#ifndef L9660_SINGLEBUFFER
	l9660_vdesc_primary *pvd = PVD(&fs->pvd);
#else
	last_file = nullptr;
	l9660_vdesc_primary *pvd = PVD(gbuf);
#endif
	uint32_t idx = 0x10;
	for (;;) {
		if (!read_sector(fs, pvd, idx))
			return L9660_EIO;

		if (std::memcmp(pvd->hdr.magic, "CD001", 5) != 0)
			return L9660_EBADFS;

		if (pvd->hdr.type == 1)
			break;
		else if (pvd->hdr.type == 255)
			return L9660_EBADFS;

		idx++;
	}

#ifdef L9660_SINGLEBUFFER
	std::memcpy(&fs->root_dir_ent, &pvd->root_dir_ent, pvd->root_dir_ent.length);
#endif

	return L9660_OK;
}

l9660_status l9660_fs_open_root(l9660_dir *dir, l9660_fs *fs) {
	l9660_file *f = &dir->file;
#ifndef L9660_SINGLEBUFFER
	l9660_dirent *dirent = &PVD(&fs->pvd)->root_dir_ent;
#else
	l9660_dirent *dirent = &fs->root_dir_ent;
#endif

	f->fs = fs;
	f->first_sector = READ32(dirent->sector);
	f->length = READ32(dirent->size);
	f->position = 0;

	return L9660_OK;
}

static l9660_status buffer(l9660_file *f) {
#ifdef L9660_SINGLEBUFFER
	last_file = f;
#endif
	if (!f->fs->read_sector(f->fs, BUF(f), f->first_sector + f->position / 2048))
		return L9660_EIO;
	else
		return L9660_OK;
}

static l9660_status prebuffer(l9660_file *f) {
	if (!HAVEBUFFER(f) || (f->position % 2048) == 0)
		return buffer(f);
	else
		return L9660_OK;
}

#if defined(DEBUG)
static void print_dirent(l9660_dirent *dent) {
	if (!std::getenv("L9660_DEBUG"))
		return;

	std::printf("| ---- dirent\n");
	std::printf("| length        %d\n", dent->length);
	std::printf("| xattr_length  %d\n", dent->xattr_length);
	std::printf("| sector        %u\n", READ32(dent->sector));
	std::printf("| size          %u\n", READ32(dent->size));
	std::printf("| name          \"%.*s\"\n", (int)dent->name_len, dent->name);
	std::printf("| ---- end dirent\n");
}
#endif

static l9660_status openat_raw(l9660_file *child, l9660_dir *parent, const char *name, bool isdir) {
	l9660_status rv;
	l9660_dirent *dent = nullptr;
	if ((rv = l9660_seekdir(parent, 0)))
		return rv;

	do {
		const char *seg = name;
#ifdef _WIN32
		name = strchrnul(name, '/');
#else
		const char *next = std::strchr(name, '/');
		name = next ? next : name + std::strlen(name);
#endif
		size_t seglen = (size_t)(name - seg);

		if (seglen == 1 && *seg == '.')
			seg = "\0";

		if (seglen == 2 && seg[0] == '.' && seg[1] == '.') {
			seg = "\1";
			seglen = 1;
		}

		for (;;) {
			if ((rv = l9660_readdir(parent, &dent)))
				return rv;

			if (!dent)
				return L9660_ENOENT;

#ifdef DEBUG
			print_dirent(dent);
#endif

			if (seglen > dent->name_len)
				continue;

			if (std::memcmp(seg, dent->name, seglen) != 0)
				continue;

			if (dent->name_len > seglen && dent->name[seglen] != ';')
				continue;

			break;
		}

		child->fs = parent->file.fs;
		child->first_sector = READ32(dent->sector) + dent->xattr_length;
		child->length = READ32(dent->size);
		child->position = 0;

		if (*name && (dent->flags & DENT_ISDIR) == 0)
			return L9660_ENOTDIR;

		parent = reinterpret_cast<l9660_dir *>(child);
		if (*name)
			name++;
	} while (*name);

	if (isdir) {
		if ((dent->flags & DENT_ISDIR) == 0)
			return L9660_ENOTDIR;
	} else {
		if ((dent->flags & DENT_ISDIR) != 0)
			return L9660_ENOTFILE;
	}

	return L9660_OK;
}

l9660_status l9660_opendirat(l9660_dir *dir, l9660_dir *parent, const char *path) {
	return openat_raw(&dir->file, parent, path, true);
}

static inline unsigned aligneven(unsigned v) {
	return v + (v & 1);
}

l9660_status l9660_readdir(l9660_dir *dir, l9660_dirent **pdirent) {
	l9660_status rv;
	l9660_file *f = &dir->file;

rebuffer:
	if (f->position >= f->length) {
		*pdirent = nullptr;
		return L9660_OK;
	}

	if ((rv = prebuffer(f)))
		return rv;

	char *off = BUF(f) + fsectoff(f);
	if (*off == 0) {
		f->position = fnextsectpos(f);
		goto rebuffer;
	}

	l9660_dirent *dirent = reinterpret_cast<l9660_dirent *>(off);
	f->position += aligneven(dirent->length);

	*pdirent = dirent;
	return L9660_OK;
}

l9660_status l9660_openat(l9660_file *child, l9660_dir *parent, const char *name) {
	return openat_raw(child, parent, name, false);
}

l9660_status l9660_seek(l9660_file *f, int whence, int32_t offset) {
	l9660_status rv;
	uint32_t cursect = fsector(f);

	switch (whence) {
		case SEEK_SET:
			f->position = (uint32_t)offset;
			break;

		case SEEK_CUR:
			f->position = (uint32_t)((int32_t)f->position + offset);
			break;

		case SEEK_END:
			f->position = (uint32_t)((int32_t)f->length - offset);
			break;
	}

	if (fsector(f) != cursect && fsectoff(f) != 0) {
		if ((rv = buffer(f)))
			return rv;
	}

	return L9660_OK;
}

uint32_t l9660_tell(l9660_file *f) {
	return f->position;
}

l9660_status l9660_read(l9660_file *f, void *buf, size_t size, size_t *read) {
	l9660_status rv;

	if ((rv = prebuffer(f)))
		return rv;

	uint16_t rem = (uint16_t)(2048 - fsectoff(f));
	if (rem > f->length - f->position)
		rem = static_cast<uint16_t>(f->length - f->position);

	if (rem < size)
		size = (size_t)rem;

	std::memcpy(buf, BUF(f) + fsectoff(f), size);

	*read = size;
	f->position += (uint32_t)size;

	return L9660_OK;
}
