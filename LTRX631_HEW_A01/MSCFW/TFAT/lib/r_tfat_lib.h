/*""FILE COMMENT""*******************************************************************
* File Name		: tfat_lib.h
* Version      	: 1.00
* Device(s)    	: RX610
* Tool-Chain 	: HEW IDE, RX Standard Toolchain (V.1.0.0.0)
* H/W Platform 	: R5F56108
* Description	: TFAT File System library header file
*************************************************************************************
* History	: DD.MM.YYYY Version Description
* 			: 15.06.2010 1.00	 First Release
*""FILE COMMENT END""****************************************************************/
#ifndef _R_TFAT_LIB_H_
#define	_R_TFAT_LIB_H_

/***********************************************************************************
Macro definitions
***********************************************************************************/
/* File access control and file status flags (FIL.flag) */
#define	TFAT_FA_OPEN_EXISTING	0x00
#define	TFAT_FA_READ			0x01
#define TFAT_FA__ERROR			0x80

#define	TFAT_FA_WRITE			0x02
#define	TFAT_FA_CREATE_NEW		0x04
#define	TFAT_FA_CREATE_ALWAYS	0x08
#define	TFAT_FA_OPEN_ALWAYS		0x10
#define TFAT_FA__WRITTEN		0x20

/* File attribute bits for directory entry */
#define	TFAT_AM_RDO	0x01	/* Read only */
#define	TFAT_AM_HID	0x02	/* Hidden */
#define	TFAT_AM_SYS	0x04	/* System */
#define	TFAT_AM_VOL	0x08	/* Volume label */
#define TFAT_AM_LFN	0x0F	/* LFN entry */
#define TFAT_AM_DIR	0x10	/* Directory */
#define TFAT_AM_ARC	0x20	/* Archive */

/************************************************************************************
Type Definitions
************************************************************************************/
/* These types must be 16-bit, 32-bit or larger integer */

/* Boolean type */
typedef enum { TFAT_FALSE = 0, TFAT_TRUE } BOOL;

typedef uint32_t	CLUST;

/* File system object structure */
typedef struct _FATFS 
{
	uint16_t	id;				/* File system mount ID */
	uint16_t	n_rootdir;		/* Number of root directory entries */
	uint32_t	winsect;		/* Current sector appearing in the win[] */
	uint32_t	fatbase;		/* FAT start sector */
	uint32_t	dirbase;		/* Root directory start sector */
	uint32_t	database;		/* Data start sector */
	CLUST		sects_fat;		/* Sectors per fat */
	CLUST		max_clust;		/* Maximum cluster# + 1 */
	CLUST		last_clust;		/* Last allocated cluster */
	CLUST		free_clust;		/* Number of free clusters */
	uint32_t	fsi_sector;		/* fsinfo sector */
	uint8_t		fsi_flag;		/* fsinfo dirty flag (1:must be written back) */
	uint8_t		pad1;
	uint8_t		fs_type;		/* FAT sub type */
	uint8_t		csize;			/* Number of sectors per cluster */
	uint8_t		n_fats;			/* Number of FAT copies */
	uint8_t		winflag;		/* win[] dirty flag (1:must be written back) */
	uint8_t		win[512];		/* Disk access window for Directory/FAT/File */
} FATFS;

/* Directory object structure */
typedef struct _DIR 
{
	uint16_t	id;			/* Owner file system mount ID */
	uint16_t	index;		/* Current index */
	FATFS*	fs;				/* Pointer to the owner file system object */
	CLUST	sclust;			/* Start cluster */
	CLUST	clust;			/* Current cluster */
	uint32_t	sect;		/* Current sector */
} DIR;

