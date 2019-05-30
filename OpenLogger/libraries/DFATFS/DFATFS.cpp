/************************************************************************/
/*                                                                      */
/*    DFATFS.cpp                                                        */
/*                                                                      */
/*                                                                      */
/************************************************************************/
/*    Author:     Keith Vogel                                           */
/*    Copyright 2015, Digilent Inc.                                     */
/************************************************************************/
/* 
*
* Copyright (c) 2015, Digilent <www.digilentinc.com>
* Contact Digilent for the latest version.
*
* This program is free software; distributed under the terms of 
* BSD 3-clause license ("Revised BSD License", "New BSD License", or "Modified BSD License")
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* 1.    Redistributions of source code must retain the above copyright notice, this
*        list of conditions and the following disclaimer.
* 2.    Redistributions in binary form must reproduce the above copyright notice,
*        this list of conditions and the following disclaimer in the documentation
*        and/or other materials provided with the distribution.
* 3.    Neither the name(s) of the above-listed copyright holder(s) nor the names
*        of its contributors may be used to endorse or promote products derived
*        from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*    10/16/2015(KeithV): Created                                       */
/************************************************************************/
//#include "DFATFS.h"
//#include "PinAssignments.h"
#include <OpenLogger.h>
#include "Threads.h"

#define iMKFS (_VOLUMES - 1)    // when we mount, we will do it on the last volume we have 

extern "C" int get_ldnumber (		/* Returns logical drive number (-1:invalid drive) */
	const char** path	/* Pointer to pointer to the path name */
);

typedef enum
{
    FSNONE,
    FSOPEN,
    FSCLOSE,
    FSREAD,
    FSWRITE,
    FSSEEK,
    FSTRUNCATE,
    FSSYNC,
    FSPUTC,
    FSPUTS,
    FSPRINTF,
    FSGETS,
    FSOPENDIR,
    FSCLOSEDIR,
    FSREADDIR,
    FSSTAT,
    FSUTIME,
    FSMOUNT,
    FSMKDIR,
    FSMKFS,
    FSUNLINK,
    FSRENAME,
    FSCHMOD,
    FSCHDIR,
    FSCHDRIVE,
    FSGETCWD,
    FSGETFREE,
    FSGETLABEL,
    FSSETLABEL
} FSFUNC;

DIR         DDIRINFO::_dir;
FILINFO     DDIRINFO::_fileInfo;
DFSVOL *    DFATFS::_arDFSVOL[_VOLUMES]= { NULL };
char const * const DFATFS::szFatFsVols[_VOLUMES] = {"0:", "1:", "2:", "3:", "4:"};
char const  DFATFS::szRoot[] = "/";

#define NBRFATPARAMS    10
static uint32_t rgParam[NBRFATPARAMS];
static FSFUNC fsFunc = FSNONE;
static int32_t curVol = -1;
static bool fYielding =  false;

//**********************************************************************
//
//          Thread1 code, called by Yield in thread.c
//
//**********************************************************************
void Thread1(void)
{
    uint32_t fr = FR_INVALID_PARAMETER;

    switch(fsFunc)
    {
        /************************************************************************/
        /****************************** DFILE ***********************************/
        /************************************************************************/
        case FSOPEN:
            fr = f_open((FIL*) rgParam[0], (const TCHAR*) rgParam[1], (BYTE) rgParam[2]);
            break;

        case FSCLOSE:
            fr = f_close((FIL*) rgParam[0]);
            break;

        case FSREAD:
            fr = f_read((FIL*) rgParam[0], (void*) rgParam[1], (UINT) rgParam[2], (UINT *) rgParam[3]);
            break;

        case FSWRITE:
            fr = f_write((FIL*) rgParam[0], (const void*) rgParam[1], (UINT) rgParam[2], (UINT*) rgParam[3]);
            break;

        case FSSEEK:
            fr = f_lseek((FIL*) rgParam[0], (DWORD) rgParam[1]);
            break;

        case FSTRUNCATE:
            fr = f_truncate ((FIL*) rgParam[0]);
            break;

        case FSSYNC:
            fr = f_sync ((FIL*) rgParam[0]);
            break;

        case FSPUTC:
            fr = f_putc ((TCHAR) rgParam[1], (FIL*) rgParam[0]);
            break;

        case FSPUTS:
            fr = f_puts ((const TCHAR*) rgParam[1], (FIL*) rgParam[0]);
            break;

        case FSPRINTF:
            fr = f_printf ((FIL*) rgParam[0], (const TCHAR*) rgParam[1], (va_list) rgParam[2]);
            break;

        case FSGETS:
            fr = (uint32_t) f_gets ((TCHAR*) rgParam[1], (int) rgParam[2], (FIL*) rgParam[0]);
            break;

        /************************************************************************/
        /**************************** DDIRINFO **********************************/
        /************************************************************************/
        case FSOPENDIR:
            fr = f_opendir ((DIR*) rgParam[0], (const TCHAR*) rgParam[1]);
            break;

        case FSCLOSEDIR:
            fr = f_closedir ((DIR*) rgParam[0]);
            break;

        case FSREADDIR:
            fr = f_readdir ((DIR*) rgParam[0], (FILINFO*) rgParam[1]);
            break;

        case FSSTAT:
            fr =  f_stat ((const TCHAR*) rgParam[1] , (FILINFO *) rgParam[0]);
            break;

        case FSUTIME:
            fr = f_utime ((const TCHAR*) rgParam[0], (const FILINFO*) rgParam[1]);
            break;

        /************************************************************************/
        /****************************** DFATFS **********************************/
        /************************************************************************/
        case FSMOUNT:
            fr =  f_mount ((FATFS *) rgParam[0], (const TCHAR *) rgParam[1], (BYTE) rgParam[2]);
            break;

        case FSMKDIR:
            fr =  f_mkdir ((const TCHAR*) rgParam[0]);
            break;

        case FSMKFS:
            fr =  f_mkfs ((const TCHAR*) rgParam[0], (BYTE) rgParam[1], (UINT) rgParam[2]);
            break;

        case FSUNLINK:
            fr =  f_unlink ((const TCHAR*) rgParam[0]);
            break;

        case FSRENAME:
            fr =  f_rename ((const TCHAR*) rgParam[0], (const TCHAR*) rgParam[1]);
            break;

        case FSCHMOD:
            fr =  f_chmod ((const TCHAR*) rgParam[0], (BYTE) rgParam[1], (BYTE) rgParam[2]);
            break;

        case FSCHDIR:
            fr =  f_chdir ((const TCHAR*) rgParam[0]);
            break;

        case FSCHDRIVE:
            fr =  f_chdrive ((const TCHAR*) rgParam[0]);
            break;

        case FSGETCWD:
            fr =  f_getcwd ((TCHAR*) rgParam[0], (UINT) rgParam[1]);
            break;

        case FSGETFREE:
            fr =  f_getfree ((const TCHAR*) rgParam[0], (DWORD*) rgParam[1], (FATFS**) rgParam[2]);
            break;

        case FSGETLABEL:
            fr =  f_getlabel ((const TCHAR*) rgParam[0], (TCHAR*) rgParam[1], (DWORD*) rgParam[2]);
            break;

        case FSSETLABEL:
            fr =  f_setlabel ((const TCHAR*) rgParam[0]);
            break;

        default:
            ASSERT(NEVER_SHOULD_GET_HERE);
            break;
    }

    rgParam[0] = fr;
    return;
}

