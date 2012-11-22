#include <cstdlib>
#include <iostream>

using namespace std;

#include <stdio.h>

#include "../global_root.h"
#include "../io/file_system.h"
#include "../container/lvector.h"
#include "../container/sortedlist.h"
#include "../string/tokenizer.h"
#include "../string/token_parser.h"
#include "../io/string_file.h"
#include "../general/system.h"



enum Type {StringType,Sharp,Include,StringMarker,CharMarker,EscapedStringMarker,EscapedCharMarker,EscapedEscape,CommentMarker,CommentBegin,
            CommentEnd,NewLine,StringNewLine,StringCarrierReturn,Space,BlockEnd,BlockBegin};




CTokenParser    tokenparser;
CTokenizer  tokenizer;
CFSFolder   local;

bool        build_failed = false,
            recompile=false,
            first_only=false;
unsigned    compilables(0),
            build_total(0),
            build_index(0);
CString     compile_flags;

struct TFile
{
    CFSFile                 ident;
    CString                 name,
                            ext;
    bool                    compilable;
    long                    local_date,
                            global_date;
    TFile                  *global_dep;
    CReferenceVector<TFile> dependents;
};




CSorted<CVector<TFile>,NameSort>           files;







void scan(CFSFolder&folder)
{
    folder.reset();
    while (const CFSFile*file = folder.nextEntry())
    {
        if (file->isFolder())
        {
            CFSFolder child(file);
            if (file->getName()!="maker" && file->getName()!="Nx" && file->getName()!="cuda")
            {
                scan(child);
            }
            continue;
        }
            
        CString ext = CString(file->getExtension()).LowerCase();
        if (ext == "h" || ext=="c" || ext=="cpp")
        {
            TFile*f = SHIELDED(new TFile());
            f->ident = *file;
            f->name = file->getLocation();
            f->ext = ext;
            f->compilable = ext != "h";
            compilables += f->compilable;
            f->local_date = f->global_date = file->fileTime();
            f->global_dep = f;
            files.insert(f);
        }
    }
}




void link(TFile*file)
{
    CString addr = file->ident.getLocation();
    
    CString name = FileSystem::extractFilePath(addr)+FOLDER_SLASH+'.'+FileSystem::extractFileNameExt(addr)+".ref";
    if (recompile || FileSystem::fileTime(name) < file->ident.fileTime())
    {
        cout << "updating dependencies of "<<file->ident.toString().c_str()<<"...\n";
        FILE*f = fopen(file->ident.getLocation().c_str(),"rb");
        if (!f)
        {
            cout << "unable to read content. aborting.\n";
            return;
        }
        fseek(f,0,SEEK_END);
        unsigned size = ftell(f);
        fseek(f,0,SEEK_SET);

        CTokenList tokens;

        char*buffer = alloc<char>(size+1);
            fread(buffer,1,size,f);
            fclose(f);
            buffer[size] = 0;

            tokens.clear();
            tokenizer.parse(buffer,tokens);

        deloc(buffer);

        CParseResult rs;

        CStringFile out;
        if (!out.create(name))
        {
            cout << "unable create ref file. aborting.\n";
            return;
        }
        unsigned cnt = 0;

        tokenparser.parse(tokens,&rs);

        rs.reset();
        while (CSortedStream*sequence = rs.each())
        {
            TParserSequence*stream;
            CString refname;
            if ((stream = sequence->lookup("IncludeAbsolute")) && stream->count()>1)
                refname = local.locationStr()+FOLDER_SLASH_STR+stream->get(1)->content;
            else
                if ((stream = sequence->lookup("IncludeRelative")) && stream->count()>1)
                    refname = file->ident.getFolder()+FOLDER_SLASH_STR+stream->get(1)->content;
            if (refname.Length() && FileSystem::isFile(refname))
            {
                out << refname << nl;
//                cout << refname.c_str() << endl;
                cnt++;
            }
        }
        if (!cnt)
            out << "(*)\n";
        
    }
    
    CStringFile in;
    if (!in.open(name))
    {
        cout << "unable to open reference file of "<<file->ident.toString().c_str()<<endl;
        return;
    }
    CString dep;
    while (in >> dep)
    {
        if (dep == "(*)")
            continue;
        for (unsigned i = 1; i < dep.Length(); i++)
            if (dep[i]=='/' || dep[i]=='\\')
                dep[i] = FOLDER_SLASH;

        CString path = FileSystem::extractFilePath(dep),
                name = FileSystem::extractFileNameExt(dep);
        CFSFolder folder(path);
        if (!folder.validLocation())
        {
            cout << "folder '"<<path.c_str()<<"', referenced from "<<file->ident.getName().c_str()<<" does not exist.\n";
            continue;
        }
        
        const CFSFile*f = folder.find(name);
        if (!f)
        {
            cout << "file '"<<folder.locationStr().c_str()<<" | "<<name.c_str()<<"', referenced from "<<file->ident.getName().c_str()<<" does not exist.\n";
            continue;
        }

                
                
        TFile*link = files.lookup(f->getLocation());
        if (link)
            link->dependents.append(file);
        else
            cout << "dependency '"<<f->getLocation().c_str()<<"', referenced from "<<file->ident.getName().c_str()<<" is unknown.\n";
    }


}


void update()
{
    files.reset();
    while (TFile*file = files.each())
        link(file);
}

void cast(TFile*from, TFile*file)
{
    if (file->global_date>=from->global_date)
        return;
    file->global_date = from->global_date;
    file->global_dep = from;
    
    for (unsigned i = 0; i < file->dependents; i++)
        cast(from,file->dependents[i]);
}



