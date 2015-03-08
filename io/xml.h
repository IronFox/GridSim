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
								
		Node&				newChild(const String&name);
		inline Node&		NewChild(const String&name)			{return newChild(name);}
		Node&				addChild(const String&name)			{return newChild(name);}
		inline Node&		AddChild(const String&name)			{return newChild(name);}
		Node&				addNewChild(const String&name)		{return newChild(name);}
		inline Node&		AddNewChild(const String&name)		{return newChild(name);}
		TAttribute*			set(const String&attrib_name, const String&attrib_value);
		inline TAttribute*	Set(const String&attribName, const String&attribValue)	{return set(attribName,attribValue);}
		void				unset(const String&attrib_name);
		inline void			Unset(const String&attribName)	{unset(attribName);}
	    Node*          		find(const String&path);				//!< Returns the first matching node (if any) @param path Path string in the form 'node/subnode/.../subnodeN' @return Pointer to the matching node or NULL if no such could be found
		inline Node*		Find(const String&path)	{return find(path);}
	    const Node*			find(const String&path)		const;	//!< @copydoc find()
		inline const Node*	Find(const String&path)		const	{return find(path);}
		Node&				create(const String&path, const String&inner_content="");
		inline Node&		Create(const String&path, const String&innerContent="")	{return create(path,innerContent);}
		bool				query(const String&attrib_name, String&val_out)	const;	//!< Queries the string content of the specified attribute of the local node. The method does not crash if this is NULL. \param attrib_name Name of the requested attribute \param val_out String reference to store the respective attribute's value in \return true if the local object is not NULL and the requested attribute exists, false otherwise.
		inline bool			Query(const String&attribName, String&valOut)	const	{return query(attribName,valOut);}
		bool				query(const char*attrib_name, String&val_out)	const;	//!< @copydoc query()
		inline bool			Query(const char*attribName, String&valOut)		const	{return query(attribName,valOut);}
		void				swap(Node&other);
	};

	class Container
	{
	private:

	public:
        Node           		root_node;
		String				encoding;							//!< Character encoding

		/**/				Container()						{root_node.name = "xml"; encoding = "UTF-8"; }
		/**/				Container(const String&source)	{root_node.name = "xml"; encoding = "UTF-8"; loadFromFile(source);}
		void				clear();													//!< Clears all local data
		inline void			Clear()	{clear();}
		Node*				find(const String&path);				//!< Returns the first matching node (if any) @param path Path string in the form 'node/subnode/.../subnodeN' @return Pointer to the matching node or NULL if no such could be found
		inline Node*		Find(const String&path)	{return find(path);}
		const Node*			find(const String&path)		const;	//!< @copydoc find()
		inline const Node*	Find(const String&path)		const	{return find(path);}
		Node&				create(const String&path, const String&inner_content="");
		inline Node&		Create(const String&path, const String&innerContent="")	{return create(path,innerContent);}

		/**
		@brief Attempts to load the local XML configuration from the specified char field
				
		The specified char field must include the terminating 0 and be mutable. The array length will not be altered but dividing zeroes will be inserted as necessary
		@param field Char array as specified
		*/
		void				loadFromCharArray(ArrayData<char>&field);
		inline void			LoadFromCharArray(ArrayData<char>&field)	{loadFromCharArray(field);}
		/**
		@brief Attempts to load the local XML configuration from the specified file
		@param filename Name of the file to load
		@param content_out Optional string to store unprocesses file content in. Pass valid string pointer or NULL if no content exporting should be performed
		*/
        void                loadFromFile(const String&filename, String*content_out=NULL);
		inline void			LoadFromFile(const String&filename, String*contentOut=NULL)	{return loadFromFile(filename,contentOut);}
		/**
		@brief Attempts to load the local XML configuration from the specified content string
		@param content String to parse
		*/
		void				loadFromString(const String&content);
		inline void			LoadFromString(const String&content)	{loadFromString(content);}
		void                saveToFile(const String&filename, export_style_t style=Nice);
		inline void			SaveToFile(const String&filename, export_style_t style=Nice)	{saveToFile(filename,style);}
		void				saveToStringBuffer(StringBuffer&target, export_style_t style=Nice);
		inline void			SaveToStringBuffer(StringBuffer&target, export_style_t style=Nice)	{saveToStringBuffer(target,style);}
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
			ScannerRule*		map(const String&name, bool want_content=true,pNodeCallback on_enter=NULL, pNodeCallback on_exit=NULL);
			ScannerRule*		map(ScannerRule*node);
			bool				unMap(const String&name);
			bool				unMap(ScannerRule*node);
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
			void				setExcept(pNodeCallback callback_on_except=NULL);
			bool				scan(const String&filename);
			const String&		errorStr()						const;
			const String&		getError()						const;
			bool				errorIsFileNotFound()			const;
	};
}

#endif