FRESULT ExFatFunc(int32_t iVol, FSFUNC fatFunc, uint32_t param0, uint32_t param1, uint32_t param2, uint32_t param3)			
{
    if(!(fsFunc == FSNONE || (iVol == curVol && fatFunc == fsFunc))) return(FR_WAITING_FOR_THREAD);

    // if we are not Yielding, call the function
    if(!fYielding) 
    {
        // set up the parameters,
        curVol      = iVol;
        fsFunc      = fatFunc;
        rgParam[0]  = param0;
        rgParam[1]  = param1;
        rgParam[2]  = param2;    
        rgParam[3]  = param3; 
    }
    
    // yield to the thread
    fYielding = Yield(YThread1);

    // if not not done, say we are waiting for the thread
    if(fYielding) return(FR_WAITING_FOR_THREAD);

    // we are done
    fsFunc = FSNONE;
    curVol = -1;

    return((FRESULT) rgParam[0]);
}

//**********************************************************************
//
//          DFILE Class Thunks to the underlying FATFS C code
//
//**********************************************************************
FRESULT DFILE::fsopen (const char * path, uint8_t mode)
{
    const char *    pathT   = path;
    int             iVol    = get_ldnumber(&pathT);
    FRESULT         fr;

    if(_fOpen) return(FR_NOT_READY);

    fr = ExFatFunc(iVol, FSOPEN, (uint32_t) &_file, (uint32_t) path, (uint32_t) mode, 0);

    if(fr == FR_OK)
    {
        _fOpen = true;
        _iVol = iVol;
    }

    return(fr);
}

FRESULT DFILE::fsclose(void)
{
    FRESULT         fr;

    if(!_fOpen) return(FR_OK);

    fr = ExFatFunc(_iVol, FSCLOSE, (uint32_t) &_file, 0, 0, 0);

    if(fr == FR_OK)
    {
        _fOpen = false;
        _iVol = -1;
    }

    return(fr);
}

FRESULT DFILE::fsread (void* buff, uint32_t btr, uint32_t * br, uint32_t cSectorMax)			
{
    uint32_t btrtp = (btr - *br) > (cSectorMax * _CB_SECTOR_) ? (cSectorMax * _CB_SECTOR_) : (btr - *br);
    FRESULT fr;

    if(!fYielding) rgParam[NBRFATPARAMS-1] = 0;

    fr = ExFatFunc(_iVol, FSREAD, (uint32_t) &_file, (uint32_t) &((uint8_t *) buff)[*br], (uint32_t) btrtp, (uint32_t) &rgParam[NBRFATPARAMS-1]);

    if(fr == FR_OK)
    {
        (*br) += rgParam[NBRFATPARAMS-1];

        // if not done reading everything requested
        if(*br < btr) return(FR_WAITING_FOR_THREAD);
    }

    return(fr);
}

