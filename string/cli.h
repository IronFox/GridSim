#ifndef cliH
#define cliH


#include "../container/string_list.h"
//#include <list/hashtable.h>
#include "../global_string.h"
#include <stdio.h>
#include "../math/vector.h"
#include "../general/ref.h"
#include <functional>
#include <memory>

//using namespace std;

	template <>
	inline bool convert(const char*cstr, String&target)
	{
		target = cstr;
		return true;
	}



namespace CLI	//! Command line interpretor
{


	namespace detail
	{
		void	makeValidName(String&name);
	}




	enum eVariableProtection	//! Variable protection flags
	{
		NoProtection		= 0x0,	//!< Variable is not protected and may be changed or unset by the user at will
		EraseProtection		= 0x1,	//!< Variable is protected against deletion. Unless WriteProtection is set the variable may still be changed though
		WriteProtection		= 0x2,	//!< Variable is protected against changes. Unless EraseProtection is set the variable may still be unset though
		FullProtection		= (EraseProtection|WriteProtection)	//!< Full protection. The variable may be queried but not changed or unset
	};

	/*!
		\brief Type of complemention requested by a command
		
		Actual command-completion (if any) has to be implemented by the respective interface. Interpretor provides completion functionality
		for folders and variables but not files.
		A CLI command may request exactly one type of completion for all its parameters
	*/
	enum eCommandCompletion
	{
		NoCompletion,			//!< No completion
		FolderCompletion,		//!< CLI::Folder completion
		VariableCompletion,		//!< CLI::Variable completion
		FileCompletion,			//!< File system completion
		CommandCompletion		//!< CLI::Command completion
	};


	/*!
	\brief Abstract virtual CLI variable container
		
	A Variable instance describes a variable in the CLI interpretor tree. A variable derivative class must provide methods
	to query or set the variable contents via strings.
	*/
	class Variable : public std::enable_shared_from_this<Variable>
	{
	public:
		const String		type,		//!< Type string (i.e. 'int')
							name;		//!< Variable name. Must not be empty
		UINT32				protection;	//!< Variable protection flags. May be any combination of CLI::eVariableProtection enumeration values
		const count_t		components;	//!< Number of components this variable provides. Contains 1 for non-array variables and a fixed number for array variables (i.e. 3 for a 3d vector).
			
			
		/**/				Variable(const String&name, const String&type, count_t components=1, UINT32 protection=NoProtection);
		virtual				~Variable()	{};
		virtual	String		ToString()			const=0;		//!< Virtual abstract out conversion method. Converts the local variable value(s) to a string representation. The returned string should be a valid parameter for set().
		virtual	bool		Set(const String&value)=0;			//!< Virtual abstract in conversion method. Converts the specified string value to the local variable value(s). @param value String containing the new value(s) \return true if the variable value could be updated, false otherwise
		virtual bool		Set(const String&component, const String&value)	{return false;};	//!< Component-wise virtual conversion method. The method sets one or more components of the local variable value(s) as described by \b component to \b value @param component Description of one or more components to set @param value String containing the new value(s) \return true if the variable value(s) could be updated, false otherwise
	};

	typedef std::shared_ptr<Variable>	PVariable;


	template <typename T>
		class SimpleVariable:public Variable	//! Standard CLI simple type variable
		{
		public:
			T				content;	//!< Scalar content
				
			/**/			SimpleVariable(const String&name, const String&typeName, const T&content_=(T)0,unsigned protection=NoProtection):Variable(name,typeName,1,protection),content(content_)	{}
			virtual String	ToString()		const	override	{return content;}
			virtual	bool	Set(const String&value)	override	{return convert(value.c_str(),content);}
			virtual	bool	Set(const String&component, const String&value)	override	{return Set(value);}
		};
	
	class IntVariable:public SimpleVariable<int>	//! Standard CLI int variable
	{
	public:
		/**/				IntVariable(const String&name, int content=0, unsigned protection=NoProtection):SimpleVariable<int>(name,"Integer",content,protection){}
	};

	class FloatVariable:public SimpleVariable<float>	//! Standard CLI float variable
	{
	public:
		/**/				FloatVariable(const String&name, float content=0, unsigned protection=NoProtection):SimpleVariable<float>(name,"Float",content,protection){}
		/**/				FloatVariable(const String&name, const String&type, float content=0, unsigned protection=NoProtection):SimpleVariable<float>(name,type,content,protection){}
	};
	
