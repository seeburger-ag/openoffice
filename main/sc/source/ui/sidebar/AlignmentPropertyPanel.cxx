/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/

#include "precompiled_sc.hxx"

#include <sfx2/sidebar/propertypanel.hrc>
#include <sfx2/sidebar/Theme.hxx>
#include <sfx2/sidebar/ControlFactory.hxx>
#include <AlignmentPropertyPanel.hxx>
#include <AlignmentPropertyPanel.hrc>
#include <svx/dialmgr.hxx>
#include "sc.hrc"
#include "scresid.hxx"
#include <sfx2/bindings.hxx>
#include <sfx2/dispatch.hxx>
#include <svx/algitem.hxx>
#include <svx/dlgutil.hxx>
#include <vcl/toolbox.hxx>
#include <svx/sidebar/SidebarDialControl.hxx>

using namespace css;
using namespace cssu;

#define A2S(pString) (::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(pString)))

//////////////////////////////////////////////////////////////////////////////
// namespace open

namespace sc { namespace sidebar {

//////////////////////////////////////////////////////////////////////////////

AlignmentPropertyPanel::AlignmentPropertyPanel(
    Window* pParent,
    const cssu::Reference<css::frame::XFrame>& rxFrame,
    SfxBindings* pBindings)
:   Control(
        pParent, 
        ScResId(RID_PROPERTYPANEL_SC_ALIGNMENT)),
    mpTBHorizontal(new ToolBox(this, ScResId(TBX_HORIZONTAL_ALIGNMENT))),
    mpTBVertical(new ToolBox(this, ScResId(TBX_VERTICAL_ALIGN))),
    mpFTLeftIndent(new FixedText(this, ScResId(FT_LEFT_INDENT))),
    mpMFLeftIndent(new MetricField(this, ScResId(MF_LEFT_INDENT))),
    mpCBXWrapText(new CheckBox(this, ScResId(CBX_WRAP))),
    mpCBXMergeCell(new CheckBox(this, ScResId(CBX_MERGE))),
    mpFtRotate(new FixedText(this, ScResId(FT_ORIENT))),
    mpCtrlDial(new svx::sidebar::SidebarDialControl(this, ScResId(DIAL_CONTROL))), // , true)),
    mpMtrAngle(new MetricBox(this, ScResId(CBOX_ANGLE))),
    mpCbStacked(new CheckBox(this, ScResId(CBX_VERT))),
    maAlignHorControl(SID_H_ALIGNCELL, *pBindings, *this),
    maAlignVerControl(SID_V_ALIGNCELL, *pBindings, *this),
    maLeftIndentControl(SID_ATTR_ALIGN_INDENT, *pBindings, *this),
    maMergeCellControl(FID_MERGE_TOGGLE, *pBindings, *this),
    maWrapTextControl(SID_ATTR_ALIGN_LINEBREAK, *pBindings, *this),
    maAngleControl(SID_ATTR_ALIGN_DEGREES, *pBindings, *this),
    maStackControl(SID_ATTR_ALIGN_STACKED, *pBindings, *this),
    maIMGAlignLeft(ScResId(IMG_ALIGN_LEFT)),
    maIMGAlignCenter(ScResId(IMG_ALIGN_CENTER)),
    maIMGAlignRight(ScResId(IMG_ALIGN_RIGHT)),
    maIMGAlignJust(ScResId(IMG_ALIGN_JUST)),
    maIMGAlignTop(ScResId(IMG_ALIGN_TOP)),
    maIMGAlignCenterV(ScResId(IMG_ALIGN_CENTER_V)),
    maIMGAlignBottom(ScResId(IMG_ALIGN_BOTTOM)),
    meHorAlignState(SVX_HOR_JUSTIFY_STANDARD),
    meVerAlignState(SVX_VER_JUSTIFY_STANDARD),
    mbMultiDisable(false),
    mxFrame(rxFrame),
    maContext(),
    mpBindings(pBindings)
{
    Initialize();
    FreeResource();
}

//////////////////////////////////////////////////////////////////////////////

AlignmentPropertyPanel::~AlignmentPropertyPanel()
{
}

//////////////////////////////////////////////////////////////////////////////

void AlignmentPropertyPanel::Initialize()
{
	mpTBHorizontal->SetItemImage(ID_SUBSTLEFT, maIMGAlignLeft);
	mpTBHorizontal->SetItemImage(ID_SUBSTCENTER, maIMGAlignCenter);
	mpTBHorizontal->SetItemImage(ID_SUBSTRIGHT, maIMGAlignRight);
	mpTBHorizontal->SetItemImage(ID_SUBSTJUSTIFY, maIMGAlignJust);
	Size aTbxSize( mpTBHorizontal->CalcWindowSizePixel() );
	mpTBHorizontal->SetOutputSizePixel( aTbxSize );
	mpTBHorizontal->SetBackground(Wallpaper());
	mpTBHorizontal->SetPaintTransparent(true);
	Link aLink = LINK(this, AlignmentPropertyPanel, TbxHorAlignSelectHdl);
	mpTBHorizontal->SetSelectHdl ( aLink );

	mpTBVertical->SetItemImage(IID_VERT_TOP, maIMGAlignTop);
	mpTBVertical->SetItemImage(IID_VERT_CENTER, maIMGAlignCenterV);
	mpTBVertical->SetItemImage(IID_VERT_BOTTOM, maIMGAlignBottom);
	Size aTbxSize2( mpTBVertical->CalcWindowSizePixel() );
	mpTBVertical->SetOutputSizePixel( aTbxSize2 );
	mpTBVertical->SetBackground(Wallpaper());
	mpTBVertical->SetPaintTransparent(true);
	aLink = LINK(this, AlignmentPropertyPanel, TbxVerAlignSelectHdl);
	mpTBVertical->SetSelectHdl ( aLink );

	mpFTLeftIndent->Disable();
	mpMFLeftIndent->Disable();
	mpMFLeftIndent->SetAccessibleName(::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Left Indent")));	//wj acc
	aLink = LINK(this, AlignmentPropertyPanel, MFLeftIndentMdyHdl);
	mpMFLeftIndent->SetModifyHdl ( aLink );

	aLink = LINK(this, AlignmentPropertyPanel, CBOXMergnCellClkHdl);
	mpCBXMergeCell->SetClickHdl ( aLink );
	
	aLink = LINK(this, AlignmentPropertyPanel, CBOXWrapTextClkHdl);
	mpCBXWrapText->SetClickHdl ( aLink );

	//rotation control
	mpCtrlDial->SetAccessibleName(::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Text Orientation")));	//wj acc
	mpCtrlDial->SetModifyHdl(LINK( this, AlignmentPropertyPanel, RotationHdl));

    //rotation
	mpMtrAngle->SetAccessibleName(::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Text Orientation")));	//wj acc
	mpMtrAngle->SetModifyHdl(LINK( this, AlignmentPropertyPanel, AngleModifiedHdl));
	mpMtrAngle->EnableAutocomplete( false );

    //Vertical stacked
	mpCbStacked->SetClickHdl( LINK( this, AlignmentPropertyPanel, ClickStackHdl ) );

	mpMtrAngle->InsertValue(0, FUNIT_CUSTOM);
	mpMtrAngle->InsertValue(45, FUNIT_CUSTOM);
	mpMtrAngle->InsertValue(90, FUNIT_CUSTOM);
	mpMtrAngle->InsertValue(135, FUNIT_CUSTOM);
	mpMtrAngle->InsertValue(180, FUNIT_CUSTOM);
	mpMtrAngle->InsertValue(225, FUNIT_CUSTOM);
	mpMtrAngle->InsertValue(270, FUNIT_CUSTOM);
	mpMtrAngle->InsertValue(315, FUNIT_CUSTOM);

    mpTBHorizontal->SetAccessibleRelationLabeledBy(mpTBHorizontal.get());
	mpTBVertical->SetAccessibleRelationLabeledBy(mpTBVertical.get());
	mpMFLeftIndent->SetAccessibleRelationLabeledBy(mpFTLeftIndent.get());
	mpMtrAngle->SetAccessibleRelationLabeledBy(mpFtRotate.get());
#ifdef HAS_IA2
	mpMtrAngle->SetMpSubEditAccLableBy(mpFtRotate.get());
#endif
}

//////////////////////////////////////////////////////////////////////////////

IMPL_LINK( AlignmentPropertyPanel, AngleModifiedHdl, void *, EMPTYARG )
{
	String sTmp = mpMtrAngle->GetText();

	sal_Unicode nChar = sTmp.GetChar( 0 );
	if((sTmp.Len()== 1 &&  nChar == '-') || 
		(nChar != '-' && ((nChar < '0') || (nChar > '9') ) ))	////modify 
		return 0;

	double dTmp = sTmp.ToDouble();
	FormatDegrees(dTmp);

	sal_Int64 nTmp = (sal_Int64)dTmp*100;
	SfxInt32Item aAngleItem( SID_ATTR_ALIGN_DEGREES,(sal_uInt32) nTmp);
	
	GetBindings()->GetDispatcher()->Execute(
		SID_ATTR_ALIGN_DEGREES, SFX_CALLMODE_RECORD, &aAngleItem, 0L );
	return 0;
}

//////////////////////////////////////////////////////////////////////////////

IMPL_LINK( AlignmentPropertyPanel, RotationHdl, void *, EMPTYARG )
{
	sal_Int32 nTmp = mpCtrlDial->GetRotation();
	SfxInt32Item aAngleItem( SID_ATTR_ALIGN_DEGREES,(sal_uInt32) nTmp);
	
	GetBindings()->GetDispatcher()->Execute(
		SID_ATTR_ALIGN_DEGREES, SFX_CALLMODE_RECORD, &aAngleItem, 0L );

	return 0;
}

//////////////////////////////////////////////////////////////////////////////

IMPL_LINK( AlignmentPropertyPanel, ClickStackHdl, void *, EMPTYARG )
{
	bool bVertical = mpCbStacked->IsChecked() ? true : false;
	SfxBoolItem  aStackItem( SID_ATTR_ALIGN_STACKED, bVertical );
	GetBindings()->GetDispatcher()->Execute(
		SID_ATTR_ALIGN_STACKED, SFX_CALLMODE_RECORD, &aStackItem, 0L );
	return 0;
}

//////////////////////////////////////////////////////////////////////////////

IMPL_LINK(AlignmentPropertyPanel, TbxHorAlignSelectHdl, ToolBox*, pToolBox)
{
	sal_uInt16 nId = pToolBox->GetCurItemId();

	if(nId == ID_SUBSTLEFT)
	{
		if(meHorAlignState != SVX_HOR_JUSTIFY_LEFT)
			meHorAlignState = SVX_HOR_JUSTIFY_LEFT;
		else
			meHorAlignState = SVX_HOR_JUSTIFY_STANDARD;
	}
	else if( nId == ID_SUBSTCENTER )
	{
		if(meHorAlignState != SVX_HOR_JUSTIFY_CENTER)
			meHorAlignState = SVX_HOR_JUSTIFY_CENTER;
		else
			meHorAlignState = SVX_HOR_JUSTIFY_STANDARD;
	}
	else if( nId == ID_SUBSTRIGHT )
	{
		if(meHorAlignState != SVX_HOR_JUSTIFY_RIGHT)
			meHorAlignState = SVX_HOR_JUSTIFY_RIGHT;
		else
			meHorAlignState = SVX_HOR_JUSTIFY_STANDARD;
	}
	else if( nId == ID_SUBSTJUSTIFY )
	{
		if(meHorAlignState != SVX_HOR_JUSTIFY_BLOCK)
			meHorAlignState = SVX_HOR_JUSTIFY_BLOCK;
		else
			meHorAlignState = SVX_HOR_JUSTIFY_STANDARD;
	}
	SvxHorJustifyItem aHorItem(meHorAlignState, SID_H_ALIGNCELL);
	GetBindings()->GetDispatcher()->Execute(SID_H_ALIGNCELL, SFX_CALLMODE_RECORD, &aHorItem, 0L);
	UpdateHorAlign();
	return 0;
}

//////////////////////////////////////////////////////////////////////////////

IMPL_LINK(AlignmentPropertyPanel, TbxVerAlignSelectHdl, ToolBox*, pToolBox)
{
	sal_uInt16 nId = pToolBox->GetCurItemId();

	//Bold
	if(nId == IID_VERT_TOP)
	{
		if(meVerAlignState != SVX_VER_JUSTIFY_TOP)
			meVerAlignState = SVX_VER_JUSTIFY_TOP;
		else
			meVerAlignState = SVX_VER_JUSTIFY_STANDARD;
	}
	else if( nId == IID_VERT_CENTER )
	{
		if(meVerAlignState != SVX_VER_JUSTIFY_CENTER)
			meVerAlignState = SVX_VER_JUSTIFY_CENTER;
		else
			meVerAlignState = SVX_VER_JUSTIFY_STANDARD;
	}
	else if( nId == IID_VERT_BOTTOM )
	{
		if(meVerAlignState != SVX_VER_JUSTIFY_BOTTOM)
			meVerAlignState = SVX_VER_JUSTIFY_BOTTOM;
		else
			meVerAlignState = SVX_VER_JUSTIFY_STANDARD;
	}
	SvxVerJustifyItem aVerItem(meVerAlignState, SID_V_ALIGNCELL);
	GetBindings()->GetDispatcher()->Execute(SID_V_ALIGNCELL, SFX_CALLMODE_RECORD, &aVerItem, 0L);
	UpdateVerAlign();
	return 0;
}

//////////////////////////////////////////////////////////////////////////////

IMPL_LINK(AlignmentPropertyPanel, MFLeftIndentMdyHdl, void*, EMPTYARG)
{
	mpCBXWrapText->EnableTriState(false);
	sal_uInt16 nVal = (sal_uInt16)mpMFLeftIndent->GetValue();
	SfxUInt16Item aItem( SID_ATTR_ALIGN_INDENT,  (sal_uInt16)CalcToUnit( nVal,  SFX_MAPUNIT_TWIP ) );

	GetBindings()->GetDispatcher()->Execute(SID_ATTR_ALIGN_INDENT, SFX_CALLMODE_RECORD, &aItem, 0L);
	return( 0L );
}

//////////////////////////////////////////////////////////////////////////////

IMPL_LINK(AlignmentPropertyPanel, CBOXMergnCellClkHdl, void*, EMPTYARG)
{
	bool bState = mpCBXMergeCell->IsChecked();

	//Modified 
	//SfxBoolItem aItem( FID_MERGE_TOGGLE , bState);
	//GetBindings()->GetDispatcher()->Execute(FID_MERGE_TOGGLE, SFX_CALLMODE_RECORD, &aItem, false, 0L);
	if(bState)
		GetBindings()->GetDispatcher()->Execute(FID_MERGE_ON, SFX_CALLMODE_RECORD);
	else
		GetBindings()->GetDispatcher()->Execute(FID_MERGE_OFF, SFX_CALLMODE_RECORD);
	GetBindings()->Invalidate(FID_MERGE_TOGGLE,true,false);	
	//modified end

	return 0;
}

//////////////////////////////////////////////////////////////////////////////

IMPL_LINK(AlignmentPropertyPanel, CBOXWrapTextClkHdl, void*, EMPTYARG)
{
	bool bState = mpCBXWrapText->IsChecked();
	SfxBoolItem aItem( SID_ATTR_ALIGN_LINEBREAK , bState);
	GetBindings()->GetDispatcher()->Execute(SID_ATTR_ALIGN_LINEBREAK, SFX_CALLMODE_RECORD, &aItem, 0L);
	return 0;
}

//////////////////////////////////////////////////////////////////////////////

AlignmentPropertyPanel* AlignmentPropertyPanel::Create (
    Window* pParent,
    const cssu::Reference<css::frame::XFrame>& rxFrame,
    SfxBindings* pBindings)
{
    if (pParent == NULL)
        throw lang::IllegalArgumentException(A2S("no parent Window given to AlignmentPropertyPanel::Create"), NULL, 0);
    if ( ! rxFrame.is())
        throw lang::IllegalArgumentException(A2S("no XFrame given to AlignmentPropertyPanel::Create"), NULL, 1);
    if (pBindings == NULL)
        throw lang::IllegalArgumentException(A2S("no SfxBindings given to AlignmentPropertyPanel::Create"), NULL, 2);
    
    return new AlignmentPropertyPanel(
        pParent,
        rxFrame,
        pBindings);
}

//////////////////////////////////////////////////////////////////////////////

void AlignmentPropertyPanel::DataChanged(
    const DataChangedEvent& rEvent)
{
    (void)rEvent;
}

//////////////////////////////////////////////////////////////////////////////

void AlignmentPropertyPanel::HandleContextChange(
    const ::sfx2::sidebar::EnumContext aContext)
{
    if(maContext == aContext)
    {
        // Nothing to do.
        return;
    }

    maContext = aContext;



    // todo
}

//////////////////////////////////////////////////////////////////////////////

void AlignmentPropertyPanel::NotifyItemUpdate( 
    sal_uInt16 nSID, 
    SfxItemState eState, 
    const SfxPoolItem* pState)
{
	switch(nSID)
	{
	case SID_H_ALIGNCELL:
		if(eState >= SFX_ITEM_DEFAULT && pState && pState->ISA(SvxHorJustifyItem) )
		{
				const SvxHorJustifyItem* pItem = (const SvxHorJustifyItem*)pState;
				meHorAlignState = (SvxCellHorJustify)pItem->GetValue();
		}
		else
		{
			meHorAlignState = SVX_HOR_JUSTIFY_STANDARD;
		}
		UpdateHorAlign();
		break;
	case SID_V_ALIGNCELL:
		if(eState >= SFX_ITEM_DEFAULT && pState && pState->ISA(SvxVerJustifyItem) )
		{
				const SvxVerJustifyItem* pItem = (const SvxVerJustifyItem*)pState;
				meVerAlignState = (SvxCellVerJustify)pItem->GetValue();
		}
		else
		{
			meVerAlignState = SVX_VER_JUSTIFY_STANDARD;
		}
		UpdateVerAlign();
		break;
	case SID_ATTR_ALIGN_INDENT:
		if(eState >= SFX_ITEM_DEFAULT && pState && pState->ISA(SfxUInt16Item) )
		{
				const SfxUInt16Item* pItem = (const SfxUInt16Item*)pState;
				sal_uInt16 nVal = pItem->GetValue();
				mpMFLeftIndent->SetValue( CalcToPoint(nVal, SFX_MAPUNIT_TWIP, 1) );
		}
		else
		{
			mpMFLeftIndent->SetValue(0);
			mpMFLeftIndent->SetText(String());
		}
		break;
	case FID_MERGE_TOGGLE:
		if(eState >= SFX_ITEM_DEFAULT && pState && pState->ISA(SfxBoolItem) )
		{
			mpCBXMergeCell->Enable();
			const SfxBoolItem* pItem = (const SfxBoolItem*)pState;
			bool bVal = pItem->GetValue();
			if(bVal)
				mpCBXMergeCell->Check(true);
			else
				mpCBXMergeCell->Check(false);
		}
		else
		{
			mpCBXMergeCell->Check(false);
			mpCBXMergeCell->Disable();
		}
		break;

	case SID_ATTR_ALIGN_LINEBREAK:
		if(eState == SFX_ITEM_DISABLED)
		{
			mpCBXWrapText->EnableTriState(false);
			mpCBXWrapText->Check(false);
			mpCBXWrapText->Disable();
		}
		else 
		{
			mpCBXWrapText->Enable();
			if(eState >= SFX_ITEM_DEFAULT && pState && pState->ISA(SfxBoolItem) )
			{
				mpCBXWrapText->EnableTriState(false);
				const SfxBoolItem* pItem = (const SfxBoolItem*)pState;
				bool bVal = pItem->GetValue();
				if(bVal)
					mpCBXWrapText->Check(true);
				else
					mpCBXWrapText->Check(false);
			}
			else if(eState == SFX_ITEM_DONTCARE)
			{
				mpCBXWrapText->EnableTriState(true);
				mpCBXWrapText->SetState(STATE_DONTKNOW);
			}
		}
		break;
	case SID_ATTR_ALIGN_DEGREES:
		if (eState >= SFX_ITEM_AVAILABLE)
		{
			long nTmp = ((const SfxInt32Item*)pState)->GetValue(); 
			mpMtrAngle->SetValue( nTmp / 100);	//wj
			mpCtrlDial->SetRotation( nTmp );
			switch(nTmp)
			{
				case 0:
					mpMtrAngle->SelectEntryPos(0);
				break;
				case 4500:
					mpMtrAngle->SelectEntryPos(1);
				break;
				case 9000:
					mpMtrAngle->SelectEntryPos(2);
				break;
				case 13500:
					mpMtrAngle->SelectEntryPos(3);
				break;
				case 18000:
					mpMtrAngle->SelectEntryPos(4);
				break;
				case 22500:
					mpMtrAngle->SelectEntryPos(5);
				break;
				case 27000:
					mpMtrAngle->SelectEntryPos(6);
				break;
				case 31500:
					mpMtrAngle->SelectEntryPos(7);
			}
		}
		else
		{
			mpMtrAngle->SetText( String() );
			mpCtrlDial->SetRotation( 0 );
		}
		break;
	case SID_ATTR_ALIGN_STACKED:
		if (eState >= SFX_ITEM_AVAILABLE)
		{			
			mpCbStacked->EnableTriState(false);
			const SfxBoolItem* aStackItem = (const SfxBoolItem*)pState;
			bool IsChecked = (bool)aStackItem->GetValue();
			if(IsChecked)
			{
				mpCbStacked->Check(IsChecked);
				mpFtRotate->Disable();
				mpMtrAngle->Disable();
				mpCtrlDial->Disable();
				mbMultiDisable = true;
			}
			else
			{
				mpCbStacked->Check(IsChecked);
				mpFtRotate->Enable();
				mpMtrAngle->Enable();
				mpCtrlDial->Enable();
				mbMultiDisable = false;
			}
		}
		else
		{
			mbMultiDisable = true;
			mpFtRotate->Disable();
			mpMtrAngle->Disable();
			mpCtrlDial->Disable();
			mpCbStacked->EnableTriState(true);
			mpCbStacked->SetState(STATE_DONTKNOW);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////

SfxBindings* AlignmentPropertyPanel::GetBindings() 
{ 
    return mpBindings; 
}

//////////////////////////////////////////////////////////////////////////////

void AlignmentPropertyPanel::FormatDegrees(double& dTmp)
{
	while(dTmp<0)
		dTmp += 360;
	while (dTmp > 359)	//modify 
		dTmp = 359;
}

//////////////////////////////////////////////////////////////////////////////

void AlignmentPropertyPanel::UpdateHorAlign()
{
	mpTBHorizontal->SetItemState(ID_SUBSTLEFT,	STATE_NOCHECK);
	mpTBHorizontal->SetItemState(ID_SUBSTCENTER,	STATE_NOCHECK);
	mpTBHorizontal->SetItemState(ID_SUBSTRIGHT,	STATE_NOCHECK);
	mpTBHorizontal->SetItemState(ID_SUBSTJUSTIFY,STATE_NOCHECK);
	mpFTLeftIndent->Disable();
	mpMFLeftIndent->Disable();
	if(meHorAlignState==SVX_HOR_JUSTIFY_REPEAT)
	{
		mpFtRotate->Disable();
		mpCtrlDial->Disable();
		mpMtrAngle->Disable();
		mpCbStacked->Disable();
	}
	else
	{
		if(!mbMultiDisable)
		{
			mpFtRotate->Enable();
			mpCtrlDial->Enable();
			mpMtrAngle->Enable();
		}
		else
		{
			mpFtRotate->Disable();
			mpCtrlDial->Disable();
			mpMtrAngle->Disable();
		}
		mpCbStacked->Enable();
	}
	switch(meHorAlignState)
	{
	case SVX_HOR_JUSTIFY_LEFT:	
		mpTBHorizontal->SetItemState(ID_SUBSTLEFT,	STATE_CHECK);
		mpFTLeftIndent->Enable();
		mpMFLeftIndent->Enable();
		break;
	case SVX_HOR_JUSTIFY_CENTER:mpTBHorizontal->SetItemState(ID_SUBSTCENTER,	STATE_CHECK);break;
	case SVX_HOR_JUSTIFY_RIGHT:	mpTBHorizontal->SetItemState(ID_SUBSTRIGHT,	STATE_CHECK);break;
	case SVX_HOR_JUSTIFY_BLOCK:	mpTBHorizontal->SetItemState(ID_SUBSTJUSTIFY,STATE_CHECK);break;
	default:;
	}
}

//////////////////////////////////////////////////////////////////////////////

void AlignmentPropertyPanel::UpdateVerAlign()
{
	mpTBVertical->SetItemState(IID_VERT_TOP,		STATE_NOCHECK);
	mpTBVertical->SetItemState(IID_VERT_CENTER,	STATE_NOCHECK);
	mpTBVertical->SetItemState(IID_VERT_BOTTOM,	STATE_NOCHECK);
	
	switch(meVerAlignState)
	{
	case SVX_VER_JUSTIFY_TOP:	mpTBVertical->SetItemState(IID_VERT_TOP,		STATE_CHECK);break;
	case SVX_VER_JUSTIFY_CENTER:mpTBVertical->SetItemState(IID_VERT_CENTER,	STATE_CHECK);break;
	case SVX_VER_JUSTIFY_BOTTOM:	mpTBVertical->SetItemState(IID_VERT_BOTTOM,	STATE_CHECK);break;
	default:;
	}
}

//////////////////////////////////////////////////////////////////////////////
// namespace close

}} // end of namespace ::sc::sidebar

//////////////////////////////////////////////////////////////////////////////
// eof