FRESULT DFILE::fswrite (const void* buff, uint32_t btw, uint32_t* bw, uint32_t cSectorMax)
{
    uint32_t btwtp = (btw - *bw) > (cSectorMax * _CB_SECTOR_) ? (cSectorMax * _CB_SECTOR_) : (btw - *bw);
    FRESULT fr;

    ASSERT(btw >= *bw);

    if(!fYielding) rgParam[NBRFATPARAMS-1] = 0;

    fr = ExFatFunc(_iVol, FSWRITE, (uint32_t) &_file, (uint32_t) &((uint8_t *) buff)[*bw], (uint32_t) btwtp, (uint32_t) &rgParam[NBRFATPARAMS-1]);

    if(fr == FR_OK && rgParam[NBRFATPARAMS-1] > 0)
    {
        (*bw) += rgParam[NBRFATPARAMS-1];

        // if not done writing 
        if(*bw < btw) return(FR_WAITING_FOR_THREAD);
    }

    return(fr);
}

FRESULT DFILE::fslseek (uint32_t ofs)							
{
    return(ExFatFunc(_iVol, FSSEEK, (uint32_t) &_file, (uint32_t) ofs, 0, 0));
}

FRESULT DFILE::fstruncate (void)										
{
    return(ExFatFunc(_iVol, FSTRUNCATE, (uint32_t) &_file, 0, 0, 0));
}

FRESULT DFILE::fssync (void)										
{
    return(ExFatFunc(_iVol, FSSYNC, (uint32_t) &_file, 0, 0, 0));
}

int DFILE::fsputc (char c)									
{
    FRESULT fr;

    fr = ExFatFunc(_iVol, FSPUTC, (uint32_t) &_file, (uint32_t) c, 0, 0);

    if(fr == FR_WAITING_FOR_THREAD) return(WOT);

    return((int) fr);
}

int DFILE::fsputs (const char* str)								
{
    FRESULT fr;

    fr = ExFatFunc(_iVol, FSPUTS, (uint32_t) &_file, (uint32_t) str, 0, 0);

    if(fr == FR_WAITING_FOR_THREAD) return(WOT);

    return((int) fr);
}

int DFILE::fsprintf (const char * str, va_list va)						
{
    FRESULT fr;

    fr = ExFatFunc(_iVol, FSPRINTF, (uint32_t) &_file, (uint32_t) str, (uint32_t) va, 0);

    if(fr == FR_WAITING_FOR_THREAD) return(WOT);

    return((int) fr);
}

char* DFILE::fsgets (char * buff, int32_t len)					
{
    FRESULT fr;

    fr = ExFatFunc(_iVol, FSGETS, (uint32_t) &_file, (uint32_t) buff, (uint32_t) len, 0);

    if(fr == FR_WAITING_FOR_THREAD) return((char *) WOT);

    return((char *) fr);
}



//**********************************************************************
//
//          DDIRINFO Class Thunks
//
//**********************************************************************
FRESULT DDIRINFO::fsopendir (const char * path)
{
    const char *    pathT   = path;
    int             iVol    = get_ldnumber(&pathT);

#if _USE_LFN
    if(!fYielding) 
    {
	    _fileInfo.lfname = NULL;
	    _fileInfo.lfsize = 0;
    }
#endif

    return(ExFatFunc(iVol, FSOPENDIR, (uint32_t) &_dir, (uint32_t) path, 0, 0));
}

FRESULT DDIRINFO::fsclosedir (void)
{
    FRESULT fr = ExFatFunc(_dir.id, FSCLOSEDIR, (uint32_t) &_dir, 0, 0, 0);

#if _USE_LFN
    if(fr == FR_OK)
    {
	    _fileInfo.lfname = NULL;
	    _fileInfo.lfsize = 0;
    }
#endif

    return(fr);
}

FRESULT DDIRINFO::fsreaddir (void)
{
    return(ExFatFunc(_dir.id, FSREADDIR, (uint32_t) &_dir, (uint32_t) &_fileInfo, 0, 0));
}

FRESULT DDIRINFO::fsrewinddir (void)
{
    return(ExFatFunc(_dir.id, FSREADDIR, (uint32_t) &_dir, 0, 0, 0));
}

FRESULT DDIRINFO::fsstat (const char * path)
{
    const char *    pathT   = path;
    int             iVol    = get_ldnumber(&pathT);

    return(ExFatFunc(iVol, FSSTAT, (uint32_t) &_fileInfo, (uint32_t) path, 0, 0));
}

FRESULT DDIRINFO::fsutime (const char * path, uint16_t date, uint16_t time)
{
    const char *    pathT   = path;
    int             iVol    = get_ldnumber(&pathT);

    if(!fYielding) 
    {
        _fileInfo.fdate = date;
        _fileInfo.ftime = time;
    }

    return(ExFatFunc(iVol, FSUTIME, (uint32_t) path, (uint32_t) &_fileInfo, 0, 0));
}

//**********************************************************************
//
//          DFATFS Class Thunks
//
//**********************************************************************
FRESULT DFATFS::fsmount(DFSVOL& dfsvol, const char* path, uint8_t opt) // FATFS comes from VOL
{ 
    const char *    pathT   = path;
    int iVol = get_ldnumber(&pathT);
    FRESULT fr = FR_INVALID_DRIVE;

    if(!fYielding) 
    {
        DFATFS::_arDFSVOL[iVol] = &dfsvol;
    }

    fr = ExFatFunc(iVol, FSMOUNT, (uint32_t) &dfsvol._fatfs, (uint32_t) path, (uint32_t) opt, 0);

    // clean up if we have an error
    if(!fYielding && fr != FR_OK)
    {
        DFATFS::_arDFSVOL[iVol] = NULL;
    }

    return(fr);
}