void cast()
{
    files.reset();
    while (TFile*file = files.each())
    {
        for (unsigned i = 0; i < file->dependents; i++)
            cast(file,file->dependents[i]);
    }
}


void buildFile(TFile*file)
{
    CString obj_file = FileSystem::extractFilePathName(file->ident.getLocation())+".o",
            cmd;
   if (file->ext == "c")
        cmd = "gcc";
    else
        cmd = "g++";
    cmd += " -c "+file->ident.getLocation()+" -o "+obj_file+compile_flags;
    cout << "compiling "<<file->ident.getLocation().c_str()<<" ("<<++build_index<<"/"<<build_total<<")...\n";
    //cmd.c_str() << endl;
    build_failed = system(cmd.c_str()) || build_failed;
}

void build()
{
    CReferenceVector<TFile> compile_list;
    files.reset();
    while (TFile*file = files.each())
        if (file->compilable)
        {
            CString obj_file = FileSystem::extractFilePathName(file->ident.getLocation())+".o";
            if (recompile || FileSystem::fileTime(obj_file) < file->global_date)
                compile_list.append(file);
        }
    build_total = compile_list.count();
    compile_list.reset();
    while (TFile*file = compile_list.each())
    {
        buildFile(file);
        if (build_failed && first_only)
            return;
    }
}



int main()
{
	try
	{
		for (unsigned i = 1; i < argc(); i++)
			if (!strcmp(argv(i),"--new"))
				recompile = true;
			else
				if (!strcmp(argv(i),"--first"))
					first_only = true;
				else
					compile_flags+=" "+CString(argv(i));
	

        tokenizer.setStringIdent(StringType);
        tokenizer.reg("#include",Include,false,true);
        tokenizer.reg("\"",StringMarker);
        tokenizer.reg("'",CharMarker);
        tokenizer.reg("\\\"",EscapedStringMarker);
        tokenizer.reg("\\'",EscapedCharMarker);
        tokenizer.reg("\\\\",EscapedEscape);
        tokenizer.reg("//",CommentMarker);
        tokenizer.reg("/*",CommentBegin);
        tokenizer.reg("*/",CommentEnd);
        tokenizer.reg("\r\n",NewLine);
        tokenizer.reg("\n",NewLine);
        tokenizer.reg("\\n",StringNewLine);
        tokenizer.reg("\\r",StringCarrierReturn);
        tokenizer.reg(" ",Space);
        tokenizer.reg("  ",Space);
        tokenizer.reg("   ",Space);
        tokenizer.reg("    ",Space);
        tokenizer.reg("     ",Space);
        tokenizer.reg("      ",Space);

        tokenizer.reg(">",BlockEnd);
        tokenizer.reg("<",BlockBegin);
		tokenizer.finalize();



        tokenparser.createStatus("Abort",PO_ABORT);
        tokenparser.createStatus("Finish",PO_FINISH);
        tokenparser.createStatus("Include");
        tokenparser.createStatus("IncludeAbsolute");
        tokenparser.createStatus("IncludeRelative");

        tokenparser.define("Include",Include);
        tokenparser.route("Include",BlockBegin,"IncludeAbsolute");
        tokenparser.route("Include",StringMarker,"IncludeRelative");
        tokenparser.route("IncludeAbsolute",BlockEnd,"Finish");
        tokenparser.route("IncludeRelative",StringMarker,"Finish");
        
        
        tokenparser.route("Include",NewLine,"Finish");

        tokenparser.globalBlock(StringMarker,0,BT_TOGGLE);
        tokenparser.globalBlock(CharMarker,1,BT_TOGGLE);
        tokenparser.globalBlock(CommentMarker,2,BT_BLOCK);
        tokenparser.globalBlock(NewLine,2,BT_UNBLOCK);
        tokenparser.globalBlock(CommentBegin,3,BT_BLOCK);
        tokenparser.globalBlock(CommentEnd,3,BT_UNBLOCK);

        tokenparser.createStatus("BlockComment");
        tokenparser.createStatus("LineComment");
        tokenparser.define("BlockComment",CommentBegin);
        tokenparser.route("BlockComment",CommentEnd,"Finish");
        tokenparser.define("LineComment",CommentMarker);
        tokenparser.route("LineComment",NewLine,"Finish");



        local = FileSystem::workingDirectory();






    /*if (!local.exit())
    {
        cout << "navigation error in "<<local.toString().c_str()<<". aborting.\n";
        system("PAUSE");
        return EXIT_SUCCESS;
    }*/
		
		scan(local);
		
		update();
		cast();
		cout << "building...\n";
		build();
	
		if (build_failed)
		{
			cout << "compilation of one or more files has failed. sorry :(\n";
			return -1;
		}
		else
		{
			CString libname;
			#if SYSTEM==WINDOWS
				libname = "libinkworks.a";
			#elif SYSTEM==UNIX
				libname = "libinkworks_x.a";
			#else
				libname = "libinclude.a";
			#endif
			cout << "compiling library...\n";
			CString cmd = "ar r ./"+libname;
			files.reset();
			while (TFile*file = files.each())
				if (file->ext != "h")
					cmd += " "+FileSystem::extractFilePathName(file->name)+".o";
			if (system(cmd.c_str()))
			{
				cout << "ar call failed :/:\n"<<cmd.c_str()<<endl;
				return -1;
			}
			if (system("ranlib ./"+libname))
			{
				cout << "ranlib call failed :S\n";
				return -1;
			}
			cout << "all done :)\n";
		}
	}
	catch (CException*exception)
	{
		cout << exception->toString()<<endl;
	
	}
   
    return EXIT_SUCCESS;
}
