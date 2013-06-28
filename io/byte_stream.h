#ifndef byte_streamH
#define byte_streamH

#include "../interface/read_stream.h"
#include "../interface/write_stream.h"

class ByteStream : public IReadStream, public IWriteStream
{
protected:

                    char		*begin,
								*current,
								*end;
                    bool        local;
       
                    void        pushData(const void*pntr, size_t size);
                    bool        getData(void*pntr, size_t size);
public:

                                ByteStream(size_t size=1024);
       virtual                 ~ByteStream();
       
                    void        reset();
                    void        resize(size_t len);
				template <typename T>
					void		Assign(Array<T>&array);
                    void        Assign(void*pntr, size_t size);
					char*		pointer();
					const char*	pointer()	const;
                    const void* data()	const;
                    size_t		size()	const;
					size_t		fillLevel()	const;
                    void        pushZero(size_t count);
template <class C>  bool        stream(C&out);
template <class C>  bool        stream(C*array, count_t count);
template <class C>  void        push(const C&object);
template <class C>  void        push(const C*array, count_t count);
template <class C>  ByteStream&operator<<(const C&object);
template <class C>  bool        operator>>(C&object);

	virtual	bool				Write(const void*data, serial_size_t size)	override {pushData(data,size);return true;}
	virtual	bool				Read(void*target_data, serial_size_t size)	override {return getData(target_data,size);}
	virtual serial_size_t		GetRemainingBytes() const override {return end - current;}
};


#include "byte_stream.tpl.h"
#endif