// this mounts vol NULL; forces an unmount of the other vol
FRESULT DFATFS::fsunmount(const char* path) 
{
    const char *    pathT   = path;
    int             iVol    = get_ldnumber(&pathT);
    FRESULT         fr      = ExFatFunc(iVol, FSMOUNT, (uint32_t) NULL, (uint32_t) path, 1, 0);

    if(fr == FR_OK) 
    {
            DFATFS::_arDFSVOL[iVol] = NULL;
    }
    return(fr);
}

bool DFATFS::fsvolmounted(const char* path)
{
    const char *    pathT   = path;
    int iVol = get_ldnumber(&pathT);

    return(DFATFS::_arDFSVOL[iVol] != NULL);
}

FRESULT DFATFS::fsmkdir(const char* path)
{
    const char *    pathT   = path;
    int             iVol    = get_ldnumber(&pathT);

    return(ExFatFunc(iVol, FSMKDIR, (uint32_t) path, 0, 0, 0));
}

FRESULT DFATFS::fsmkfs(DFSVOL& dfsVol)
{
    static DFSVOL * pCurDfsVol = NULL;  
    static STATE    curState = Idle;
    static FRESULT  fr = FR_OK;

    if(!(pCurDfsVol == NULL || pCurDfsVol == &dfsVol)) return(FR_WAITING_FOR_THREAD);

    switch(curState)
    {
        case Idle:
            ASSERT(!fsvolmounted(szFatFsVols[VOLNONE]));
            pCurDfsVol = &dfsVol;
            curState = FILEmount;
 
            // fall thru

        case FILEmount:
            if((fr = fsmount(dfsVol, szFatFsVols[VOLNONE], 0)) == FR_OK) curState = Working;
            break;

        case Working:
            if((fr = ExFatFunc(VOLNONE, FSMKFS, (uint32_t) szFatFsVols[VOLNONE], (uint32_t) dfsVol._sfd, (uint32_t) dfsVol._au, 0)) == FR_OK) curState = FILEunmount;
            break;

        case FILEunmount:
            // don't touch fr; we have to return it later
            if(fsunmount(szFatFsVols[VOLNONE]) == FR_OK) curState = Idle;
            break;
   }

    // do not touch fr
    if(!(fr == FR_OK || fr == FR_WAITING_FOR_THREAD))
    {
        // if we need to unmount
        if(fsvolmounted(szFatFsVols[VOLNONE])) curState = FILEunmount;
 
        // already unmounted
        else curState = Idle;
    }

    // if we are done, good or bad
    // return our last fr state
    if(curState == Idle)
    {
        pCurDfsVol = NULL;
        ASSERT(!fsvolmounted(szFatFsVols[VOLNONE]));
        return(fr);
    }

    return(FR_WAITING_FOR_THREAD);
}

FRESULT DFATFS::fsunlink(const char* path)
{
    const char *    pathT   = path;
    int             iVol    = get_ldnumber(&pathT);

    return(ExFatFunc(iVol, FSUNLINK, (uint32_t) path, 0, 0, 0));
}

FRESULT DFATFS::fsrename(const char* path_old, const char* path_new)
{
    const char *    pathT   = path_old;
    int             iVol    = get_ldnumber(&pathT);

    return(ExFatFunc(iVol, FSRENAME, (uint32_t) path_old, (uint32_t) path_new, 0, 0));
}

FRESULT DFATFS::fschmod(const char* path, uint8_t value, uint8_t mask)
{
    const char *    pathT   = path;
    int             iVol    = get_ldnumber(&pathT);

    return(ExFatFunc(iVol, FSCHMOD, (uint32_t) path, (uint32_t) value, (uint32_t) mask, 0));
}

FRESULT DFATFS::fschdir(const char* path)
{
    const char *    pathT   = path;
    int             iVol    = get_ldnumber(&pathT);

    return(ExFatFunc(iVol, FSCHDIR, (uint32_t) path, 0, 0, 0));
}

FRESULT DFATFS::fschdrive(const char* path)
{
    const char *    pathT   = path;
    int iVol = get_ldnumber(&pathT);

    if(DFATFS::_arDFSVOL[iVol] == NULL) return(FR_NOT_READY);

    return(ExFatFunc(iVol, FSCHDRIVE, (uint32_t) path, 0, 0, 0));
}

FRESULT DFATFS::fsgetcwd(char* buff, uint32_t len)
{
    const char *    pathT   = NULL;         // so we get the working directory
    int iVol = get_ldnumber(&pathT);

    return(ExFatFunc(iVol, FSGETCWD, (uint32_t) buff, (uint32_t) len, 0, 0));
}


FRESULT DFATFS::fsgetfree (const char* path, uint32_t* pcFreeClusters, uint32_t * pcSectorsPerCluster)
{
    const char *    pathT   = path;
    int             iVol    = get_ldnumber(&pathT);
    FRESULT         fr;

    if(!fYielding) rgParam[NBRFATPARAMS-1] = 0; 

    fr = ExFatFunc(iVol, FSGETFREE, (uint32_t) path, (uint32_t) pcFreeClusters, (uint32_t) &rgParam[NBRFATPARAMS-1], 0);

    if(fr == FR_OK) *pcSectorsPerCluster = ((FATFS *) rgParam[NBRFATPARAMS-1])->csize;

    return(fr);  
}

