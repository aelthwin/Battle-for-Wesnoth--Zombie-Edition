/*******************************************************************************
 * Copyright (c) 2010 - 2011 by Timotei Dolean <timotei21@gmail.com>
 *
 * This program and the accompanying materials are made available
 * under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/
package org.wesnoth.ui;

import org.eclipse.core.resources.IFile;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.xtext.parsetree.AbstractNode;
import org.eclipse.xtext.parsetree.CompositeNode;
import org.eclipse.xtext.parsetree.LeafNode;
import org.eclipse.xtext.parsetree.NodeUtil;
import org.eclipse.xtext.ui.editor.utils.EditorUtils;

public class WMLUtil
{
	public static String debug(EObject root)
	{
		CompositeNode node = NodeUtil.getNode(root);
		Iterable<AbstractNode> contents = NodeUtil.getAllContents(node);
		StringBuffer text = new StringBuffer();
		for (AbstractNode abstractNode : contents)
		{
			if (abstractNode instanceof LeafNode)
			{
				System.out.println((((LeafNode) abstractNode).getText()));
				text.append(((LeafNode) abstractNode).getText());
			}
		}
		return text.toString();
	}

	/**
	 * Gets current edited file
	 * @return
	 */
	public static IFile getActiveEditorFile()
	{
		if (EditorUtils.getActiveXtextEditor() == null)
			return null;
		return (IFile)EditorUtils.getActiveXtextEditor()
					.getEditorInput().getAdapter(IFile.class);
	}
}