	class StringVariable:public SimpleVariable<String>	//! Standard CLI string variable
	{
	public:
		/**/				StringVariable(const String&name, const String&content="", unsigned protection=NoProtection):SimpleVariable<String>(name,"String",content,protection){}
		virtual String		ToString() const override	{return "'"+content+"'";}
	};
	
	class BoolVariable:public SimpleVariable<bool>	//! Standard CLI bool variable
	{
	public:
		/**/				BoolVariable(const String&name, bool content=false, unsigned protection=NoProtection):SimpleVariable<bool>(name,"Boolean",content,protection){}
		virtual String		ToString()	const	override {return content?"true":"false";}
	};


	/*!
	\brief Standardized component wise vector variable setter
		
	Sets one or more components of the specified \a content array. Each character of the described \a component string represents one component to set.<br />
	Supported components are (sorted by element in \a content ):<ol>
	<li>r,R,x,X,u,U</li>
	<li>g,G,y,Y,v,V</li>
	<li>b,B,z,Z</li>
	<li>a,A,w,W</li></ol>
		
	@param content Variable field array consisting of at least \b Components elements
	@param component String description of the components to set. Each character describes on component (i.e. "rgba" or "xzy" or "uv")
	@param value String containing one or more values to set (see setVectorVariableContent(T*,const String&) for details).
	\return true if all specifications were correct, false otherwise. Some components of \b content may have been set even if the function returns false
	*/
	template <typename T, count_t Components>
		bool				SetVectorVariableContent(T*content, const String&component, const String&value);

	/*!
		\brief Standardized vector variable setter
		
		Sets components of the specified \a content array. Values of \a content are only set as far as avaible. The provided \a value string may be of various array like forms.<br />
		Examples:<ul>
		<li>42</li>
		<li>1.8;2.1 ,3,4</li>
		<li>(10,2)</li>
		<li>[1,222;3; 4]</li>
		<li>{13;12,3,4 ,5}</li>
		<li>some ignored string {1,2,3.12}</li>
		<li>pi(3.1415)</li></ul>
		
		@param content Variable field array consisting of at least \b Components elements
		@param value String containing one or more values to set (in the above stated manner)
		\return true if the assignment was successful, false otherwise. Some components of \b content may have been set even if the function returns false
	*/
	template <typename T, count_t Components>
		bool				SetVectorVariableContent(T*content, const String&value);
		
	
	template <count_t Components>
		class VectorVariable:public Variable	//! Standard CLI fixed size float vector variable (uses setVectorVariableContent())
		{
		public:
			float				content[Components];	//!< Field of variable values

			/**/				VectorVariable(const String&name, float content[Components]=NULL, unsigned protection=NoProtection);
			virtual String		ToString() const override;
			virtual	bool		Set(const String&value) override;
			virtual	bool		Set(const String&component, const String&value) override;
		};


	template <count_t Components>
		class DoubleVectorVariable:public Variable	//! Standard CLI fixed size double vector variable (uses setVectorVariableContent())
		{
		public:
			double				content[Components];	//!< Field of variable values

			/**/				DoubleVectorVariable(const String&name, double content[Components]=NULL, unsigned protection=NoProtection);
			virtual String		ToString() const override;
			virtual	bool		Set(const String&value) override;
			virtual	bool		Set(const String&component, const String&value) override;
		};


	class Attachable	//! Base class for Folder and Command attachments
	{
	public:
		virtual					~Attachable()	{};
	};

	typedef std::shared_ptr<Attachable>	PAttachment;

	class Command;
	typedef std::shared_ptr<Command>	PCommand;
	
	class Command : public std::enable_shared_from_this<Command>	//! CLI command container
	{
	public:
		typedef Attachable			Attachable;
			
		class EventObject	//! Abstract virtual event handler object 
		{
		public:
		virtual	void		handleCommandInput(const PCommand&command, const StringList&arguments, const Tokenizer::Config&config)	//!< Abstract virtual event handler method invoked when this command is executed @param command Executing command @param arguments Const reference to a string list containing the execution arguments. The first entry contains the name of the executed command @param config Tokenizer config used to determine quotes
							{}
		};

