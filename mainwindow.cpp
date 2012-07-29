#include "mainwindow.h"

#include <QCloseEvent>
#include <QAction>
#include <QMenu>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QCoreApplication>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent, Qt::WFlags flags)
	: QWidget(parent, Qt::Tool | Qt::WindowStaysOnTopHint), workerEnabled(true)
{

	textEdit = new QTextEdit(this);
	textEdit->setReadOnly(true);

	QVBoxLayout* boxLayout = new QVBoxLayout(this);
	boxLayout->addWidget(textEdit);
	boxLayout->setContentsMargins(0,0,0,0);
	setLayout(boxLayout);

	enableAction = new QAction(QString("Enable"),this);
	enableAction->setCheckable(true);
	enableAction->setChecked(workerEnabled);
	connect(enableAction,SIGNAL(toggle(bool)),this,SLOT(enableToggled(bool)));

	showAction = new QAction(QString("Show output"),this);
	connect(showAction,SIGNAL(triggered()),this,SLOT(show()));
	exitAction = new QAction(QString("Exit"),this);
	connect(exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
	
	QMenu *trayMenu = new QMenu(this);
	trayMenu->addAction(enableAction);
	trayMenu->addAction(showAction);
	trayMenu->addSeparator();
	trayMenu->addAction(exitAction);

	trayIcon = new QSystemTrayIcon(QIcon(":/ShugoConsole/shugo.png"),this);
	trayIcon->setContextMenu(trayMenu);

	trayIcon->show();

	timer = new QTimer(this);
	connect(timer,SIGNAL(timeout()),this,SLOT(scantimer()));
	timer->setInterval(5000);

	if(workerEnabled)
	{
		timer->start();
	}
	else
	{
		timer->stop();
	}
}

class MemoryPattern
{
private:
	int align_offset;
	BYTE *pattern;
	int patternsize;
public:
	MemoryPattern(int _align_offset, BYTE *_pattern, int _patternsize)
		: align_offset(_align_offset), pattern(_pattern), patternsize(_patternsize)
	{
	}

	bool match(BYTE *mem)
	{
		int mem_it = align_offset;
		int pattern_it = 0;
		do
		{
			if(mem[mem_it] != pattern[pattern_it])
				return false;
			mem_it++;
			pattern_it++;
		}
		while(pattern_it < patternsize);
		return true;
	}

	int totalsize() const { return align_offset + patternsize; }
};

class RemoteMemoryLookup
{
private:
	BYTE *buffer;
	int buffer_size;
public:
	RemoteMemoryLookup(int _buffer_size) : buffer(NULL), buffer_size(_buffer_size)
	{
		buffer = (BYTE*)malloc(buffer_size);
	}

	~RemoteMemoryLookup()
	{
		if(buffer)
		{
			free(buffer);
			buffer = NULL;
		}
	}

	BYTE* lookupPage(HANDLE process, MEMORY_BASIC_INFORMATION* mem_info, MemoryPattern* pattern)
	{
		BYTE* begin = (BYTE*)mem_info->BaseAddress;
		SIZE_T remaining_bytes = mem_info->RegionSize;
		SIZE_T bytes_read = 0;
		SIZE_T bytes_to_read = remaining_bytes;
		if(bytes_to_read > buffer_size)
		{
			bytes_to_read = buffer_size;
		}
		while(ReadProcessMemory(process,begin,buffer,bytes_to_read,&bytes_read))
		{
			if(bytes_read == 0)
				break;

			SIZE_T end = bytes_read - pattern->totalsize();

			for(SIZE_T i = 0 ; i < end; i += 16)
			{
				if(pattern->match((BYTE*)(buffer+i)))
					return begin+i;
			}

			if(bytes_read >= remaining_bytes)
			{
				break;
			}
			else
			{
				remaining_bytes -= bytes_read;
				begin += bytes_read;

				bytes_to_read = remaining_bytes;
				if(bytes_to_read > buffer_size)
				{
					bytes_to_read = buffer_size;
				}
			}
		}

		return NULL;
	}

	BYTE* addressOf(HANDLE process, MemoryPattern* pattern)
	{
		const char* read_address = 0;
		MEMORY_BASIC_INFORMATION mem_info = {0};
		BYTE* ret = NULL;

		while(VirtualQueryEx(process,read_address,&mem_info,sizeof(MEMORY_BASIC_INFORMATION)))
		{
			if (mem_info.Type == MEM_PRIVATE && mem_info.State == MEM_COMMIT)
			{
				if(mem_info.Protect == PAGE_READWRITE || mem_info.Protect == PAGE_EXECUTE_READWRITE)
				{
					ret = lookupPage(process,&mem_info,pattern);

					if(ret)
						break;
				}
			}

			const char* next_address = (const char*)mem_info.BaseAddress + mem_info.RegionSize;
			if(next_address > read_address)
			{
				read_address = next_address;
			}
			else
			{
				break;
			}
		}

		return ret;
	}
};

#define CVAR_MEM_SIZE_32 (28*16)
#define CVAR_MEM_NAME_32 5
#define CVAR_MEM_INT_32 160
#define CVAR_MEM_FLOAT_32 164
#define CVAR_MEM_STRING_32 168

#define CVAR_MEM_NAME_64 9
#define CVAR_MEM_INT_64 184
#define CVAR_MEM_FLOAT_64 188
#define CVAR_MEM_STRING_64 192

#define CVAR_STRING_SIZE 256
#define CVAR_MAX_SIZE (28*16)

class CVar
{
private:
	HANDLE h;
	char* p;
	bool x;

	struct {
		int int_value;
		int float_value;
		int string_value;
	} offsets;
public:
	CVar() : h(0), p(NULL), x(false) { setupoffsets(); }
	CVar(HANDLE _h,char* _p, bool _x) : h(_h), p(_p),x(_x) { setupoffsets(); }
	CVar(const CVar& o) : h(o.h), p(o.p), x(o.x) { setupoffsets(); }
	const CVar& operator=(const CVar& o) { h = o.h; p = o.p; x = o.x; setupoffsets(); return *this; }

	void setupoffsets()
	{
		if(x)
		{
			offsets.int_value = CVAR_MEM_INT_64;
			offsets.float_value = CVAR_MEM_FLOAT_64;
			offsets.string_value = CVAR_MEM_STRING_64;
		}
		else
		{
			offsets.int_value = CVAR_MEM_INT_32;
			offsets.float_value = CVAR_MEM_FLOAT_32;
			offsets.string_value = CVAR_MEM_STRING_32;
		}
	}

	void set(int i)
	{
		char buffer[CVAR_MAX_SIZE];
		memset(buffer,0,CVAR_MAX_SIZE);

		*(int*)(buffer+offsets.int_value) = i;
		*(float*)(buffer+offsets.float_value) = (float)i;
		int sz = sprintf_s((char*)(buffer+offsets.string_value),CVAR_STRING_SIZE,"%d",i);

		WriteProcessMemory(h,p+offsets.int_value,
				                buffer+offsets.int_value,
								sizeof(int)+sizeof(float)+sz+1,NULL);
	}

	void set(float f)
	{
		char buffer[CVAR_MAX_SIZE];
		memset(buffer,0,CVAR_MAX_SIZE);

		*(int*)(buffer+offsets.int_value) = (int)f;
		*(float*)(buffer+offsets.float_value) = f;
		int sz = sprintf_s((char*)(buffer+offsets.string_value),CVAR_STRING_SIZE,"%f",f);

		WriteProcessMemory(h,p+offsets.int_value,
				                buffer+offsets.int_value,
								sizeof(int)+sizeof(float)+sz+1,NULL);
	}

	void set(const char* s)
	{
		char buffer[CVAR_MAX_SIZE];
		memset(buffer,0,CVAR_MAX_SIZE);

		*(int*)(buffer+offsets.int_value) = 0;
		*(float*)(buffer+offsets.float_value) = 0.0f;
		int sz = sprintf_s((char*)(buffer+offsets.string_value),CVAR_STRING_SIZE,"%s",s);

		WriteProcessMemory(h,p+offsets.int_value,
				                buffer+offsets.int_value,
								sizeof(int)+sizeof(float)+sz+1,NULL);
	}

	int getInt()
	{
		int i = 0;
		ReadProcessMemory(h,p+offsets.int_value,(void*)&i,sizeof(int),NULL);
		return i;
	}

	float getFloat()
	{
		float f = 0;
		ReadProcessMemory(h,p+offsets.float_value,(void*)&f,sizeof(float),NULL);
		return f;
	}

	bool valid() const { return h != NULL && p != NULL; }
};

void MainWindow::scantimer()
{
	if(!workerEnabled)
		return;

	QList<HANDLE> process_list = siquery.findProcessByImageName("aion.bin");

	// remove all pid not running
	processes_done &= QSet<HANDLE>::fromList(process_list);

	foreach(HANDLE h, process_list)
	{
		if(!processes_done.contains(h))
		{
			trayIcon->showMessage("Brass-Eye Grogget says...",QString("I found an aion.bin instance (PID %1), my lads are looking for the console...").arg(qulonglong(h)));
			textEdit->append(QString("> opening aion process (PID %1)...").arg(qulonglong(h)));
			HANDLE process = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION,FALSE,(DWORD)h);
			if(process)
			{
				textEdit->append(QString("> opened aion process"));
				
				// 128k buffer for remote reading
				RemoteMemoryLookup rml(131072);
				BYTE* address_of_con_disable_console;

				// messy code below, should not-work on 32-bit windows...

				BOOL iswow64 = FALSE;
				IsWow64Process(process,&iswow64);

				if(iswow64)
				{
					textEdit->append(QString("> 32-bit"));
					MemoryPattern pattern(CVAR_MEM_NAME_32 - 1,(BYTE*)"\0con_disable_console",20);
					address_of_con_disable_console = rml.addressOf(process,&pattern);
				}
				else
				{
					if (sizeof(void *) == 8)
					{
						// I AM 64 bit
						textEdit->append(QString("> 64-bit"));
						MemoryPattern pattern(CVAR_MEM_NAME_64 - 1,(BYTE*)"\0con_disable_console",20);
						address_of_con_disable_console = rml.addressOf(process,&pattern);
					}
					else
					{
						textEdit->append(QString("> cannot scan 64-bit processes from 32-bit ones, use the 64-bit version of this tool"));
						processes_done.insert(h);
					}
				}

				if(address_of_con_disable_console)
				{
					CVar con_disable_console(process, (char*)address_of_con_disable_console, iswow64 == 0);

					textEdit->append("> found con_disable_console");
					if(con_disable_console.getInt() != 0)
					{
						con_disable_console.set(0);
						textEdit->append(QString("> set con_disable_console to 0 !"));
						trayIcon->showMessage("Brass-Eye Grogget says...",QString("Golden-Eye Mantutu pushed some memory around, console is now enabled !"));
					}
					else
					{
						textEdit->append(QString("> con_disable_console was already set to 0"));
						trayIcon->showMessage("Brass-Eye Grogget says...",QString("Console is already enabled, you must already have some powerful stigma."));
						
					}

					processes_done.insert(h);
				}

				CloseHandle(process);
			}
			else
			{
				textEdit->append(QString("> could not open process (not enough privileges, try running as an administrator)"));
				trayIcon->showMessage("Brass-Eye Grogget says...",QString("Lahulahu broke one leg ! Are you sure you gave enough admin rights to us shugos ?"));
				processes_done.insert(h);
			}
		}
	}
}

void MainWindow::enableToggled(bool checked)
{
	workerEnabled = checked;
	if(workerEnabled)
	{
		timer->start();
	}
	else
	{
		timer->stop();
	}
}

void MainWindow::closeEvent (QCloseEvent *e)
{
	e->ignore();
	this->hide();
}

MainWindow::~MainWindow()
{

}
