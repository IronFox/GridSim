#ifndef memory_shieldH
#define memory_shieldH

/******************************************************************

Memory-log and -verification-tool.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/


#include <stdio.h>
#if SYSTEM==UNIX
    #include "../general/typedefs.h"
    #include <string.h>
#endif
#include "../general/exception.h"


#define SHIELD_ENTRIES  0x10000


struct TMemorySection
{
        void            **root,*owner;
        unsigned        size;
};

struct TMemoryField
{
        void            *root;
        unsigned        size;
};

class MemoryShield
{
private:
static  TMemorySection  section[SHIELD_ENTRIES];
static  TMemoryField    field[SHIELD_ENTRIES];
static  unsigned        sections;
static  unsigned        fields;
static  char            buffer[0x100];
		TCodeLocation	location;
        FILE           *f;
        unsigned        id;

        void            castError(const char*err);
        void            log(const char*line);
        
public:


                        MemoryShield();
virtual                ~MemoryShield();

        MemoryShield&  locate(const char*file, const char*func, int line_);
        MemoryShield&  identify(unsigned id);
        void**          allocate(void*owner, unsigned size);
        void            delocate(void*owner, void**root);
        bool            shielded(void*data);
        bool            covered(void*data);
        bool            occupied(void*pntr, unsigned size);
        void            release(const void*data);
        void            shield(void*pntr, unsigned size);
        void            verify(const void*owner, void**root, void**offset);
        void*           retrieve(const void*owner, void**root, void**offset);

template <class Type> inline
        Type*           shieldArray(Type*pntr, unsigned cnt)
        {
                        shield(pntr,cnt*sizeof(Type));
                        return pntr;
        }

template <class Type> inline
        Type*           shield(Type*pntr)
        {
                        shield(pntr, sizeof(Type));
                        return pntr;
        }

};

extern MemoryShield    mem_shield;
extern unsigned         global_list_id;


#endif
