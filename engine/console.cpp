#include "../global_root.h"
#include "console.h"

/******************************************************************

Language/Font-depended console.

******************************************************************/

namespace Engine
{

	static LogFile		console_log("console.log",true);
	#define CONSOLE_LOG(msg) console_log << __func__<<"("<<__LINE__<<"): "<<msg<<nl;


void Console::clearHistory()
{
	lines.clear();
}

void Console::closeEvent()
{
	close();
}

void Console::fetchInput()
{
	CONSOLE_LOG("retrieving input");
    const char*input = keyboard.GetInput();
	CONSOLE_LOG("input is '"<<input<<"'");
    if (keyboard.GetInputLen())
    {
		CONSOLE_LOG("erasing history occurances");
        inputs.eraseOccurrences(input);
		CONSOLE_LOG("appending to input histpry");
        inputs.add(input);
		CONSOLE_LOG("history modified");
    }
    if (parser)
	{
		CONSOLE_LOG("sending to parser");
        parser(input);
		CONSOLE_LOG("parser returned");
	}
	CONSOLE_LOG("flushing input");
    keyboard.FlushInput();
	CONSOLE_LOG("reseting selected input");
    selected_input = 0;
	CONSOLE_LOG("input handled");
}

void Console::prevInput()
{
    if (selected_input < inputs.count())
    {
        selected_input++;
        keyboard.FillInput(inputs.getReverse(selected_input-1));
    }
}

void Console::nextInput()
{
    if (selected_input)
    {
        selected_input--;
        keyboard.FillInput(inputs.getReverse(selected_input-1));
    }
}

void Console::back()
{
    keyboard.DropLastCharacter();
}

Console::Console():profile(true,false),selected_input(0),parser(NULL),status(0),is_focused(false),target_open(false)
{
    input.pushProfile();
		input.bindProfile(profile);
        input.bind(Key::Return,std::bind(&Console::fetchInput,this));
        input.bind(Key::Back,std::bind(&Console::back,this));
        input.bind(Key::Up,std::bind(&Console::prevInput,this));
        input.bind(Key::Down,std::bind(&Console::nextInput,this));
    input.popProfile();
}

void	Console::bindCloseKey(Key::Name key)
{
	input.pushProfile();
		input.bindProfile(profile);
		input.bind(key,std::bind(&Console::closeEvent,this));
	input.popProfile();
}

Console::~Console()
{
    killFocus();
}

void Console::emptyLine()
{
    lines.add("");
}

void Console::print(const String&line)
{
    String local = line;
    while (local.length())
    {
        index_t nl = local.indexOf('\n');
        if (nl)
        {
            lines.add(local.subString(0,nl-1));
            local.erase(0,nl);
        }
        else
        {
            lines.add(local);
            return;
        }
    }
}

void Console::setInterpretor(pInputParser interpretor)
{
    parser = interpretor;
}

void Console::focus()
{
    if (is_focused)
        return;
    is_focused = true;
    keyboard.read = true;
    input.pushProfile();
        input.chooseProfile(profile);
}

void Console::killFocus()
{
    if (!is_focused)
        return;
    is_focused = false;
    input.popProfile();
    keyboard.read = false;
}

void Console::unfocus()
{
	killFocus();
}

void Console::forgetLastInput()
{
    inputs.erase(inputs.count()-1);
}


void Console::setState(bool open)
{
    target_open = open;
    if (open)
        focus();
    else
        killFocus();
    status = open;
}

void Console::setTarget(bool open)
{
    target_open = open;
    if (open)
        focus();
    else
        killFocus();
}


void Console::open()
{
    target_open = true;
    focus();
}

void Console::close()
{
    target_open = false;
    killFocus();
}


bool Console::focused()	const
{
    return is_focused;
}

bool Console::isOpen()	const
{
	return target_open;
}

void Console::toggle()
{
	setTarget(!target_open);
}

}
