// SVN Info: $Id: TCReadACQU.cxx 862 2011-03-19 20:22:10Z werthm $

/*************************************************************************
 * Author: Dominik Werthmueller
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TCReadACQU                                                           //
//                                                                      //
// Read raw ACQU MK1 files.                                             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#include "TCReadACQU.h"

ClassImp(TCReadACQU)

TCReadACQU::TCReadACQU(const Bool_t isFileMk2) : fPath(0), fFiles(0), isMk2(isFileMk2)
{
}
//______________________________________________________________________________
TCReadACQU::TCReadACQU(const Char_t* path, const Bool_t isFileMk2) : isMk2(isFileMk2)
{
    // Constructor using the path of the raw files 'path'.
    
    // init members
    fPath = new Char_t[256];
    fFiles = new TList();
    fFiles->SetOwner(kTRUE);
    
    // copy path
    strcpy(fPath, path);

    // read all files
    ReadFiles();
}

//______________________________________________________________________________
TCReadACQU::~TCReadACQU()
{
    // Destructor.
    
    if (fPath) delete [] fPath;
    if (fFiles) delete fFiles;
}

//______________________________________________________________________________
void TCReadACQU::ReadFiles()
{
    // Read all raw files.

    // user information
    Info("ReadFiles", "Looking for ACQU raw files in '%s'", fPath);
    
    // try to get directory content
    TSystemDirectory dir("rawdir", fPath);
    TList* list = dir.GetListOfFiles();
    if (!list)
    {
        Error("ReadFiles", "'%s' is not a directory!", fPath);
        return;
    }

    // sort files
    list->Sort();

    // loop over directory content
    TIter next(list);
    TSystemFile* f;
    while ((f = (TSystemFile*)next()))
    {
        // look for ACQU raw files
        TString str(f->GetName());
        if (str.BeginsWith("CB_") && (str.EndsWith(".dat") || str.EndsWith(".dat.gz")))
        {
            // user information
            Info("ReadFiles", "Reading '%s/%s'", fPath, f->GetName());
            
            // create file object
            TCACQUFile* acqufile = new TCACQUFile(isMk2);
            acqufile->ReadFile(fPath, f->GetName());
            
            // check file 
            if (!acqufile->IsGoodStartMarker())
            {
                Warning("ReadFiles", "Bad file header found in '%s/%s'", fPath, f->GetName());
            }
            
            // add file to list
            fFiles->Add(acqufile);
        }
    }

    // clean-up
    delete list;
}