		typedef void (*full_func_t)(const PCommand&,const StringList&arguments, const Tokenizer::Config&config);
		typedef void (*func0_t)();
		typedef void (*func1_t)(const String&);
		typedef void (*func2_t)(const String&, const String&);
		typedef void (*func3_t)(const String&, const String&, const String&);
		typedef void (*func4_t)(const String&, const String&, const String&, const String&);
		typedef void (*func5_t)(const String&, const String&, const String&, const String&, const String&);
		typedef void (*func6_t)(const String&, const String&, const String&, const String&, const String&, const String&);

		typedef std::function<void(const PCommand&,const StringList&arguments, const Tokenizer::Config&config)>					full_function_t;
		typedef std::function<void()>																							function0_t;
		typedef std::function<void(const String&)>																				function1_t;
		typedef std::function<void(const String&, const String&)>																function2_t;
		typedef std::function<void(const String&, const String&, const String&)>												function3_t;
		typedef std::function<void(const String&, const String&, const String&, const String&)>									function4_t;
		typedef std::function<void(const String&, const String&, const String&, const String&, const String&)>					function5_t;
		typedef std::function<void(const String&, const String&, const String&, const String&, const String&, const String&)>	function6_t;

			
		full_function_t			callback;
		String					name;	//!< Command name
		String					description;	//!< Command description
		eCommandCompletion		completion;		//!< Type of command completion requested by this command
		PAttachment				attachment;		//!< Command attachment
			
		/**/					Command();	//!< Constructor for virtual commands
		/**/					Command(const String&name, const String&description, const full_function_t&, eCommandCompletion completion);
		/**/					Command(const String&name, const String&description, const function0_t&, eCommandCompletion completion);
		/**/					Command(const String&name, const String&description, const function1_t&, eCommandCompletion completion);
		/**/					Command(const String&name, const String&description, const function2_t&, eCommandCompletion completion);
		/**/					Command(const String&name, const String&description, const function3_t&, eCommandCompletion completion);
		/**/					Command(const String&name, const String&description, const function4_t&, eCommandCompletion completion);
		/**/					Command(const String&name, const String&description, const function5_t&, eCommandCompletion completion);
		/**/					Command(const String&name, const String&description, const function6_t&, eCommandCompletion completion);
		/**/					Command(const String&name, const String&description, const full_func_t&, eCommandCompletion completion);
		/**/					Command(const String&name, const String&description, const func0_t&, eCommandCompletion completion);
		/**/					Command(const String&name, const String&description, const func1_t&, eCommandCompletion completion);
		/**/					Command(const String&name, const String&description, const func2_t&, eCommandCompletion completion);
		/**/					Command(const String&name, const String&description, const func3_t&, eCommandCompletion completion);
		/**/					Command(const String&name, const String&description, const func4_t&, eCommandCompletion completion);
		/**/					Command(const String&name, const String&description, const func5_t&, eCommandCompletion completion);
		/**/					Command(const String&name, const String&description, const func6_t&, eCommandCompletion completion);
		virtual	void			Invoke(const StringList&arguments, const Tokenizer::Config&dequote_config);
	private:
			void					construct(const String&name, const String&description, eCommandCompletion completion);
	};



	class Folder;
	typedef std::shared_ptr<Folder>	PFolder;
	typedef std::weak_ptr<Folder>	WFolder;

	template <typename T>
		class ItemTable : private Buffer<std::shared_ptr<T> >
		{
		public:
			typedef std::shared_ptr<T>	Type;
		private:
			typedef Buffer<Type>	Super;
			StringTable<index_t>		table;
		public:
			typedef typename Super::const_iterator	const_iterator;

			Super::count;
			Super::length;
			Super::size;


			const_iterator			begin()	const	{return Super::begin();}
			const_iterator			end() const		{return Super::end();}
			const Type&				operator[](index_t element)	const {return Super::operator[](element);}
			
			bool					IsSet(const String&name) const	{return table.isSet(name);}
			void					Unset(const String&name)		{index_t at; if (table.queryAndUnSet(name,at)) Super::erase(at);}
			Type					InsertNew(const String&name, T*newObject)	{index_t at = Super::count(); Super::append(Type(newObject)); table.set(name,at); return Super::last();}
			void					Set(const String&name, const Type&object)	{index_t at = Super::count(); Super::append(object); table.set(name,at);}
			void					Insert(const String&name, const Type&object)	{Set(name,object);}
			Type					Query(const String&name) const	{index_t at; if (table.query(name,at)) return Super::at(at); return Type();}
			void					clear()	{Super::clear(); table.clear();}
		};


