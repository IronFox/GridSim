#ifndef progress_logTplH
#define progress_logTplH

/******************************************************************

progress logger unit.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/


template <class C>
void Progress::Log::execute(const String&name, Visualizer&visualizer, void (*func_)(C&), C& param)
{
    begin(name,visualizer);
    func_(param);
}










#endif

