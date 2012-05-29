/** \file ClientCatchcopy.cpp
\brief Define the catchcopy client
\author alpha_one_x86
\version 0002
\date 2010 */

#include <stdio.h>

#include "ClientCatchcopy.h"

ClientCatchcopy::ClientCatchcopy()
{
	m_hpipe=NULL;
	idNextOrder=0;
	const char prefix[]="\\\\.\\pipe\\advanced-copier-";
	char uname[1024];
	DWORD len=1023;
	char *data;
	// false ??
	GetUserNameA(uname, &len);
	// convert into hexa
	data = toHex(uname);
	m_pipename = (char *) malloc(sizeof(prefix)+strlen(data)+2);
#if defined(_MFC_VER)
	strcpy_s(m_pipename, _countof(prefix) ,prefix);
	strcat_s(m_pipename, sizeof(prefix)+strlen(data)+2,data);
#else
	strcpy(m_pipename, prefix);
	strcat(m_pipename, data);
#endif
	free(data);
	m_blk=NULL;
	m_len=0;
	m_tot=0;
}

ClientCatchcopy::~ClientCatchcopy()
{
	disconnectFromServer();
}

// Dump UTF16 (little endian)
char * ClientCatchcopy::toHex(const char *str)
{
	char *p, *sz;
	size_t len;
	if (str==NULL)
		return NULL;
	len= strlen(str);
	p = sz = (char *) malloc((len+1)*4);
	// username goes hexa...
	for (size_t i=0; i<len; i++)
	{
		#if defined(_MFC_VER)
			sprintf_s(p, (len+1)*4, "%.2x00", str[i]);
		#else
			sprintf(p, "%.2x00", str[i]);
		#endif
		p+=4;
	}
	*p=0;
	return sz;
}

bool ClientCatchcopy::connectToServer()
{
	if (m_hpipe==NULL)
	{
		// create pipe
		m_hpipe = CreateFileA(m_pipename, GENERIC_READ|GENERIC_WRITE|FILE_FLAG_OVERLAPPED , 0, NULL, OPEN_EXISTING, 0, NULL);
		if (m_hpipe==INVALID_HANDLE_VALUE)
		{
			m_hpipe=NULL;
			if (GetLastError()!= ERROR_PIPE_BUSY)
				return false;
			if (!WaitNamedPipeA(m_pipename, 10000))
			{
				CloseHandle(m_hpipe);
				m_hpipe=NULL;
				return false;
			}
		}
	}
	return true;
}

void ClientCatchcopy::disconnectFromServer()
{
	clear();
	if(m_hpipe!=NULL)
	{
		/* needed to not crash the server when have data to read */
		FlushFileBuffers(m_hpipe);
		CloseHandle(m_hpipe);
		m_hpipe=NULL;
	}
}

/// \brief to send order
bool ClientCatchcopy::sendProtocol()
{
	std::deque<std::wstring> data;
	data.push_back(L"protocol");
	data.push_back(L"0002");
	return sendRawOrderList(data);
}

bool ClientCatchcopy::setClientName(const std::wstring & name)
{
	std::deque<std::wstring> data;
	data.push_back(L"client");
	data.push_back(name);
	return sendRawOrderList(data);
}

bool ClientCatchcopy::addCopyWithDestination(std::deque<std::wstring> sources,const std::wstring & destination)
{
	sources.push_front(L"cp");
	sources.push_back(destination);
	return sendRawOrderList(sources);
}

bool ClientCatchcopy::addCopyWithoutDestination(std::deque<std::wstring> sources)
{
	sources.push_front(L"cp-?");
	return sendRawOrderList(sources);
}

bool ClientCatchcopy::addMoveWithDestination(std::deque<std::wstring> sources,const std::wstring & destination)
{
	sources.push_front(L"mv");
	sources.push_back(destination);
	return sendRawOrderList(sources);
}

bool ClientCatchcopy::addMoveWithoutDestination(std::deque<std::wstring> sources)
{
	sources.push_front(L"mv-?");
	return sendRawOrderList(sources);
}