	/*!
	\brief Folder of the CLI interpretor tree
		
	Folder stores sub folder, variables and commands as defined in this folder.
	*/
	class Folder : public std::enable_shared_from_this<Folder>
	{
	public:
		typedef Attachable			Attachable;
		typedef Command				Command;
		typedef Variable			Variable;
			
		PAttachment					attachment;

		UINT32						customFlags;

		ItemTable<Variable>			variables;		//!< Variables stored in this folder
		ItemTable<Command>			commands;		//!< Commands in this folder
		ItemTable<Folder>			folders;		//!< Sub folders
		const WFolder				parent;	//!< Parent folder or NULL if this folder has no parent
		const String				name;			//!< Folder name
			
		/**/						Folder(const PFolder&,const String&);
		count_t						CountCommands()					const;		//!< Recursivly counts all commands
		count_t						CountVariables()				const;		//!< Recursivly counts all variables
		String						GetPath()						const;		//!< Retrieves the full folder path to this folder (path segments separated by '/')
		String						GetPath(unsigned max_depth)		const;		//!< Retrieves a folder path with up to \b max_depth segments to this folder (path segments separated by '/') @param max_depth Maximum number of path segments in the returned path. If the actual path would be longer than '(...)/' preceeds the returned segment string \return (potentially truncated) path string to the local folder (i.e. "(...)/folder22/folder23/folder24" for \b max_depth = 3)
		bool						IsChildOf(const PFolder&other)	const;		//!< Checks if the local folder is some child of \b other

		PVariable					SetVariable(const PVariable&v);	//!< Attempts to declare the specified variable. If the operation fails, an exception of type Program::UniquenessViolation
	};
	

	class Interpretor	//! Command line interpretor
	{
		String						error;				//!< Contains a description of the last occured error (if any)
		struct
		{
			Tokenizer::Config		pathConfig,		//!< Config used to split the first segment of an input line into path segments
									segmentConfig,		//!< Config used to split a line into space separated segments
									lineConfig;		//!< Config used to split a string into lines
			StringList				pathSegments,		//!< Internal storage for the segments of a path
									cmdSegments;		//!< Internal storage for the segments of a command line
			PVariable				variable;
			PCommand				command;
			PFolder					folder;
		}							lookup;
		PFolder						focused;			//!< Points to the currently focused folder (not NULL)
		PCommand					executing;			//!< Currently executing command
		PFolder						executingFolder;	//!< Parent folder of the currently executing command
		Buffer<PFolder,4>			focusStack;
		PFolder						root;
			
		bool						_ParseLine(const char*line,bool allowGlobalCommands);	//!< Parses an input line
		PFolder						_Resolve(bool fromRoot);	//!< Resolves the folder described by the first n-1 segments of the current content of Interpretor::segments @param from_root Specifies that the folder lookup should start from the parser root folder rather than the currently focused folder \return Pointer to the respective folder or NULL if no such folder could be found
		PFolder						_ResolveFull(bool fromRoot);	//!< Resolves the folder described by the current content of Interpretor::segments @param from_root Specifies that the folder lookup should start from the parser root folder rather than the currently focused folder \return Pointer to the respective folder or NULL if no such folder could be found
			
		bool						_EntryLookup(const String&name, bool mayExist);	//!< Attempts to locate an element by the specified name (and path). @param name Path to look for @param may_exist Set false to let the function fail if an element of that name exists @return true on success, false otherwise
	public:
		//Sorted<List::Vector<Command>,NameSort>	global_commands;	//!< List of commands that work in all contexts
		ItemTable<Command>			globalCommands;
		std::function<void(PVariable)>	onVariableCall;	//!< Callback function invoked whenever a variable is executed (NULL by default)

