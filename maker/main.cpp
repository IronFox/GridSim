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




TokenParser    		tokenparser;
StringTokenizer  	tokenizer;
FileSystem::Folder	local;

bool        build_failed = false,
            recompile=false,
            first_only=false;
unsigned    compilables(0),
            build_total(0),
            build_index(0);
String     	compile_flags;

struct TFile
{
    FileSystem::File		ident;
    String					name,
                            ext;
    bool                    compilable;
    long                    local_date,
                            global_date;
    TFile                  *global_dep;
    List::ReferenceVector<TFile> dependents;
};




Sorted<List::Vector<TFile>,NameSort>           files;







void scan(FileSystem::Folder&folder)
{
    folder.Rewind();
    while (const FileSystem::File*file = folder.NextEntry())
    {
        if (file->IsFolder())
        {
            FileSystem::Folder child(file);
            if (file->GetName()!="maker" && file->GetName()!="Nx" && file->GetName()!="cuda")
            {
                scan(child);
            }
            continue;
        }
            
        String ext = String(file->GetExtension()).copyToLowerCase();
        if (ext == "h" || ext=="c" || ext=="cpp")
        {
            TFile*f = SHIELDED(new TFile());
            f->ident = *file;
            f->name = file->GetLocation();
            f->ext = ext;
            f->compilable = ext != "h";
            compilables += f->compilable;
            f->local_date = f->global_date = file->GetModificationTime();
            f->global_dep = f;
            files.insert(f);
        }
    }
}




