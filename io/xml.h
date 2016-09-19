#ifndef xmlH
#define xmlH

#include "../global_string.h"
#include "../container/lvector.h"
#include "../container/sortedlist.h"
#include "../container/hashlist.h"
#include "../io/string_file.h"
#include "../string/string_buffer.h"
#include "../container/buffer.h"


/******************************************************************

E:\include\io\xml.h

******************************************************************/

namespace XML
{
	struct TScannerNode;
	class ScannerRule;
	class Scanner;



	enum class Encoding
	{
		UTF8,
		ANSI
	};



	enum export_style_t	//! Style used to export XML content to files or string buffers
	{
		Raw,	//!< Export data as available. Whitespaces and newlines are exported if they are contained in the exporting structure
		Nice,	//!< Export inner content if XML node has no children and ignore node-following content. Newlines and indentation are added where appropriate
		Tidy	//!< Export inner content if XML node has no children and ignore node-following content. Neither newlines nor indentation are added
	};
	

	struct  TAttribute
	{
		String					name,
								value;
	};

	class Node
	{
	public:
	    String				name,
							inner_content,
							following_content;
	                    
	    //Node          			*parent;
		Buffer<Node,0,Swap>	children;
			
			
		StringMappedList<TAttribute>
							attributes;
	                            
								//Node()/*:parent(NULL)*/    {}
								//Node(const Node&other);
								
		Node&				NewChild(const String&name);
		Node&				AddChild(const String&name)			{return NewChild(name);}
		Node&				AddNewChild(const String&name)		{return NewChild(name);}
		TAttribute*			Set(const String&attrib_name, const String&attrib_value);
		Node&				SetMore(const String&attrib_name, const String&attrib_value)	{Set(attrib_name,attrib_value); return *this;}
		void				Unset(const String&attrib_name);
	    Node*          		Find(const String&path);				//!< Returns the first matching node (if any) @param path Path string in the form 'node/subnode/.../subnodeN' @return Pointer to the matching node or NULL if no such could be found
	    const Node*			Find(const String&path)		const;	//!< @copydoc find()
		Node&				Create(const String&path, const String&inner_content="");
		bool				Query(const String&attrib_name, String&val_out)	const;	//!< Queries the string content of the specified attribute of the local node. The method does not crash if @a this is NULL. \param attrib_name Name of the requested attribute \param val_out String reference to store the respective attribute's value in \return true if the local object is not NULL and the requested attribute exists, false otherwise.
		bool				Query(const char*attrib_name, String&val_out)	const;	//!< @copydoc Query()
		bool				Query(const String&attrib_name, StringRef&val_out)	const;	//!< Queries the string content of the specified attribute of the local node. The method does not crash if @a this is NULL. \param attrib_name Name of the requested attribute \param val_out String reference to store the respective attribute's value in \return true if the local object is not NULL and the requested attribute exists, false otherwise.
		bool				Query(const char*attrib_name, StringRef&val_out)	const;	//!< @copydoc Query()
		void				swap(Node&other);
	};

	class Container
	{
	private:

	public:
        Node           		root_node;
		Encoding			encoding;							//!< Character encoding

		/**/				Container()						{root_node.name = "xml"; encoding = Encoding::UTF8; }
		/**/				Container(const PathString&source)	{root_node.name = "xml"; encoding = Encoding::UTF8; LoadFromFile(source);}
		void				Clear();													//!< Clears all local data
		Node*				Find(const String&path);				//!< Returns the first matching node (if any) @param path Path string in the form 'node/subnode/.../subnodeN' @return Pointer to the matching node or NULL if no such could be found
		const Node*			Find(const String&path)		const;	//!< @copydoc find()
		Node&				Create(const String&path, const String&inner_content="");

		/**
		@brief Attempts to load the local XML configuration from the specified char field
				
		The specified char field must include the terminating 0 and be mutable. The array length will not be altered but dividing zeroes will be inserted as necessary
		@param field Char array as specified
		*/
		void				LoadFromCharArray(ArrayData<char>&field);
		/**
		@brief Attempts to load the local XML configuration from the specified file
		@param filename Name of the file to load
		@param content_out Optional string to store unprocesses file content in. Pass valid string pointer or NULL if no content exporting should be performed
		*/
        void                LoadFromFile(const PathString&filename, String*content_out=NULL);
		/**
		@brief Attempts to load the local XML configuration from the specified content string
		@param content String to parse
		*/
		void				LoadFromString(const String&content);
		void                SaveToFile(const PathString&filename, export_style_t style=Nice) const;
		void				SaveToStringBuffer(StringBuffer&target, export_style_t style=Nice) const;
	};
	
	
	
	struct TScannerNode
	{
		StringMappedList<TAttribute>
							attributes;
		String				content;
		TScannerNode		*parent;
		ScannerRule			*rule;
	};
	
	
	class ScannerRule
	{
	public:
			typedef void(*pNodeCallback)(TScannerNode*node);
	protected:
			StringTable<ScannerRule*>
								connections;
			Scanner				*parent;
			String				name;
			bool				want_content;
			pNodeCallback		on_enter,on_exit;
			friend class Scanner;	//only gcc knows why i need to declare this :S
	public:
								ScannerRule(const String&name, Scanner*parent, bool want_content, pNodeCallback on_enter, pNodeCallback on_exit);
			ScannerRule*		Map(const String&name, bool want_content=true,pNodeCallback on_enter=NULL, pNodeCallback on_exit=NULL);
			ScannerRule*		Map(ScannerRule*node);
			bool				Unmap(const String&name);
			bool				Unmap(ScannerRule*node);
	};
	
	
	class Scanner:public ScannerRule
	{
	private:
			List::Vector<ScannerRule>
								rules;
			pNodeCallback		except;
			char				buffer[0x1000];
			size_t				buffer_fill_state;
			String				error_string;
			bool				error_is_filenotfound;
			
			friend class ScannerRule;
			
			bool				read(char until, StringBuffer&to, FILE*f);
			bool				skip(char until, FILE*f);
			bool				skip(const char*until, FILE*f);
	public:
								Scanner();
			void				SetExcept(pNodeCallback callback_on_except=NULL);
			bool				Scan(const PathString&filename);
			const String&		GetError()						const;
			bool				ErrorIsFileNotFound()			const;
	};
}

#endif