FRESULT DFATFS::fsgetlabel(const char* path, char * label, uint32_t* vsn) 
{
    const char *    pathT   = path;
    int             iVol    = get_ldnumber(&pathT);

    return(ExFatFunc(iVol, FSGETLABEL, (uint32_t) path, (uint32_t) label, (uint32_t) vsn, 0));
}

FRESULT DFATFS::fssetlabel(const char* label)
{
    const char *    pathT   = label;
    int             iVol    = get_ldnumber(&pathT);

    return(ExFatFunc(iVol, FSSETLABEL, (uint32_t) label, 0, 0, 0));
}


#ifdef DEAD
#if (_MULTI_PARTITION == 1)
FRESULT DFATFS::fsfdisk(uint8_t pdrv, const uint32_t szt[], void* work)
{
    return(f_fdisk (pdrv, szt, work));
}
#endif
#endif



// The following is original thread code for documentation

#if 0
FRESULT DFILE::fsopen (const char * path, uint8_t mode)
{
    const char *    pathT   = path;
    int             iVol    = get_ldnumber(&pathT);
    FRESULT         fr;

    if(iVol != VOLSD)
    {
        fr = f_open (&_file, path, mode);
        _fOpen = (fr == FR_OK);
        _fThread = false;
        return(fr);
    }
    
    // someone else has it
    if(!(fsFunc == FSNONE || fsFunc == FSOPEN)) return(FR_WAITING_FOR_THREAD);

    // if we are not Yielding, call the function
    if(!fYielding) 
    {
        // set up the parameters, 
        fsFunc      = FSOPEN;
        rgParam[0]  = (uint32_t) &_file;
        rgParam[1]  = (uint32_t) path;
        rgParam[2]  = (uint32_t) mode;        
    }
    
    // yield to the thread
    fYielding = Yield(YThread1);

    // if done, then finish
    if(!fYielding)
    {
        fr = (FRESULT) rgParam[0];
        fsFunc = FSNONE;
        _fOpen = (fr == FR_OK);
        _fThread = _fOpen;
        return(fr);
    }
 
    // Otherwise, still working on it
    return(FR_WAITING_FOR_THREAD);
}

FRESULT DFILE::fsclose (void)											
{
    FRESULT fr;

    if(!_fOpen) return(FR_OK);
    
    if(!_fThread)
    {
        _fOpen = false;
        return(f_close(&_file));
    }
    
    // someone else has the thread
    if(!(fsFunc == FSNONE || fsFunc == FSCLOSE)) return(FR_WAITING_FOR_THREAD);

    // if we are not Yielding, call the function
    if(!fYielding) 
    {
        // set up the parameters, 
        fsFunc      = FSCLOSE;
        rgParam[0]  = (uint32_t) &_file;
    }
    
    // yield to the thread
    fYielding = Yield(YThread1);

    // if done, then finish
    if(!fYielding)
    {
        fr = (FRESULT) rgParam[0];
         _fOpen = false;
         _fThread = false;
        fsFunc = FSNONE;
        return(fr);
    }
 
    return(FR_WAITING_FOR_THREAD);
}

FRESULT DFILE::fsread (void* buff, uint32_t btr, uint32_t * br, uint32_t cSectorMax)			
{
    uint32_t brtp = 0;
    uint32_t btrtp = (btr - *br) > (cSectorMax * _CB_SECTOR_) ? (cSectorMax * _CB_SECTOR_) : (btr - *br);
    FRESULT fr;

    if(!_fThread)
    {
       fr = f_read (&_file, &((uint8_t *) buff)[*br], btrtp, &brtp);    
    }
    
    else
    {
        // someone else has it
        if(!(fsFunc == FSNONE || fsFunc == FSREAD)) return(FR_WAITING_FOR_THREAD);

        // if we are not Yielding, call the function
        if(!fYielding) 
        {
            // set up the parameters, 
            fsFunc      = FSREAD;
            rgParam[0]  = (uint32_t) &_file;
            rgParam[1]  = (uint32_t) &((uint8_t *) buff)[*br];
            rgParam[2]  = (uint32_t) btrtp;        
            rgParam[3]  = (uint32_t) &rgParam[4];   
            rgParam[4]  = 0;
        }
    
        // yield to the thread
        fYielding = Yield(YThread1);

        // if done, then finish
        if(fYielding)
        {
            return(FR_WAITING_FOR_THREAD);
        }
        else
        {
            fr = (FRESULT) rgParam[0];
            brtp = rgParam[4];
            fsFunc = FSNONE;
        }
    }

    (*br) += brtp;

    if(fr == FR_OK && *br < btr)
    {
        return(FR_WAITING_FOR_THREAD);
    }

    return(fr);
}

