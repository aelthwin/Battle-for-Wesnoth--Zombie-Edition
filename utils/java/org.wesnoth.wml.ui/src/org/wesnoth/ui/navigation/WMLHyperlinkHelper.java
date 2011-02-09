/*******************************************************************************
 * Copyright (c) 2010 - 2011 by Timotei Dolean <timotei21@gmail.com>
 *
 * This program and the accompanying materials are made available
 * under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/
package org.wesnoth.ui.navigation;

import org.eclipse.core.resources.IFile;
import org.eclipse.emf.ecore.util.EcoreUtil;
import org.eclipse.jface.text.Region;
import org.eclipse.xtext.parsetree.CompositeNode;
import org.eclipse.xtext.parsetree.LeafNode;
import org.eclipse.xtext.parsetree.NodeUtil;
import org.eclipse.xtext.resource.XtextResource;
import org.eclipse.xtext.ui.editor.hyperlinking.HyperlinkHelper;
import org.eclipse.xtext.ui.editor.hyperlinking.IHyperlinkAcceptor;
import org.wesnoth.Constants;
import org.wesnoth.Logger;
import org.wesnoth.preferences.Preferences;
import org.wesnoth.preprocessor.Define;
import org.wesnoth.ui.Messages;
import org.wesnoth.ui.WMLUtil;
import org.wesnoth.ui.emf.ObjectStorageAdapter;
import org.wesnoth.utils.ProjectUtils;
import org.wesnoth.wml.WMLMacroCall;


public class WMLHyperlinkHelper extends HyperlinkHelper
{
	@Override
	public void createHyperlinksByOffset(XtextResource resource, int offset,
			IHyperlinkAcceptor acceptor)
	{
		super.createHyperlinksByOffset(resource, offset, acceptor);
		CompositeNode rootNode = NodeUtil.getRootNode(resource.getContents().get(0));
		if (rootNode == null)
			return;

		LeafNode node = (LeafNode)NodeUtil.findLeafNodeAtOffset(rootNode, offset);
		if (node == null)
			return;

		createMacroHyperlink(node, acceptor, resource);
		LeafNode prevNode = (LeafNode)NodeUtil.findLeafNodeAtOffset(rootNode,
									node.getOffset() - 1);
		if(prevNode == null)
			return;

		createMapHyperlink(prevNode, node, acceptor, resource);
	}

	/**
	 * Creates a hyperlink for opening the macro definition
	 * @param prevNode
	 * @param node
	 * @param acceptor
	 * @param resource
	  */
	private void createMacroHyperlink(LeafNode node,
			IHyperlinkAcceptor acceptor, XtextResource resource)
	{
		if (node.eContainer() == null ||
			(node.eContainer() instanceof CompositeNode) == false)
			return;

		CompositeNode container = (CompositeNode)node.eContainer();
		if ((container.getElement() instanceof WMLMacroCall) == false)
			return;

		WMLMacroCall macro = (WMLMacroCall)container.getElement();


		IFile file = WMLUtil.getActiveEditorFile();
		if (file == null)
		{
			Logger.getInstance().logError(Messages.WMLHyperlinkHelper_0);
			return;
		}

		// get the define for the macro
		Define define = ProjectUtils.getCacheForProject(file.getProject()).getDefines().get(macro.getName());
		if (define == null)
		{
			//TODO: handle macro include call - open folder?
			//Logger.getInstance().log("No macro with that name found.");
			return;
		}

		if (define.getLocation().isEmpty() == true)
			return;

		String filePath = define.getLocation().split(" ")[0]; //$NON-NLS-1$

		if (filePath.startsWith("~")) // user addon relative location //$NON-NLS-1$
			filePath = filePath.replaceFirst("~", //$NON-NLS-1$
					Preferences.getString(Constants.P_WESNOTH_USER_DIR).replace('\\', '/') +
					"/data/"); //$NON-NLS-1$
		else if (filePath.startsWith("core")) // data/core relative location //$NON-NLS-1$
			filePath = filePath.replaceFirst("core", //$NON-NLS-1$
					Preferences.getString(Constants.P_WESNOTH_WORKING_DIR).replace('\\', '/') +
					"/data/core/"); //$NON-NLS-1$

		FileLocationOpenerHyperlink macroTarget = new FileLocationOpenerHyperlink();
		macroTarget.setHyperlinkRegion(new Region(container.getOffset(), container.getLength()));
		macroTarget.setFilePath(filePath);
		macroTarget.setLinenumber(define.getLineNum());
		acceptor.accept(macroTarget);
	}

	/**
	 * Creates a hyperlink for opening the map.
	 * @param key The key (must me 'map_data' in this case)
	 * @param value The value of key, that is, the location of the map
	 */
	private void createMapHyperlink(LeafNode key, LeafNode value,
			IHyperlinkAcceptor acceptor, XtextResource resource)
	{
		if (!(key.getText().equals("map_data"))) //$NON-NLS-1$
			return;

		// trim the " and the { (if any exist)
		String mapLocation = value.getText();
		if (mapLocation.startsWith("\"")) //$NON-NLS-1$
			mapLocation = mapLocation.substring(1, value.getLength() - 1);
		if (mapLocation.startsWith("{")) //$NON-NLS-1$
			mapLocation = mapLocation.substring(1, value.getLength() - 1);

		mapLocation = mapLocation.replaceFirst("~", //$NON-NLS-1$
				Preferences.getString(Constants.P_WESNOTH_USER_DIR).replace('\\','/') +
				"/data/"); //$NON-NLS-1$

		ObjectStorageAdapter adapter = (ObjectStorageAdapter)EcoreUtil.getAdapter(value.eAdapters(),
				ObjectStorageAdapter.class);
		if (adapter == null)
		{
			adapter = new ObjectStorageAdapter(mapLocation);
			value.eAdapters().add(adapter);
		}
		else
			adapter.setObject(mapLocation);

		MapOpenerHyperlink hyperlink = new MapOpenerHyperlink();
		hyperlink.setHyperlinkRegion(new Region(value.getOffset(), value.getLength()));
		hyperlink.setLocation(mapLocation);
		acceptor.accept(hyperlink);
	}
}
