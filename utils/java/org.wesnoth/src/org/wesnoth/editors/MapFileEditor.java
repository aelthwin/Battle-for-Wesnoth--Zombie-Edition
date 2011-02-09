package org.wesnoth.editors;

import org.eclipse.core.runtime.IPath;
import org.eclipse.ui.IEditorLauncher;
import org.wesnoth.utils.GameUtils;


public class MapFileEditor implements IEditorLauncher
{
	@Override
	public void open(IPath file)
	{
		GameUtils.startEditor(file.toOSString());
	}
}