void link(TFile*file)
{
    String addr = file->ident.GetLocation();
    
    String name = FileSystem::ExtractFileDir(addr)+FOLDER_SLASH+'.'+FileSystem::ExtractFileNameExt(addr)+".ref";
    if (recompile || FileSystem::GetModificationTime(name) < file->ident.GetModificationTime())
    {
        cout << "updating dependencies of "<<file->ident.ToString().c_str()<<"...\n";
        FILE*f = fopen(file->ident.GetLocation().c_str(),"rb");
        if (!f)
        {
            cout << "unable to read content. aborting.\n";
            return;
        }
        fseek(f,0,SEEK_END);
        unsigned size = ftell(f);
        fseek(f,0,SEEK_SET);

        TokenList tokens;

        char*buffer = alloc<char>(size+1);
            fread(buffer,1,size,f);
            fclose(f);
            buffer[size] = 0;

            tokens.clear();
            tokenizer.parse(buffer,tokens);

        dealloc(buffer);

        Parser::Result rs;

        StringFile out;
        if (!out.create(name))
        {
            cout << "unable create ref file. aborting.\n";
            return;
        }
        unsigned cnt = 0;

        tokenparser.parseTokenSequence(tokens,rs);

		for (Parser::Stream*stream = rs.begin(); stream != rs.end(); ++stream)
        {
			Parser::Sequence* any;
            String refname;
            if ((any = stream->FindAny("IncludeAbsolute")) && any->Length() > 1)
                refname = local.LocationStr()+FOLDER_SLASH_STR+tokens[any->at(1)].content;
            else
                if ((any = stream->FindAny("IncludeRelative")) && any->Length() > 1)
                    refname = file->ident.GetFolder()+FOLDER_SLASH_STR+tokens[any->at(1)].content;
            if (refname.length() && FileSystem::IsFile(refname))
            {
                out << refname << nl;
//                cout << refname.c_str() << endl;
                cnt++;
            }
        }
        if (!cnt)
            out << "(*)\n";
        
    }
    
    StringFile in;
    if (!in.open(name))
    {
        cout << "unable to open reference file of "<<file->ident.ToString().c_str()<<endl;
        return;
    }
    String dep;
    while (in >> dep)
    {
        if (dep == "(*)")
            continue;
        for (unsigned i = 1; i < dep.length(); i++)
            if (dep.get(i)=='/' || dep.get(i)=='\\')
                dep.set(i, FOLDER_SLASH);

        String path = FileSystem::ExtractFileDir(dep),
                name = FileSystem::ExtractFileNameExt(dep);
        FileSystem::Folder folder(path);
        if (!folder.IsValidLocation())
        {
            cout << "folder '"<<path.c_str()<<"', referenced from "<<file->ident.GetName().c_str()<<" does not exist.\n";
            continue;
        }
        
        const FileSystem::File*f = folder.Find(name);
        if (!f)
        {
            cout << "file '"<<folder.LocationStr().c_str()<<" | "<<name.c_str()<<"', referenced from "<<file->ident.GetName().c_str()<<" does not exist.\n";
            continue;
        }

                
                
        TFile*link = files.lookup(f->GetLocation());
        if (link)
            link->dependents.append(file);
        else
            cout << "dependency '"<<f->GetLocation().c_str()<<"', referenced from "<<file->ident.GetName().c_str()<<" is unknown.\n";
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
    String obj_file = FileSystem::ExtractFileDirName(file->ident.GetLocation())+".o",
            cmd;
   if (file->ext == "c")
        cmd = "clang-3.5";
    else
        cmd = "clang++-3.5 -o2 -std=c++11 -Wno-ignored-attributes -ferror-limit=1 -Wno-dangling-else -Wno-undefined-bool-conversion";
    cmd += " -c "+file->ident.GetLocation()+" -o "+obj_file+compile_flags;
    cout << "compiling "<<file->ident.GetLocation().c_str()<<" ("<<++build_index<<"/"<<build_total<<")...\n";
    //cmd.c_str() << endl;
    build_failed = system(cmd.c_str()) || build_failed;
}

void build()
{
    List::ReferenceVector<TFile> compile_list;
    files.reset();
    while (TFile*file = files.each())
        if (file->compilable)
        {
            String obj_file = FileSystem::ExtractFileDirName(file->ident.GetLocation())+".o";
            if (recompile || FileSystem::GetModificationTime(obj_file) < file->global_date)
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



int main(int argc, char *argv[])
{
	try
	{
		for (int i = 1; i < argc; i++)
			if (!strcmp(argv[i],"--new"))
				recompile = true;
			else
				if (!strcmp(argv[i],"--first"))
					first_only = true;
				else
					compile_flags+=" "+String(argv[i]);
	

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
		tokenizer.finalizeInitialization();



        tokenparser.createStatus("Abort",Parser::PO_ABORT);
        tokenparser.createStatus("Finish",Parser::PO_FINISH);
        tokenparser.createStatus("Include");
        tokenparser.createStatus("IncludeAbsolute");
        tokenparser.createStatus("IncludeRelative");

        tokenparser.define("Include",Include);
        tokenparser.route("Include",BlockBegin,"IncludeAbsolute");
        tokenparser.route("Include",StringMarker,"IncludeRelative");
        tokenparser.route("IncludeAbsolute",BlockEnd,"Finish");
        tokenparser.route("IncludeRelative",StringMarker,"Finish");
        
        
        tokenparser.route("Include",NewLine,"Finish");

        tokenparser.globalBlock(StringMarker,0,Parser::BT_TOGGLE);
        tokenparser.globalBlock(CharMarker,1,Parser::BT_TOGGLE);
        tokenparser.globalBlock(CommentMarker,2,Parser::BT_BLOCK);
        tokenparser.globalBlock(NewLine,2,Parser::BT_UNBLOCK);
        tokenparser.globalBlock(CommentBegin,3,Parser::BT_BLOCK);
        tokenparser.globalBlock(CommentEnd,3,Parser::BT_UNBLOCK);

        tokenparser.createStatus("BlockComment");
        tokenparser.createStatus("LineComment");
        tokenparser.define("BlockComment",CommentBegin);
        tokenparser.route("BlockComment",CommentEnd,"Finish");
        tokenparser.define("LineComment",CommentMarker);
        tokenparser.route("LineComment",NewLine,"Finish");



        local = FileSystem::GetWorkingDirectory();






    /*if (!local.exit())
    {
        cout << "navigation error in "<<local.ToString().c_str()<<". aborting.\n";
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
			String libname;
			#if SYSTEM==WINDOWS
				libname = "libinkworks.a";
			#elif SYSTEM==UNIX
				libname = "libinkworks_x.a";
			#else
				libname = "libinclude.a";
			#endif
			cout << "compiling library...\n";
			String cmd = "ar r ./"+libname;
			files.reset();
			while (TFile*file = files.each())
				if (file->ext != "h")
					cmd += " "+FileSystem::ExtractFileDirName(file->name)+".o";
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
	catch (const std::exception&exception)
	{
		cerr << exception.what()<<endl;
	
	}
   
    return EXIT_SUCCESS;
}
