// SetDialog.cpp : implementation file
//

#include "stdafx.h"

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#include "snmptree.h"
#include "asntypes.h"
#include "SetDialog.h"
#include "snmpcore.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetDialog dialog


CSetDialog::CSetDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSetDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetDialog)
	m_Name = _T("");
	m_Type = _T("");
	m_Value = _T("");
	//}}AFX_DATA_INIT
}


void CSetDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetDialog)
	DDX_Control(pDX, IDC_VALUE_EDIT, m_ValueEdit);
	DDX_Control(pDX, IDC_PREV_BUTTON, m_PrevButton);
	DDX_Control(pDX, IDC_NEXT_BUTTON, m_NextButton);
	DDX_Text(pDX, IDC_NAME_EDIT, m_Name);
	DDX_Text(pDX, IDC_TYPE_EDIT, m_Type);
	DDX_Text(pDX, IDC_VALUE_EDIT, m_Value);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetDialog, CDialog)
	//{{AFX_MSG_MAP(CSetDialog)
	ON_BN_CLICKED(IDC_NEXT_BUTTON, OnNextButton)
	ON_BN_CLICKED(IDC_PREV_BUTTON, OnPrevButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetDialog message handlers

void CSetDialog::OnOK() 
{
    if( ValidateData() ) {
        CDialog::OnOK();
    }
}

BOOL CSetDialog::ValidateData( void )
{
    int retcode;

	UpdateData( TRUE );

    // validate the current var
    ASN::Instance *instance = set_variables[ m_CurrentInstanceIdx ];

    retcode = instance->parse( m_Value );

    if( retcode != ASN::NoError ) {
        CString errstr;
        // FIXME - need better error message
        errstr.Format( "Invalid %s", m_Type );
        MessageBox( errstr, "Invalid Value", MB_ICONERROR );
        return FALSE;
    }

    return TRUE;
}

BOOL CSetDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
    int len;

    len = set_variables.GetSize();

    ASSERT( len > 0 );

    m_CurrentInstanceIdx = 0;

    // always start on the first entry so we don't want the user to try to go
    // past the "front" of the list
    m_PrevButton.EnableWindow(FALSE);

    // if we only have one entry, don't try to go beyond the end of the list
    if( len == 1 ) {
        m_NextButton.EnableWindow(FALSE);
    }
	
    UpdateVariableDisplay();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSetDialog::UpdateVariableDisplay( void )
{
    ASN::Instance *instance = set_variables[m_CurrentInstanceIdx];

    m_Name = instance->get_object_name_string();
    m_Type = instance->get_type_name();

    // get the simple display string, not the "pretty" display string with 
    // extra formatting since we want the user to be able to edit this string
    m_Value = instance->get_display_string();

    // push the new strings out to the dialog
	UpdateData( FALSE );

    // set all text in the value box editable
    m_ValueEdit.SetSel( 0, -1 );

    // set the title so user knows where she is in the collection of variables
    m_Title.Format( _T("Set %d of %d"), m_CurrentInstanceIdx+1, set_variables.GetSize() );
    SetWindowText( m_Title );
}

void CSetDialog::OnNextButton() 
{
    int next_idx;
    int len, max_idx;

    // if the current data is wrong, don't change
    if( ! ValidateData() ) {
        return;
    }

    len = set_variables.GetSize();
    max_idx = len-1;

    next_idx = m_CurrentInstanceIdx + 1;

    ASSERT( next_idx < len );
    if( next_idx >= len ) {
        // shouldn't happen (we should have disabled the Next button so we 
        // can't go past the end of the list)
        return;
    }

    // if we're going to be on the last entry, disable the next button so we
    // can't try to go beyond the end of the list
    if( next_idx == max_idx ) {
        m_NextButton.EnableWindow( FALSE );
    }

    // if we were on the first entry, reenable the prev button
    if( m_CurrentInstanceIdx == 0 ) {
        m_PrevButton.EnableWindow( TRUE );
    }

    m_CurrentInstanceIdx = next_idx;

    UpdateVariableDisplay();
}

void CSetDialog::OnPrevButton() 
{
    int prev_idx;
    int len, max_idx;

    // if the current data is wrong, don't change
    if( ! ValidateData() ) {
        return;
    }

    len = set_variables.GetSize();
    max_idx = len-1;

    ASSERT( m_CurrentInstanceIdx > 0 );
    if( m_CurrentInstanceIdx <= 0 ) {
        // shouldn't happen (we should have disabled the prev button so we 
        // can't go beyond the front of the list)
        return;
    }

    prev_idx = m_CurrentInstanceIdx - 1;

    // if we're going to be on the first entry, disable the prev button so we
    // can't try to go beyond the first of the list
    if( prev_idx == 0 ) {
        m_PrevButton.EnableWindow( FALSE );
    }

    // if we were on the last entry, reenable the next button
    if( m_CurrentInstanceIdx == max_idx ) {
        m_NextButton.EnableWindow( TRUE );
    }

    m_CurrentInstanceIdx = prev_idx;

    UpdateVariableDisplay();
}

