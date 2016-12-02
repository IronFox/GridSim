#include "../global_root.h"
#include "memory_shield.h"


/******************************************************************

Memory-log and -verification-tool.

******************************************************************/

#define TRACE_ADDRESS   0x30eae98
#define TRACE_ID        792

unsigned        MemoryShield::sections(0);
unsigned        MemoryShield::fields(0);
TMemorySection  MemoryShield::section[SHIELD_ENTRIES];
TMemoryField    MemoryShield::field[SHIELD_ENTRIES];
char            MemoryShield::buffer[0x100];

unsigned        global_list_id(0);
void            *traced_addr=NULL;

MemoryShield   mem_shield;


#ifdef _MSC_VER
	#define snprintf	sprintf_s
#endif



void MemoryShield::castError(const char*err)
{
    log(err);
    for (unsigned i = 0; i < sections; i++)
        fprintf(f,"      0x%p (owned by 0x%p) size: %u\n",section[i].root,section[i].owner,section[i].size);


    throw Except::Fatal(location,err);
}

void MemoryShield::log(const char*err)
{
    if (!f)
        return;
        
        
    if (id == -1)
        fprintf(f,"(%s) %s\n",location.formatted,err);
    else
        fprintf(f,"(%s {%u}) %s\n",location.formatted,id,err);

    fflush(f);
}

MemoryShield::MemoryShield()
{
    f = fopen("memory.log","wb");
}

MemoryShield::~MemoryShield()
{
    if (!f)
        return;
    fprintf(f,"\nmemory-shield closing down.\nremaining sections are:\n");
    for (unsigned i = 0; i < sections; i++)
        fprintf(f," 0x%p\n",section[i].root);

    fprintf(f,"\nremaining fields are:\n");
    for (unsigned i = 0; i < fields; i++)
        fprintf(f," 0x%p\n",field[i].root);

    fflush(f);
    fclose(f);
}

MemoryShield&  MemoryShield::identify(unsigned id_)
{
    id = id_;
    return *this;
}

MemoryShield&  MemoryShield::locate(const char*file, const char*func, int line_)
{
	location = TCodeLocation(file,func,line_);
    id = -1;
    return *this;
}


void MemoryShield::shield(void*data, unsigned size)
{
    #ifdef TRACE_ID
        if (traced_addr && !covered(traced_addr))
            castError("lost traced address at shield0");
    #endif


    #ifdef TRACE_ADDRESS
        if (data == (void*)TRACE_ADDRESS)
            log("[traced address shielded]");
    #endif
    
    #ifdef TRACE_ID
        if (id == TRACE_ID)
        {
			snprintf(buffer,sizeof(buffer),"<now tracing 0x%p>",data);
            log(buffer);
            traced_addr = data;
        }
    #endif
        
        
    if (fields >= SHIELD_ENTRIES)
        castError("shield-capacity exceeded");
    for (unsigned i = 0; i < sections; i++)
        if ((BYTE*)data+size > (BYTE*)section[i].root && data < section[i].root+section[i].size)
        {
            snprintf(buffer,sizeof(buffer),"cannot shield 0x%p (%u bytes) - area used by field 0x%p",data,size,section[i].root);
            castError(buffer);
        }
        
        
    unsigned    lower = 0,
                upper = fields;
    while (lower< upper && upper <= fields)
    {
        unsigned el = (lower+upper)/2;
        TMemoryField&element = field[el];
        if (data >= (BYTE*)element.root + element.size)
            upper = el;
        else
            if ((BYTE*)data+size <= element.root)
                lower = el+1;
            else
            {
                snprintf(buffer,sizeof(buffer),"cannot shield 0x%p (%u bytes) - area occupied (used by 0x%p)",data,size,element.root);
                castError(buffer);
            }
    }
    for (unsigned i = fields; i > lower; i--)
        field[i] = field[i-1];
    fields++;

    field[lower].root = data;
    field[lower].size = size;
    snprintf(buffer,sizeof(buffer),"0x%p (%u bytes) shielded",data,size);
    log(buffer);
    #ifdef TRACE_ID
        if (traced_addr && !covered(traced_addr))
            castError("lost traced address at shield1");
    #endif

}

void MemoryShield::release(const void*data)
{
    #ifdef TRACE_ID
        if (traced_addr && !covered(traced_addr))
            castError("lost traced address at release0");
    #endif

    #ifdef TRACE_ADDRESS
        if (data == (void*)TRACE_ADDRESS)
            log("[traced address released]");
    #endif
    #ifdef TRACE_ID
        if (data == traced_addr)
        {
            snprintf(buffer,sizeof(buffer),"<release operation {%u} on 0x%p (traced address is 0x%p)>",id,data,traced_addr);
            log(buffer);
            traced_addr = NULL;
        }
    #endif

    unsigned    lower = 0,
                upper = fields;
    while (lower< upper && upper <= fields)
    {
        unsigned el = (lower+upper)/2;
        TMemoryField&element = field[el];
        if (data >= (BYTE*)element.root + element.size)
            upper = el;
        else
            if (data < element.root)
                lower = el+1;
            else
            {
                if (element.root != data)
                {
                    snprintf(buffer,sizeof(buffer),"cannot release 0x%p",data);
                    castError(buffer);
                }
                fields--;
                for (unsigned i = el; i < fields; i++)
                    field[i] = field[i+1];
                snprintf(buffer,sizeof(buffer),"0x%p released",data);
                log(buffer);
                #ifdef TRACE_ID
                    if (traced_addr && !covered(traced_addr))
                        castError("lost traced address at release1");
                #endif

                return;
            }
    }
    snprintf(buffer,sizeof(buffer),"cannot release 0x%p",data);
    castError(buffer);
}

