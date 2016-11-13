#ifndef progress_logTplH
#define progress_logTplH

/******************************************************************

progress logger unit.

******************************************************************/


template <class C>
void Progress::Log::execute(const String&name, Visualizer&visualizer, void (*func_)(C&), C& param)
{
    begin(name,visualizer);
    func_(param);
}










#endif

