/************************************************************************/
/*                                                                      */
/*    DeferredTasks.h                                                   */
/*                                                                      */
/*    Manage deferred tasks                                             */
/*                                                                      */
/************************************************************************/
/*    Author:     Keith Vogel                                           */
/*    Copyright 2018, Digilent Inc.                                     */
/************************************************************************/
/*  Revision History:                                                   */
/*    8/28/2018(KeithV): Created                                        */
/************************************************************************/



class DEFTask 
{
    public:

        typedef enum
        {       
            None,
            CalDC1,
            CalDC2,
            CalAWG,
            CalDAQ1,
            CalDAQ2,
            CalDAQ3,
            CalDAQ4,
            CalDAQ5,
            CalDAQ6,
            CalDAQ7,
            CalDAQ8,
            CalGndDAQ1,
            CalGndDAQ2,
            CalGndDAQ3,
            CalGndDAQ4,
            CalGndDAQ5,
            CalGndDAQ6,
            CalGndDAQ7,
            CalGndDAQ8,
            UpdWFFW,
            UpdWFCert,
            SoftReset,
            SaveCalibration,
            ResetInstruments,
            FileDelete,
            FileLog,
            WIFIConnect,
            WIFIDisconnect,
            WIFIScan,
        } DFER;

        static const int32_t UNTIL = -1;

    private:

        static const uint32_t cMaxTask = 50;
        struct
        {
            DFER        dTask; 
            int32_t     tmsWait; 
            uint32_t    cbScratch;
            uint8_t __attribute__((aligned(4))) scratch[32];
        } taskInfo[cMaxTask];
        uint32_t idTaskNext;
        uint32_t idTaskCur;
 
    public:
        DEFTask() : idTaskNext(0), idTaskCur(0) { };

        bool QueTask(DFER dTask, void * pData, uint32_t cbData, int32_t tmsWait);
        bool QueTask(DFER dTask, int32_t tmsWait)
        {
            return(QueTask(dTask, NULL, 0, tmsWait));
        };

        int32_t GetWaitTime(void);
        int32_t GetWaitTime(DFER dTask);

        STATE PeriodicTasks(void);
};


class FILETask 
{
    public:

        typedef enum
        {
            NOTHING,
            LOGGING,
            DELETING,
            CALIBRATING,
            WRITING,
            READING,
            DIRECTORY,
            ENDDOING
        } DOING;

    private:

        struct
        {
            VOLTYPE vol;
            char szQualifiedFilePath[256];
        } rgUsage[ENDDOING];

        char szQualPath[256];
        
    public:

        static char const * const   rgszDoing[ENDDOING];
        static DFILE *              rgpdFile[ENDDOING];

        char const * const MkQualifiedPath(VOLTYPE vol, char const * const szPath);

        FILETask()
        {
            memset(rgUsage, 0, sizeof(rgUsage));   
            memset(szQualPath, 0, sizeof(szQualPath));
        }

        DOING IsQualPathInUse(char const * const szQualPath)
        {
            uint32_t i;

            // see if the file is used somewhere
            for(i=1; i<ENDDOING; i++) if(strcasecmp(szQualPath, rgUsage[i].szQualifiedFilePath) == 0) return((DOING) i);

            return(NOTHING);
        }

        bool SetUsage(DOING doing, VOLTYPE volP, char const * const szPath)
        {
            char const * const szQualPath = MkQualifiedPath(volP, szPath);

            // no file, entry in use, no-go
            if(szQualPath == NULL || rgUsage[doing].szQualifiedFilePath[0] != 0) return(false);

            // if the file is in use somewhere else, no go
            if(IsQualPathInUse(szQualPath) != NOTHING)  return(false);
 
            // fill the entry
            strcpy(rgUsage[doing].szQualifiedFilePath, szQualPath);
            rgUsage[doing].vol = volP;

            return(true);
        }   

        bool ClearUsage(DOING doing)
        {
            // if we have a file handle, and it is open; we can't clear
            ASSERT(!(rgpdFile[doing] != NULL && (*rgpdFile[doing])));
//            if(rgpdFile[doing] != NULL && (*rgpdFile[doing])) return(false);
             
            // clear the entry
            memset(&rgUsage[doing], 0, sizeof(rgUsage[0])); 
            return(true);
        }

        char const * const GetPath(DOING doing, bool fQualified)
        {
            uint32_t j = fQualified ? 0 : 3;
            char const * szRet = &rgUsage[doing].szQualifiedFilePath[j];
            
            if(rgUsage[doing].szQualifiedFilePath[0] == 0) return(NULL);
            return(szRet);
        }

        VOLTYPE GetVol(DOING doing)
        {
            return(rgUsage[doing].vol);
        }
};



