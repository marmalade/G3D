#ifndef G3D_HIERARCHY_UTIL_H
#define G3D_HIERARCHY_UTIL_H

//hash function used by event system.
#define G3D_HASH_INITIAL 5381
unsigned int G3DHashString(const char* string, unsigned int hash = G3D_HASH_INITIAL);

#endif