bool MemoryShield::shielded(void*data)
{
    unsigned    lower = 0,
                upper = fields;
    while (lower< upper && upper <= fields)
    {
        unsigned el = (lower+upper)/2;
        TMemoryField&element = field[el];
        if (data >= (BYTE*)element.root + element.size)
            upper = el;
        else
            if (data < element.root)
                lower = el+1;
            else
                return (element.root == data);
    }
    
    return false;
}


bool MemoryShield::covered(void*data)
{
    for (unsigned i = 0; i < sections; i++)
        if (data >= section[i].root && data < section[i].root+section[i].size)
            return true;
            
    unsigned    lower = 0,
                upper = fields;
    while (lower< upper && upper <= fields)
    {
        unsigned el = (lower+upper)/2;
        TMemoryField&element = field[el];
        if (data >= (BYTE*)element.root + element.size)
            upper = el;
        else
            if (data < element.root)
                lower = el+1;
            else
                return true;
    }

    return false;
}

bool MemoryShield::occupied(void*data, unsigned size)
{
    for (unsigned i = 0; i < sections; i++)
        if ((BYTE*)data+size > (BYTE*)section[i].root && data < section[i].root+section[i].size)
            return true;

    unsigned    lower = 0,
                upper = fields;
    while (lower< upper && upper <= fields)
    {
        unsigned el = (lower+upper)/2;
        TMemoryField&element = field[el];
        if ((BYTE*)data+size > (BYTE*)element.root + element.size)
            upper = el;
        else
            if (data < element.root)
                lower = el+1;
            else
                return true;
    }

    return false;
}


void** MemoryShield::allocate(void*owner, unsigned size)
{
    if (sections >= SHIELD_ENTRIES-1)
        castError("shield-buffer exceeded");
    #ifdef TRACE_ID
        if (traced_addr && !covered(traced_addr))
            castError("lost traced address at allocate0");
    #endif

    void**pntr = new void*[size];
    #ifdef TRACE_ADDRESS
        if (pntr == (void**)TRACE_ADDRESS)
		{}
    #endif
    




    if (occupied((void*)pntr,size*sizeof(void*)))
    {
        snprintf(buffer,sizeof(buffer),"section-allocation-error! allocated section 0x%p is occupied",pntr);
        castError(buffer);
    }

    section[sections].root = pntr;
    section[sections].owner = owner;
    section[sections++].size = size;
    snprintf(buffer,sizeof(buffer),"0x%p (%u entry/ies) allocated",pntr,size);
    log(buffer);
    #ifdef TRACE_ADDRESS
        if (pntr == (void**)TRACE_ADDRESS)
            log("[traced address allocated]");
    #endif
    #ifdef TRACE_ID
        if (id == TRACE_ID)
        {
            snprintf(buffer,sizeof(buffer),"<now tracing section 0x%p>",pntr);
            log(buffer);
            traced_addr = (void*)pntr;
        }
    #endif
    #ifdef TRACE_ID
        if (traced_addr && !covered(traced_addr))
            castError("lost traced address at allocate1");
    #endif


    return pntr;
}

void MemoryShield::delocate(void*owner, void**root)
{
    #ifdef TRACE_ID
        if (traced_addr && !covered(traced_addr))
            castError("lost traced address at delocate0");
    #endif

    #ifdef TRACE_ID
        if ((void*)root == traced_addr)
        {
            snprintf(buffer,sizeof(buffer),"<section delocated {%u} 0x%p>",id,traced_addr);
            log(buffer);
            traced_addr = NULL;
        }
    #endif
    
    for (unsigned i = 0; i < sections; i++)
        if (section[i].root == root)
        {
            if (section[i].owner != owner)
            {
                snprintf(buffer,sizeof(buffer),"owner-mismatch for section 0x%p should be 0x%p but is 0x%p",root,section[i].owner,owner);
                castError(buffer);
            }
            delete[] section[i].root;
            snprintf(buffer,sizeof(buffer),"0x%p (%u entry/ies) delocated",section[i].root,section[i].size);
            log(buffer);

            sections--;
            for (unsigned j = i; j < sections; j++)
                section[j] = section[j+1];

            #ifdef TRACE_ADDRESS
                if (root == (void**)TRACE_ADDRESS)
                    log("[traced address delocated]");
            #endif
            #ifdef TRACE_ID
                if (traced_addr && !covered(traced_addr))
                    castError("lost traced address at delocate1");
            #endif

            return;
        }
    snprintf(buffer,sizeof(buffer),"unable to locate section 0x%p",root);
    castError(buffer);
}

void MemoryShield::verify(const void*owner, void**root, void**offset)
{
    #ifdef TRACE_ID
        if (traced_addr && !covered(traced_addr))
            castError("lost traced address at verify0");
    #endif

    for (unsigned i = 0; i < sections; i++)
        if (section[i].root == root)
        {
            if (section[i].owner != owner)
            {
                snprintf(buffer,sizeof(buffer),"owner-mismatch for root 0x%p should be 0x%p but is 0x%p",root,section[i].owner,owner);
                castError(buffer);
            }
            else
                if ((unsigned long)(offset-root) >= section[i].size)
                {
					snprintf(buffer, sizeof(buffer), "index of section 0x%p out of bounds: %lli / %u", root, (UINT64)(offset - root), section[i].size);
                    castError(buffer);
                }
            return;
        }
    #ifdef TRACE_ID
        if (traced_addr && !covered(traced_addr))
            castError("lost traced address at verify1");
        else
            if (!traced_addr)
                log("<not tracing>");
    #endif

    snprintf(buffer,sizeof(buffer),"section not found: 0x%p",root);
    castError(buffer);
}

void*MemoryShield::retrieve(const void*owner, void**root, void**offset)
{
    verify(owner,root,offset);
    return (*offset);
}