FRESULT DFILE::fswrite (const void* buff, uint32_t btw, uint32_t* bw, uint32_t cSectorMax)
{
    uint32_t bwtp = 0;
    uint32_t btwtp = (btw - *bw) > (cSectorMax * _CB_SECTOR_) ? (cSectorMax * _CB_SECTOR_) : (btw - *bw);
    FRESULT fr;

    ASSERT(btw >= *bw);

    if(!_fThread)
    {
        fr = f_write (&_file, &((uint8_t *) buff)[*bw], btwtp, &bwtp);    
    }
    
    else
    {
        // someone else has it
        if(!(fsFunc == FSNONE || fsFunc == FSWRITE)) return(FR_WAITING_FOR_THREAD);

        // if we are not Yielding, call the function
        if(!fYielding) 
        {
            // set up the parameters, 
            fsFunc      = FSWRITE;
            rgParam[0]  = (uint32_t) &_file;
            rgParam[1]  = (uint32_t) &((uint8_t *) buff)[*bw];
            rgParam[2]  = (uint32_t) btwtp;        
            rgParam[3]  = (uint32_t) &rgParam[4];  
            rgParam[4] = 0;
        }
    
        // yield to the thread
        fYielding = Yield(YThread1);

        // if still yielding, let the caller know I am not done
        if(fYielding)
        {
            return(FR_WAITING_FOR_THREAD);
        }
        // otherwise we are done, return the result
        else
        {
            fr = (FRESULT) rgParam[0];
            bwtp = rgParam[4];
            fsFunc = FSNONE;
        }
    }
  
    (*bw) += rgParam[4];

    if(fr == FR_OK && *bw < btw)
    {
        return(FR_WAITING_FOR_THREAD);
    }

    return(fr);
}

FRESULT DFILE::fslseek (uint32_t ofs)							
{
    FRESULT fr;

    if(!_fThread)
    {
        return(f_lseek (&_file, ofs));    
    }
    
    // someone else has the thread
    if(!(fsFunc == FSNONE || fsFunc == FSSEEK)) return(FR_WAITING_FOR_THREAD);

    // if we are not Yielding, call the function
    if(!fYielding) 
    {
        // set up the parameters, 
        fsFunc      = FSSEEK;
        rgParam[0]  = (uint32_t) &_file;
        rgParam[1]  = (uint32_t) ofs;
    }
    
    // yield to the thread
    fYielding = Yield(YThread1);

    // if done, then finish
    if(!fYielding)
    {
        fr = (FRESULT) rgParam[0];
        fsFunc = FSNONE;
        return(fr);
    }
 
    return(FR_WAITING_FOR_THREAD);
}

FRESULT DFILE::fstruncate (void)										
{
    FRESULT fr;

    if(!_fThread)
    {
        return(f_truncate (&_file));   
    }
    
    // someone else has the thread
    if(!(fsFunc == FSNONE || fsFunc == FSTRUNCATE)) return(FR_WAITING_FOR_THREAD);

    // if we are not Yielding, call the function
    if(!fYielding) 
    {
        // set up the parameters, 
        fsFunc      = FSTRUNCATE;
        rgParam[0]  = (uint32_t) &_file;
    }
    
    // yield to the thread
    fYielding = Yield(YThread1);

    // if done, then finish
    if(!fYielding)
    {
        fr = (FRESULT) rgParam[0];
        fsFunc = FSNONE;
        return(fr);
    }
 
    return(FR_WAITING_FOR_THREAD);
}

FRESULT DFILE::fssync (void)										
{
    FRESULT fr;

    if(!_fThread)
    {
        return(f_sync (&_file));    
    }
    
    // someone else has the thread
    if(!(fsFunc == FSNONE || fsFunc == FSSYNC)) return(FR_WAITING_FOR_THREAD);

    // if we are not Yielding, call the function
    if(!fYielding) 
    {
        // set up the parameters, 
        fsFunc      = FSSYNC;
        rgParam[0]  = (uint32_t) &_file;
    }
    
    // yield to the thread
    fYielding = Yield(YThread1);

    // if done, then finish
    if(!fYielding)
    {
        fr = (FRESULT) rgParam[0];
        fsFunc = FSNONE;
        return(fr);
    }
 
    return(FR_WAITING_FOR_THREAD);
}

int DFILE::fsputc (char c)									
{
    int32_t fr;

    if(!_fThread)
    {
        return(f_putc (c, &_file));     
    }
    
    // someone else has the thread
    if(!(fsFunc == FSNONE || fsFunc == FSPUTC)) return(-1);

    // if we are not Yielding, call the function
    if(!fYielding) 
    {
        // set up the parameters, 
        fsFunc      = FSPUTC;
        rgParam[0]  = (uint32_t) &_file;
        rgParam[1]  = (uint32_t) c; 
    }
    
    // yield to the thread
    fYielding = Yield(YThread1);

    // if done, then finish
    if(!fYielding)
    {
        fr = (int32_t) rgParam[0];
        fsFunc = FSNONE;
        return(fr);
    }
 
    return(-1);
}

int DFILE::fsputs (const char* str)								
{
    int32_t fr;

    if(!_fThread)
    {
        return(f_puts (str, &_file));  
    }
    
    // someone else has the thread
    if(!(fsFunc == FSNONE || fsFunc == FSPUTS)) return(-1);

    // if we are not Yielding, call the function
    if(!fYielding) 
    {
        // set up the parameters, 
        fsFunc      = FSPUTS;
        rgParam[0]  = (uint32_t) &_file;
        rgParam[1]  = (uint32_t) str; 
    }
    
    // yield to the thread
    fYielding = Yield(YThread1);

    // if done, then finish
    if(!fYielding)
    {
        fr = (int32_t) rgParam[0];
        fsFunc = FSNONE;
        return(fr);
    }
 
    return(-1);
}

