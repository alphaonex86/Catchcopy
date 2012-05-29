/** \file ClientCatchcopy.h
\brief Define the catchcopy client
\author alpha_one_x86
\version 0002
\date 2010 */

#ifndef CLIENTCATCHCOPY_H
#define CLIENTCATCHCOPY_H

#define BUFSIZE 512
#define S_LEN	8

#define C_INT(p)	*((int *) (p))
#define BLOCK_SIZ	64000
#define BUFFER_PIPE	32000

#include <windows.h>
#include <string>
#include <deque>
#include <stdlib.h>

typedef unsigned char byte_t;

class ClientCatchcopy
{
	public:
		ClientCatchcopy();
		~ClientCatchcopy();
		bool connectToServer();
		void disconnectFromServer();
		/// \brief to send order
		bool sendProtocol();
		bool setClientName(const std::wstring & name);
		bool addCopyWithDestination(std::deque<std::wstring> sources,const std::wstring & destination);
		bool addCopyWithoutDestination(std::deque<std::wstring> sources);
		bool addMoveWithDestination(std::deque<std::wstring> sources,const std::wstring & destination);
		bool addMoveWithoutDestination(std::deque<std::wstring> sources);
		/// \brief to send stream of string list
		bool sendRawOrderList(const std::deque<std::wstring> & order,const bool & first_try=true);
		bool isConnected();
	private:
		HANDLE m_hpipe;
		unsigned int idNextOrder;
		char *m_pipename;
		byte_t *m_blk;
		int m_tot;
		int m_len;

		char * toHex(const char *str);
		int dataToPipe();
		int addInt32(int value);
		int addStr(WCHAR *data);
		byte_t *blkGrowing(int added);
		void setInt32(int offset, int value);
		int toBigEndian(WCHAR *p);
		void clear();
		int writePipe(HANDLE hPipe, byte_t *ptr, int len);
};

#endif // CLIENTCATCHCOPY_H