		/**/						Interpretor();
		template <typename F>
			PCommand				DefineCommand(const String& def, const F&f, eCommandCompletion completion=NoCompletion);	//!< Defines a new command (0 argument pointer command) in the active folder @param def Name of the command to create(may include folder names) @param method Pointer to the handler function @param completion Command completion
		PCommand					DefineCommand(const String& def, const PCommand&cmd);	//!< Inserts a new command in the specified location. The method fails if a command already exists in the specified location. The method's name and description will be overwritten depending on the content of the \b def variable @param def Name of the command to create(may include folder names)  @param cmd New command object. The object will be managed by this structure and must not be deleted \return Pointer to the inserted command object if no command of that name existed, NULL otherwise
		template <typename F>
			PCommand				DefineGlobalCommand(const String& def, const F&f, eCommandCompletion completion=NoCompletion);	//!< Defines a new command (0 argument pointer command) in the active folder @param def Name of the command to create(may include folder names) @param method Pointer to the handler function @param completion Command completion
		PCommand					DefineGlobalCommand(const String& def, const PCommand&cmd);	//!< Inserts a new command in the specified location. The method fails if a command already exists in the specified location. The method's name and description will be overwritten depending on the content of the \b def variable @param def Name of the command to create(may include folder names)  @param cmd New command object. The object will be managed by this structure and must not be deleted \return Pointer to the inserted command object if no command of that name existed, NULL otherwise
			
			
		PVariable					SetString(const String& path, unsigned Protection);					//!< Sets a new empty string variable (StringVariable). @param path path of the string variable to set (may contain folders). No changes occur if a variable of the specified path already exists @param Protection Protection config of the new variable. May be any combination of CLI::eVariableProtection enumeration values @return Pointer to a new abstract variable on success, NULL otherwise. Use getError() to retrieve an error description in this case
		PVariable					SetString(const String& path, const String& value, unsigned Protection, bool allow_overwrite=false);	//!< Sets a new string variable to the specified value. @param path path of the variable to set (may contains paths). No changes occur if a variable of the specified path already exists and \b allow_overwrite is set false @param Protection Protection config of the new variable. May be any combination of CLI::eVariableProtection enumeration values @param allow_overwrite If a variable of the specified path already exists and \b allow_overwrite is set true then the existing variable will be updated to the specified value (its type remaind unchanged though) @return Pointer to a new abstract variable on success, NULL otherwise. Use getError() to retrieve an error description in this case
		PVariable					SetInt(const String& path, unsigned Protection);						//!< Sets a new empty integer variable (IntVariable). @param path path of the int variable to set (may contain folders). No changes occur if a variable of the specified path already exists @param Protection Protection config of the new variable. May be any combination of CLI::eVariableProtection enumeration values @return Pointer to a new abstract variable on success, NULL otherwise. Use getError() to retrieve an error description in this case
		PVariable					SetInt(const String& path, int value, unsigned Protection);			//!< Sets a new integer variable (IntVariable) to the specified value. @param path path of the int variable to set (may contain folders). No changes occur if a variable of the specified path already exists @param value Value of the new integer variable object @param Protection Protection config of the new variable. May be any combination of CLI::eVariableProtection enumeration values @return Pointer to a new abstract variable on success, NULL otherwise. Use getError() to retrieve an error description in this case
		PVariable					SetBool(const String& path, unsigned Protection);						//!< Sets a new boolean variable (BoolVariable) @param path path of the int variable to set (may contain folders). No changes occur if a variable of the specified path already exists @param Protection Protection config of the new variable. May be any combination of CLI::eVariableProtection enumeration values @return Pointer to a new abstract variable on success, NULL otherwise. Use getError() to retrieve an error description in this case
		PVariable					SetBool(const String& path, bool value, unsigned Protection);			//!< Sets a new boolean variable (BoolVariable) to the specified value. @param path path of the int variable to set (may contain folders). No changes occur if a variable of the specified path already exists @param value Value of the new bool variable object @param Protection Protection config of the new variable. May be any combination of CLI::eVariableProtection enumeration values @return Pointer to a new abstract variable on success, NULL otherwise. Use getError() to retrieve an error description in this case
		PVariable					SetFloat(const String& path, unsigned Protection);						//!< Sets a new empty float variable (FloatVariable). @param path path of the float variable to set (may contain folders). No changes occur if a variable of the specified path already exists @param Protection Protection config of the new variable. May be any combination of CLI::eVariableProtection enumeration values @return Pointer to a new abstract variable on success, NULL otherwise. Use getError() to retrieve an error description in this case
		PVariable					SetFloat(const String& path, float value, unsigned Protection);		//!< Sets a new float variable (FloatVariable) to the specified value. @param path path of the float variable to set (may contain folders). No changes occur if a variable of the specified path already exists @param value Value of the new float variable object @param Protection Protection config of the new variable. May be any combination of CLI::eVariableProtection enumeration values @return Pointer to a new abstract variable on success, NULL otherwise. Use getError() to retrieve an error description in this case
		PVariable					SetVector3f(const String& path, unsigned Protection);					//!< Sets a new empty float vector variable (VectorVariable). @param path path of the float vector variable to set (may contain folders). No changes occur if a variable of the specified path already exists @param Protection Protection config of the new variable. May be any combination of CLI::eVariableProtection enumeration values @return Pointer to a new abstract variable on success, NULL otherwise. Use getError() to retrieve an error description in this case
		PVariable					SetVector3f(const String& path, float value[3], unsigned Protection);	//!< Sets a new float vector variable (VectorVariable) to the specified value. @param path path of the float vector variable to set (may contain folders). No changes occur if a variable of the specified path already exists @param value Value of the new float vector variable object @param Protection Protection config of the new variable. May be any combination of CLI::eVariableProtection enumeration values @return Pointer to a new abstract variable on success, NULL otherwise. Use getError() to retrieve an error description in this case
		PVariable					SetVector4f(const String& path, unsigned Protection);					//!< Sets a new empty float vector variable (VectorVariable). @param path path of the float vector variable to set (may contain folders). No changes occur if a variable of the specified path already exists @param Protection Protection config of the new variable. May be any combination of CLI::eVariableProtection enumeration values @return Pointer to a new abstract variable on success, NULL otherwise. Use getError() to retrieve an error description in this case
		PVariable					SetVector4f(const String& path, float value[4], unsigned Protection);	//!< Sets a new float vector variable (VectorVariable) to the specified value. @param path path of the float vector variable to set (may contain folders). No changes occur if a variable of the specified path already exists @param value Value of the new float vector variable object @param Protection Protection config of the new variable. May be any combination of CLI::eVariableProtection enumeration values @return Pointer to a new abstract variable on success, NULL otherwise. Use getError() to retrieve an error description in this case
		PVariable					SetVector3d(const String& path, unsigned Protection);					//!< Sets a new empty double vector variable (DoubleVectorVariable). @param path path of the double vector variable to set (may contain folders). No changes occur if a variable of the specified path already exists @param Protection Protection config of the new variable. May be any combination of CLI::eVariableProtection enumeration values @return Pointer to a new abstract variable on success, NULL otherwise. Use getError() to retrieve an error description in this case
		PVariable					SetVector3d(const String& path, double value[3], unsigned Protection);	//!< Sets a new double vector variable (DoubleVectorVariable) to the specified value. @param path path of the double vector variable to set (may contain folders). No changes occur if a variable of the specified path already exists @param value Value of the new double vector variable object @param Protection Protection config of the new variable. May be any combination of CLI::eVariableProtection enumeration values @return Pointer to a new abstract variable on success, NULL otherwise. Use getError() to retrieve an error description in this case
		PVariable					SetVector4d(const String& path, unsigned Protection);					//!< Sets a new empty double vector variable (DoubleVectorVariable). @param path path of the double vector variable to set (may contain folders). No changes occur if a variable of the specified path already exists @param Protection Protection config of the new variable. May be any combination of CLI::eVariableProtection enumeration values @return Pointer to a new abstract variable on success, NULL otherwise. Use getError() to retrieve an error description in this case
		PVariable					SetVector4d(const String& path, double value[4], unsigned Protection);	//!< Sets a new double vector variable (DoubleVectorVariable) to the specified value. @param path path of the double vector variable to set (may contain folders). No changes occur if a variable of the specified path already exists @param value Value of the new double vector variable object @param Protection Protection config of the new variable. May be any combination of CLI::eVariableProtection enumeration values @return Pointer to a new abstract variable on success, NULL otherwise. Use getError() to retrieve an error description in this case
		PVariable					SetVariable(const PVariable&v);							//!< Sets a custom type variable in active folder @param v Variable to set \return @b v if no variable of the specified path existed and the variable could be inserted, NULL otherwise
		/*!
		\brief Attempts to set the content of a variable
											
		If a variable of the specified path exists then set() will attempt to update its value. The method fails if the matching variable is write-protected.
		Otherwise a new variable will be created depending on the content of \a value (float vector, float, int or string). The new variable is not protected.
											
		@param path Path of the new or existing variable
		@param value New value to assign
		\return Pointer to the newly created or existing variable if the value assignment was successful, NULL otherwise. Use getError() to retrieve an error description if NULL is returned.
		*/
		PVariable 					Set(const String& path, const String& value);
		/*!
		\brief Erases the folder matching \b path.
											
		The method fails silently if no such folder exists. If the erased folder is the active folder or a descendant of it then the active folder will be set to the immediate parent of the erased folder.
		@param path Path to the folder to erase
		*/
		void						EraseFolder(const String& path);
		void						EraseFolder(const PFolder&folder);	//!< Identical to eraseFolder(const String&) for an already found folder
		/*!
		\brief Unsets a variable
											
		This method attempts to unset the specified variable.
											
		@param path Path to the variable to unset
		\return true if the specified variable could be found and unset, false otherwise. The method fails if the found variable is protected against deletion.
		*/
		bool						Unset(const String& path);
		void						UnsetIgnoreProtection(const String& path);	//!< Similar to unset(const String&) except that any protection of the target variable will be ignored
		/*!
		\brief Unsets an object variable
											
		This method attempts to unset the specified variable.
											
		@param folder Parent folder of the specified variable. 
		@param var Variable to unset. The method fails if \b var is no member of \b folder
		@param ignoreProtection Set true to ignore variable protection
		\return true if the specified variable could be found and unset, false otherwise. The method fails if the found variable is protected against deletion and \b ignore_protection set false
		*/
		bool						Unset(const PFolder&folder, const PVariable&var, bool ignoreProtection=false);
		PCommand					GetExecutingCommand()		const;		//!< Retrieves the currently executing command. The result is NULL if no command is currently executing
		PFolder						GetExecutionContext()		const;		//!< Retrieves the parent folder of the currently executing command. The result is NULL if no command is currently executing
		const StringList&			GetExecutionSegments()		const;		//!< Returns the segments passed on to the currently executing command. Can be used to implement variable numbers of arguments. The first entry in the returned string list is the executing command, all following elements the respective parameters
		unsigned					GetAsUnsigned(const String& path, unsigned exception = 0);	//!< Attempts to retrieve the specified variable as an unsigned int @param path Variable path @param exception Value to return if the specified variable could not be found or its value not converted to an unsigned int
		int							GetAsInt(const String& path, int exception = 0);				//!< Attempts to retrieve the specified variable as an int @param path Variable path @param exception Value to return if the specified variable could not be found or its value not converted to an int
		float						GetAsFloat(const String& path, float exception = 0);				//!< Attempts to retrieve the specified variable as a float @param path Variable path @param exception Value to return if the specified variable could not be found or its value not converted to a float
		Key::Name					GetAsKey(const String& path, Key::Name exception = (Key::Name)0);		//!< Attempts to retrieve the specified variable as a key name @param path Variable path @param exception Value to return if the specified variable could not be found or its value not converted to a key name
		bool						GetAsBool(const String& path, bool exception = false);			//!< Attempts to retrieve the specified variable as a boolean @param path Variable path @param exception Value to return if the specified variable could not be found or its value not converted to a bool
		const String&				GetAsString(const String& path, const String& exception = "");					//!< Attempts to retrieve the specified variable as a string. All variables inherently provide a ToString() method so the only reason for this method to fail is if the requested variable does not exist @param path Variable path @param exception Value to return if the specified variable could not be found
		const String&				GetErrorStr()	const;						//!< Retrieves an error description of the last occured error
		const String&				GetError()		const;						//!< Identical to GetErrorStr()
		String						Complete(const String&line, StringList&out);			//!< Attempts to complete an incomplete line towards a command or folder. @param line Line to complete @param out Reference list to store all possibilities in \return Longest common string among all possibilities or a complete command line if there is just one possibility. The returned string is empty if no matching possibility was found.
		String						CompleteFolders(const String&line, StringList&out);		//!< Attempts to complete an incomplete line towards a folder. @param line Line to complete @param out Reference list to store all possibilities in \return Longest common string among all possibilities or a complete command line if there is just one possibility. The returned string is empty if no matching possibility was found.
		String						CompleteVariables(const String&line, StringList&out);		//!< Attempts to complete an incomplete line towards a variable or folder. @param line Line to complete @param out Reference list to store all possibilities in \return Longest common string among all possibilities or a complete command line if there is just one possibility. The returned string is empty if no matching possibility was found.
		PCommand 					Find(const String&path,PFolder*folder_out=NULL);	//!< Attempts to find a command matching \b path @param path Path of the command to look for \return Pointer to a matching command or NULL if no such command exists
		PCommand 					Find(const char*path,PFolder*folder_out=NULL);		//!< Identical to find(const String&)
		PVariable 					FindVar(const String&path,PFolder*folder_out=NULL);	//!< Attempts to find a variable matching \b path @param path Path of the variable to look for \return Pointer to a matching variable or NULL if no such variable exists
		PVariable					FindVar(const char*path,PFolder*folder_out=NULL);		//!< Identical to FindVar(const String&)
		PFolder						FindFolder(const String&path);	//!< Attempts to find a folder matching \b path @param path Path of the folder to look for \return Pointer to a matching folder or NULL if no such folder exists
		const PFolder&				GetRoot() const	{return root;}
		/*!
		\brief Interprets one or more lines
				
		Interpret() attempts to parse and execute the specified line(s).
		Commands should be of the form '[command] [parameter0] ... [parameterN]'.<br />
		[command] like all paths may be preceeded by paths relative to the currently active folder (i.e. 'myFolder/doSomething')
		or absolute paths with a slash at the beginning (i.e. '/myTopmostFolder/mySubFolder/mySubSubFolder/myCommand').<br />
		Commands are case sensitive! 'doSomething' is not the same command as 'DoSomething'.<br />
		Existing variables may also be 'executed' if Interpretor::exec_on_variable_call is not NULL.<br />
		[parameter] may be a single word or a group of words grouped by quotation marks (i.e. "hello world" or "string \"in a\" string").<br />
		Note that backslashes need not be escaped (i.e. "\" is the same as "\\" but in order to insert two backslashes inside quotation marks you need to pass "\\\\")
				
		@param line Line(s) to parse. Multiple lines may be separated by ';'. Separators in strings or parenthesis are ignored.
		@param allow_global_commands Set false to disallow the execution of global commands
		\return true if all commands could be executed, false otherwise. Use getError() to retrieve an error description if Interpret() returned false
		*/
		bool						Interpret(const String&line, bool allow_global_commands=true);
		bool						Interpret(const char*line, bool allow_global_commands=true);		//!< Identical to Interpret(const String&)
		bool						Parse(const String&line, bool allow_global_commands=true);		//!< Identical to Interpret(const String&)
		bool						Parse(const char*line, bool allow_global_commands=true);		//!< Identical to Interpret(const String&)
		bool						MoveFocus(const String&path);	//!< Attempts to move the active folder to the specified path @param path Path pointing to a folder \return true on success, false otherwise. The active folder stays the same if the method fails.
		PFolder						EnterNewFolder(const String&path, bool enter_existing=false);		//!< Attempts to create a new folder and enter it. All but the last segment of \b path must already exist @param path Path of the new folder \return Pointer to the newly or already existing folder on success, NULL otherwise. The active folder stays the same of the method fails, unless @a enter_existing is specified.
		bool						ExitFolder();			//!< Attempts to drop out of the currently active folder to its immediate parent \return true on success
		bool						ExitFocus();			//!< Identical to exitFolder()
		void						ResetFocus();			//!< Resets the active folder to the root folder
		void						SetFocus(const PFolder&folder);		//!< Sets the active folder to \b folder
		void						PushFocus();					//!< Pushes the active focus to the focus stack, increasing stack size by one
		void						PushAndReplaceFocus(const PFolder&new_focus);	//!< Pushes the active focus to the focus stack, increasing stack size by one. Directly enters the specified new focus
		void						PopFocus();						//!< Restores the active focus from the focus stack, decreasing stack size by one
		PFolder						GetFocus() const;						//!< Retrieves the current active folder
		String						GetContext(unsigned depth=3)	const;	//!< Retrieves a string representing the current context up to the specified folder depth

			
		const Tokenizer::Config&	GetSegmentizerConfig()	const;	//!< Retrieves the configuration of the command segmentizer (used to divide a single command line into command and parameter segments)
			
	};

	class Script:public StringList	//! Command line collection (script)
	{
	public:
			String		name;	//!< Script name
						Script(const String&Name);
						Script(const String&Name, const String&filename);
			bool		load(const String&filename);		//!< Loads script content from the specified text file
			void		loadFromStream(char*line, size_t len);	//!< Loads script content from a field of characters
	};

	class ScriptList:public List::Vector<Script>	//! List of scripts
	{
	public:
			bool		load(const String&filename, const String&alias);	//!< Attempts to load a new script to the end of the script list
			bool		erase(const String&alias);							//!< Erases the specified script
			Script*	find(const String&alias);							//!< Attempts to locate a loaded script
			bool		execute(const String&alias,Interpretor*parser);	//!< Executes a script
	};
	
	#include "cli.tpl.h"
}

#endif