int DFILE::fsprintf (const char * str, va_list va)						
{
    int32_t fr;

    if(!_fThread)
    {
        return(f_printf (&_file, str, va));     
    }
    
    // someone else has the thread
    if(!(fsFunc == FSNONE || fsFunc == FSPRINTF)) return(-1);

    // if we are not Yielding, call the function
    if(!fYielding) 
    {
        // set up the parameters, 
        fsFunc      = FSPRINTF;
        rgParam[0]  = (uint32_t) &_file;
        rgParam[1]  = (uint32_t) str; 
        rgParam[1]  = (uint32_t) va; 
    }
    
    // yield to the thread
    fYielding = Yield(YThread1);

    // if done, then finish
    if(!fYielding)
    {
        fr = (int32_t) rgParam[0];
        fsFunc = FSNONE;
        return(fr);
    }
 
    return(-1);
}

char* DFILE::fsgets (char * buff, int32_t len)					
{
    char * fr;

    if(!_fThread)
    {
        return((char *) f_gets (buff, len, &_file));   
    }
    
    // someone else has the thread
    if(!(fsFunc == FSNONE || fsFunc == FSGETS)) return((char *)-1);

    // if we are not Yielding, call the function
    if(!fYielding) 
    {
        // set up the parameters, 
        fsFunc      = FSGETS;
        rgParam[0]  = (uint32_t) &_file;
        rgParam[1]  = (uint32_t) buff; 
        rgParam[1]  = (uint32_t) len; 
    }
    
    // yield to the thread
    fYielding = Yield(YThread1);

    // if done, then finish
    if(!fYielding)
    {
        fr = (char *) rgParam[0];
        fsFunc = FSNONE;
        return(fr);
    }
 
    return((char *)-1);
}

FRESULT DDIRINFO::fsreaddir (void)
{
    if(_dir.id == VOLSD && fYielding) return(FR_WAITING_FOR_THREAD);
    return(f_readdir (&_dir, &_fileInfo));
}

FRESULT DDIRINFO::fsopendir (const char * path)
{
    const char *    pathT   = path;
    int             iVol    = get_ldnumber(&pathT);

    if(iVol == VOLSD && fYielding) return(FR_WAITING_FOR_THREAD);
#if _USE_LFN
	_fileInfo.lfname = NULL;
	_fileInfo.lfsize = 0;
#endif
    return(f_opendir (&_dir, path));
}

FRESULT DDIRINFO::fsclosedir (void)
{
    if(_dir.id == VOLSD && fYielding) return(FR_WAITING_FOR_THREAD);
#if _USE_LFN
	_fileInfo.lfname = NULL;
	_fileInfo.lfsize = 0;
#endif
    return(f_closedir (&_dir));
}

FRESULT DDIRINFO::fsstat (const char * path)
{
    const char *    pathT   = path;
    int             iVol    = get_ldnumber(&pathT);
    FRESULT         fr;

    if(iVol != VOLSD)                                   return(f_stat(path, &_fileInfo));
    else if(!(fsFunc == FSNONE || fsFunc == FSSTAT))    return(FR_WAITING_FOR_THREAD);
 
    // if we are not Yielding, call the function
    if(!fYielding) 
    {
        // set up the parameters, 
        fsFunc      = FSSTAT;
        rgParam[0]  = (uint32_t) &_fileInfo;
        rgParam[1]  = (uint32_t) path; 
    }
    
    // yield to the thread
    fYielding = Yield(YThread1);

    // if done, then finish
    if(!fYielding)
    {
        fr = (FRESULT) rgParam[0];
        fsFunc = FSNONE;
        return(fr);
    }
 
    return(FR_WAITING_FOR_THREAD);
}

FRESULT DDIRINFO::fsutime (const char * path, uint16_t date, uint16_t time)
{
    const char *    pathT   = path;
    int             iVol    = get_ldnumber(&pathT);

    if(iVol == VOLSD && fYielding) return(FR_WAITING_FOR_THREAD);
    _fileInfo.fdate = date;
    _fileInfo.ftime = time;
    return(f_utime (path, &_fileInfo));
}

FRESULT DFATFS::fsmount(DFSVOL& dfsvol, const char* path, uint8_t opt) // FATFS comes from VOL
{ 
    const char *    pathT   = path;
    FRESULT fr = FR_INVALID_DRIVE;
    int iVol = get_ldnumber(&pathT);

    if(0 <= iVol && iVol < _VOLUMES)
    {
        if(iVol != VOLSD)
        {
            DFATFS::_arDFSVOL[iVol] = &dfsvol;
            fr = f_mount(&dfsvol._fatfs, path, opt);
        }

        // someone else has it
        else if(!(fsFunc == FSNONE || fsFunc == FSMOUNT)) 
        {
            return(FR_WAITING_FOR_THREAD);
        }

        else
        {
            // if we are not Yielding, call the function
            if(!fYielding) 
            {
                DFATFS::_arDFSVOL[iVol] = &dfsvol;

                // set up the parameters, 
                fsFunc      = FSMOUNT;
                rgParam[0]  = (uint32_t) &dfsvol._fatfs;
                rgParam[1]  = (uint32_t) path;
                rgParam[2]  = (uint32_t) opt;        
            }
    
            // yield to the thread
            fYielding = Yield(YThread1);

            // Still working
            if(fYielding)
            {
                return(FR_WAITING_FOR_THREAD);
            }
            else
            {
                fr = (FRESULT) rgParam[0];
                fsFunc = FSNONE;
            }
        }

        // clean up if we have an error
        if(fr != FR_OK)
        {
            DFATFS::_arDFSVOL[iVol] = NULL;
        }
    }

    return(fr);
}

