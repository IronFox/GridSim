#include "../global_root.h"
#include "texture_buffer.h"
#include "../string/encoding.h"

/******************************************************************

Texture-buffer allowing to buffer/write simple texture-collections.

******************************************************************/


namespace DeltaWorks
{







	void TextureBuffer::Add(const CGS::TextureA&texture)
	{
		if (Super::IsSet(texture))
			return;
		Super::Set(texture);
	}



	void TextureBuffer::Write(const PathString&filename)
	{
		Riff::File  riff;
		if (comment.length() > TypeInfo<Riff::RIFF_SIZE>::max)
			throw Except::IO::ParameterFault(CLOCATION,"Specified comment exceeds maximum allowed size");
		if (!riff.Create(filename.c_str()))
			throw Except::IO::DriveAccess(CLOCATION,"Unable to create file '"+String(filename)+"'");
		UINT32 version = 0x0103;
		riff.AppendBlock("VERS",&version,sizeof(version));
		riff.AppendBlock("CONT",comment.c_str(),Riff::RIFF_SIZE(comment.length()));
		Super::VisitAllKeys([&riff](const CGS::TextureA&tex)
		{
			riff.AppendBlock("ID  ",&tex.name,sizeof(tex.name));
			for (BYTE k = 0; k < tex.face_field.length(); k++)
			{
				if (tex.face_field[k].size() > TypeInfo<Riff::RIFF_SIZE>::max)
					throw Except::IO::ParameterFault(CLOCATION,"Specified face exceeds maximum allowed size");
				riff.AppendBlock("FACE",tex.face_field[k].pointer(),Riff::RIFF_SIZE(tex.face_field[k].size()));
			}
		});
	}

	String TextureBuffer::GetState()
	{
		String rs = String(Count())+" entry/ies\n";

		index_t cnt = 0;
		Super::VisitAllKeys([&rs,&cnt](const CGS::TextureA&tex)
		{
			rs+=" ("+String(cnt)+"): \""+name2str(tex.name)+"\" "+String(tex.face_field.length())+" face(s)\n";
			cnt++;
		});
		return rs;
	}

}
