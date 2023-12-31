/* Definitions for the stream-based netstrmworking class.
 *
 * Copyright 2007, 2008 Rainer Gerhards and Adiscon GmbH.
 *
 * This file is part of the rsyslog runtime library.
 *
 * The rsyslog runtime library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The rsyslog runtime library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the rsyslog runtime library.  If not, see <http://www.gnu.org/licenses/>.
 *
 * A copy of the GPL can be found in the file "COPYING" in this distribution.
 * A copy of the LGPL can be found in the file "COPYING.LESSER" in this distribution.
 */

#ifndef INCLUDED_NETSTRM_H
#define INCLUDED_NETSTRM_H

#include "netstrms.h"

/* the netstrm object */
struct netstrm_s {
	BEGINobjInstance;	/* Data to implement generic object - MUST be the first data element! */
	nsd_t *pDrvrData;	/**< the driver's data elements (at most other places, this is called pNsd) */
	nsd_if_t Drvr;		/**< our stream driver */
	netstrms_t *pNS;	/**< pointer to our netstream subsystem object */
};


/* interface */
BEGINinterface(netstrm) /* name must also be changed in ENDinterface macro! */
	rsRetVal (*Construct)(netstrm_t **ppThis);
	rsRetVal (*ConstructFinalize)(netstrm_t *pThis);
	rsRetVal (*Destruct)(netstrm_t **ppThis);
	rsRetVal (*AbortDestruct)(netstrm_t **ppThis);
	rsRetVal (*LstnInit)(netstrms_t *pNS, void *pUsr, rsRetVal(*)(void*,netstrm_t*),
		             uchar *pLstnPort, uchar *pLstnIP, int iSessMax);
	rsRetVal (*AcceptConnReq)(netstrm_t *pThis, netstrm_t **ppNew);
	rsRetVal (*Rcv)(netstrm_t *pThis, uchar *pRcvBuf, ssize_t *pLenBuf);
	rsRetVal (*Send)(netstrm_t *pThis, uchar *pBuf, ssize_t *pLenBuf);
	rsRetVal (*Connect)(netstrm_t *pThis, int family, unsigned char *port, unsigned char *host);
	rsRetVal (*GetRemoteHName)(netstrm_t *pThis, uchar **pszName);
	rsRetVal (*GetRemoteIP)(netstrm_t *pThis, uchar **pszIP);
	rsRetVal (*SetDrvrMode)(netstrm_t *pThis, int iMode);
	rsRetVal (*SetDrvrAuthMode)(netstrm_t *pThis, uchar*);
	rsRetVal (*SetDrvrPermPeers)(netstrm_t *pThis, permittedPeers_t*);
	void     (*CheckConnection)(netstrm_t *pThis);	/* This is a trick mostly for plain tcp syslog */
	/* the GetSock() below is a hack to make imgssapi work. In the long term,
	 * we should migrate imgssapi to a stream driver, which will relieve us of
	 * this problem. Please note that nobody else should use GetSock(). Using it 
	 * will also tie the caller to nsd_ptcp, because other drivers may not support
	 * it at all. Once the imgssapi problem is solved, GetSock should be removed from
	 * this interface. -- rgerhards, 2008-05-05
	 */
	rsRetVal (*GetSock)(netstrm_t *pThis, int *pSock);
	rsRetVal (*GetRemAddr)(netstrm_t *pThis, struct sockaddr_storage **ppAddr);
	/* getRemAddr() is an aid needed by the legacy ACL system. It exposes the remote
	 * peer's socket addr structure, so that the legacy matching functions can work on
	 * it. Note that this ties netstream drivers to things that can be implemented over
	 * sockets - not really desirable, but not the end of the world... TODO: should be
	 * reconsidered when a new ACL system is build. -- rgerhards, 2008-12-01
	 */
ENDinterface(netstrm)
#define netstrmCURR_IF_VERSION 3 /* increment whenever you change the interface structure! */
/* interface version 3 added GetRemAddr() */

/* prototypes */
PROTOTYPEObj(netstrm);

/* the name of our library binary */
#define LM_NETSTRM_FILENAME LM_NETSTRMS_FILENAME

#endif /* #ifndef INCLUDED_NETSTRM_H */