FRESULT DFATFS::fsunmount(const char* path) 
{
    const char *    pathT   = path;
    int             iVol    = get_ldnumber(&pathT);
    FRESULT         fr;

    if(iVol == VOLSD && fYielding) return(FR_WAITING_FOR_THREAD);
    fr      = f_mount(NULL, path, 1);
    if(0 <= iVol && iVol < _VOLUMES && fr == FR_OK)
    {
            DFATFS::_arDFSVOL[iVol] = NULL;
    }
    return(fr);
}

FRESULT DFATFS::fsmkfs(DFSVOL& dfsVol)
{
    FRESULT fr = FR_OK;
    DFSVOL * pDFSVolSave = _arDFSVOL[iMKFS];

    if(fYielding) return(FR_WAITING_FOR_THREAD);

    // unmount the drive, in case it is mounted
    fsunmount(szFatFsVols[iMKFS]);

    // delay mount the volume   
    if((fr = fsmount(dfsVol, szFatFsVols[iMKFS], 0)) != FR_OK)
    {
        return(fr);
    }

    // create the file system
    fr = f_mkfs(szFatFsVols[iMKFS], dfsVol._sfd, dfsVol._au);

    // unmount the drive
    fsunmount(szFatFsVols[iMKFS]);

    // restore the old mount
    if(pDFSVolSave != NULL)
    {
        fsmount(*pDFSVolSave, szFatFsVols[iMKFS], 0);
    }

    return(fr);
}

FRESULT DFATFS::fsmkdir(const char* path)
{
    const char *    pathT   = path;
    int             iVol    = get_ldnumber(&pathT);

    if(iVol == VOLSD && fYielding) return(FR_WAITING_FOR_THREAD);
    return(f_mkdir(path));
}

FRESULT DFATFS::fsunlink(const char* path)
{
    const char *    pathT   = path;
    int             iVol    = get_ldnumber(&pathT);

    if(iVol == VOLSD && fYielding) return(FR_WAITING_FOR_THREAD);
    return(f_unlink(path));
}

FRESULT DFATFS::fsrename(const char* path_old, const char* path_new)
{
    const char *    pathT   = path_old;
    int             iVol    = get_ldnumber(&pathT);

    if(iVol == VOLSD && fYielding) return(FR_WAITING_FOR_THREAD);
    return(f_rename(path_old, path_new));
}

FRESULT DFATFS::fschmod(const char* path, uint8_t value, uint8_t mask)
{
    const char *    pathT   = path;
    int             iVol    = get_ldnumber(&pathT);

    if(iVol == VOLSD && fYielding) return(FR_WAITING_FOR_THREAD);
    return(f_chmod(path, value, mask));
}

FRESULT DFATFS::fschdir(const char* path)
{
    const char *    pathT   = path;
    int             iVol    = get_ldnumber(&pathT);

    if(iVol == VOLSD && fYielding) return(FR_WAITING_FOR_THREAD);
    return(f_chdir(path));
}

FRESULT DFATFS::fschdrive(const char* path)
{
    const char *    pathT   = path;
    int iVol = get_ldnumber(&pathT);

    if(DFATFS::_arDFSVOL[iVol] == NULL) return(FR_NOT_READY);
    else if(iVol == VOLSD && fYielding) return(FR_WAITING_FOR_THREAD);
    else return(f_chdrive(path));
}

FRESULT DFATFS::fsgetcwd(char* buff, uint32_t len)
{
    const char *    pathT   = NULL;         // so we get the working directory
    int iVol = get_ldnumber(&pathT);

    if(iVol == VOLSD && fYielding) return(FR_WAITING_FOR_THREAD);
    return(f_getcwd (buff, len));
}

FRESULT DFATFS::fsgetfree (const char* path, uint32_t* pcFreeClusters, uint32_t * pcSectorsPerCluster, uint32_t * pcBytesPerSector)
{
    FATFS * pfatfs;
    const char *    pathT   = path;
    int             iVol    = get_ldnumber(&pathT);
    FRESULT         fr;

    if(iVol == VOLSD && fYielding) return(FR_WAITING_FOR_THREAD);

    if((fr = f_getfree(path, pcFreeClusters, &pfatfs)) == FR_OK) 
    {
        *pcSectorsPerCluster    = fatfs->csize;
        *pcBytesPerSector       = pfatfs->ssize;
    }
    return(fr);  
}

FRESULT DFATFS::fsgetlabel(const char* path, char * label, uint32_t* vsn) 
{
    const char *    pathT   = path;
    int             iVol    = get_ldnumber(&pathT);

    if(iVol == VOLSD && fYielding) return(FR_WAITING_FOR_THREAD);
    return(f_getlabel(path, label, vsn));
}

FRESULT DFATFS::fssetlabel(const char* label)
{
    if(fYielding) return(FR_WAITING_FOR_THREAD);
    return(f_setlabel(label));
}
#endif