/* File object structure */
typedef struct _FIL 
{
	uint16_t	id;				/* Owner file system mount ID */
	uint8_t	flag;				/* File status flags */
	uint8_t	csect;				/* Sector address in the cluster */
	FATFS*	fs;					/* Pointer to owner file system */
	uint32_t	fptr;			/* File R/W pointer */
	uint32_t	fsize;			/* File size */
	CLUST	org_clust;			/* File start cluster */
	CLUST	curr_clust;			/* Current cluster */
	uint32_t	curr_sect;		/* Current sector */
	uint32_t	dir_sect;		/* Sector containing the directory entry */
	uint8_t*	dir_ptr;		/* Ponter to the directory entry in the window */
} FIL;

/* File status structure */
typedef struct _FILINFO 
{
	uint32_t fsize;				/* Size */
	uint16_t fdate;				/* Date */
	uint16_t ftime;				/* Time */
	uint8_t fattrib;			/* Attribute */
	uint8_t fname[8+1+3+1];		/* Name (8.3 format) */
} FILINFO;

/* File function return code (FRESULT) */
typedef enum 
{
	TFAT_FR_OK = 0,				/* 0 */
	TFAT_FR_NOT_READY,			/* 1 */
	TFAT_FR_NO_FILE,			/* 2 */
	TFAT_FR_NO_PATH,			/* 3 */
	TFAT_FR_INVALID_NAME,		/* 4 */
	TFAT_FR_INVALID_DRIVE,		/* 5 */
	TFAT_FR_DENIED,				/* 6 */
	TFAT_FR_EXIST,				/* 7 */
	TFAT_FR_RW_ERROR,			/* 8 */
	TFAT_FR_WRITE_PROTECTED,	/* 9 */
	TFAT_FR_NOT_ENABLED,		/* 10 */
	TFAT_FR_NO_FILESYSTEM,		/* 11 */
	TFAT_FR_INVALID_OBJECT,		/* 12 */
	TFAT_FR_MKFS_ABORTED		/* 13 (not used) */
} FRESULT;

/***********************************************************************************
Function Prototypes
***********************************************************************************/
/* Mount/Unmount a logical drive */
extern FRESULT R_tfat_f_mount (uint8_t, FATFS*);						
/* Open or create a file */
extern FRESULT R_tfat_f_open (FIL*, const uint8_t*, uint8_t);			
/* Read data from a file */
extern FRESULT R_tfat_f_read (FIL*, void*, uint16_t, uint16_t*);			
/* Write data to a file */
extern FRESULT R_tfat_f_write (FIL*, const void*, uint16_t, uint16_t*);	
/* Move file pointer of a file object */
extern FRESULT R_tfat_f_lseek (FIL*, uint32_t);						
/* Close an open file object */
extern FRESULT R_tfat_f_close (FIL*);								
/* Open an existing directory */
extern FRESULT R_tfat_f_opendir (DIR*, const uint8_t*);				
/* Read a directory item */
extern FRESULT R_tfat_f_readdir (DIR*, FILINFO*);					
/* Get file status */
extern FRESULT R_tfat_f_stat (const uint8_t*, FILINFO*);				
/* Get number of free clusters on the drive */
extern FRESULT R_tfat_f_getfree (const uint8_t*, uint32_t*, FATFS**);	
/* Truncate file */
extern FRESULT R_tfat_f_truncate (FIL*);							
/* Flush cached data of a writing file */
extern FRESULT R_tfat_f_sync (FIL*);								
/* Delete an existing file or directory */
extern FRESULT R_tfat_f_unlink (const uint8_t*);						
/* Create a new directory */
extern FRESULT R_tfat_f_mkdir (const uint8_t*);						
/* Change file/dir attriburte */
extern FRESULT R_tfat_f_chmod (const uint8_t*, uint8_t, uint8_t);			
/* Change file/dir timestamp */
extern FRESULT R_tfat_f_utime (const uint8_t*, const FILINFO*);		
/* Rename/Move a file or directory */
extern FRESULT R_tfat_f_rename (const uint8_t*, const uint8_t*);		
/* Forward data to the stream */
extern FRESULT R_tfat_f_forward (FIL*, uint16_t(*)(const uint8_t*,uint16_t), uint16_t, uint16_t*);	

#endif	/*	_R_TFAT_LIB_H_ 	*/