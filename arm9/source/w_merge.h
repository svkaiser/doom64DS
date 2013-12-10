#ifndef W_MERGE_H
#define W_MERGE_H

#define W_NWT_MERGE_SPRITES   0x1
#define W_NWT_MERGE_FLATS     0x2

// Add a new WAD and merge it into the main directory

void W_MergeFile(char *filename);

// NWT-style merging

void W_NWTMergeFile(char *filename, int flags);

// Acts the same as NWT's "-merge" option.

void W_NWTDashMerge(char *filename);

// Debug function that prints the WAD directory.

void W_PrintDirectory(void);

#endif /* #ifndef W_MERGE_H */

