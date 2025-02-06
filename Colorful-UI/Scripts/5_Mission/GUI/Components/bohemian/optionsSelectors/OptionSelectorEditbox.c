modded class OptionSelectorEditbox extends OptionSelectorBase
{
	override void ColorHighlight( Widget w )
	{
		if( !w )
			return;
		
		if ( m_EditBox )
		{
			SetFocus( m_EditBox );
			m_EditBox.SetColor(UIColor.SemiTransWhite());
		}
		
		super.ColorHighlight( w );
	}
	
	override void ColorNormal( Widget w )
	{
		if( !w )
			return;
		
		if ( m_EditBox )
		{
			m_EditBox.SetColor(UIColor.SemiTransWhite());
		}
		
		super.ColorNormal( w );
	}
}