/// \brief to send stream of string list
bool ClientCatchcopy::sendRawOrderList(const std::deque<std::wstring> & order,const bool & first_try)
{
	if(m_hpipe!=NULL)
	{
		int data_size=sizeof(int)*3;
		for(unsigned int i=0;i<order.size();i++)
		{
			data_size+=(int)sizeof(int);
			data_size+=(int)sizeof(wchar_t)*(int)order.at(i).size();
		}
		addInt32(data_size);
		addInt32(idNextOrder++);
		addInt32((int)order.size());
		for(unsigned int i=0;i<order.size();i++)
			//set string contenant
			addStr((WCHAR*)order.at(i).c_str());
		if(dataToPipe()<0)
		{
			if(first_try)
			{
				disconnectFromServer();
				connectToServer();
				return sendRawOrderList(order,false);
			}
			else
					clear();
		}
		return true;
	}
	else
	{
		clear();
		return false;
	}
}

// Send data block to named pipe
int ClientCatchcopy::dataToPipe()
{
	byte_t *ptr;
	int ret=0, max;
	if (m_hpipe!=NULL)
	{
			ptr = m_blk;
			while (!ret && m_len)
			{
					max=(m_len>BUFFER_PIPE) ? BUFFER_PIPE:m_len;
					if(writePipe(m_hpipe, ptr, max)!=0)
					{
							ret=-2;
							break;
					}
					m_len-=max;
					ptr+=max;
			}
	}
        return ret;
}

int ClientCatchcopy::writePipe(HANDLE hPipe, byte_t *ptr, int len)
{
	DWORD cbWritten;
	if (!WriteFile(hPipe, ptr, len, &cbWritten, NULL))
		return -4;
	return 0;
}

// Add int32 (big-endian) into binary block
int ClientCatchcopy::addInt32(int value)
{
	blkGrowing(sizeof(int));
	// add value
	setInt32(m_len, value);
	m_len+=sizeof(int);
	return m_len-sizeof(int);
}

void ClientCatchcopy::setInt32(int offset, int value)
{
	C_INT(m_blk+offset)=htonl(value);
}

// Add unicode string into binary block from ASCIIZ
int ClientCatchcopy::addStr(WCHAR *data)
{
	int ret=-1, len;
	WCHAR *x;
	if (data!=NULL && *data)
	{
		// le => be
		x = _wcsdup(data);
		len = toBigEndian(x);
		// set size of string
		ret = addInt32(len);
		// and add it to block
		blkGrowing(len);
		memmove(m_blk+m_len, x, len);
		m_len+=len;
		free(x);
	}
	return ret;
}

// resize binary block (if needed)
byte_t *ClientCatchcopy::blkGrowing(int added)
{
	if (m_len+added>m_tot)
	{
		// check if added isn't bigger than buffer itself...
		m_tot+= (added>BLOCK_SIZ) ? added:BLOCK_SIZ;
		m_blk = (byte_t *) realloc(m_blk, m_tot);
	}
	return m_blk+m_len;
}

int ClientCatchcopy::toBigEndian(WCHAR *p)
{
	WCHAR tmp;
	int ret=0;
	while(*p)
	{
		tmp = htons(*p);
		*p++=tmp;
		ret+=2;
	}
	return ret;
}

void ClientCatchcopy::clear()
{
	m_tot=0;
	m_len=0;
	if (m_blk!=NULL)
	{
		free(m_blk);
		m_blk=NULL;
	}
}

bool ClientCatchcopy::isConnected()
{
	if(m_hpipe==NULL)
		return false;
		
		bool fSuccess = PeekNamedPipe(
	  m_hpipe,
	  NULL,
	  NULL,
	  NULL,
	  NULL,
	  NULL
		);

	if(!fSuccess && GetLastError() != ERROR_MORE_DATA)
	{
		disconnectFromServer();
		return false;
	}
	else
		return true;
}
