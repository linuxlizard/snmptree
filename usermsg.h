#ifndef USERMSG_H
#define USERMSG_H

#define MSG_ASYNC_SELECT    (WM_USER+0x100)   // WSAAsyncSelect
#define MSG_SNMP_RESULT     (WM_USER+0x101)   // NetSNMP has data results to display
#define MSG_SET_AGENT       (WM_USER+0x102)   // user selected a new agent
#define MSG_SNMP_WALK_START (WM_USER+0x103)   // start a walk on WPARAM oid
#define MSG_SNMP_TIMEOUT    (WM_USER+0x104)   // No response to SNMP request
#define MSG_SNMP_WALK_DONE  (WM_USER+0x105)   // snmp walk is finished
#define MSG_SNMP_SET_DONE   (WM_USER+0x106)   // snmp set is finished
#define MSG_SWAP_VIEW       (WM_USER+0x107)   // swap active CSplitterWnd in MainFrm
#define MSG_SNMP_SET_START  (WM_USER+0x108)   // start a set by reading selected list entries

#endif

