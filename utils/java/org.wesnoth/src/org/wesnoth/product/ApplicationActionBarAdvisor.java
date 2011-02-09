/*******************************************************************************
 * Copyright (c) 2010 - 2011 by Timotei Dolean <timotei21@gmail.com>
 *
 * This program and the accompanying materials are made available
 * under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/
package org.wesnoth.product;

import org.eclipse.jface.action.GroupMarker;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.ui.IWorkbenchActionConstants;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.actions.ActionFactory;
import org.eclipse.ui.actions.ActionFactory.IWorkbenchAction;
import org.eclipse.ui.application.ActionBarAdvisor;
import org.eclipse.ui.application.IActionBarConfigurer;
import org.wesnoth.Messages;

public class ApplicationActionBarAdvisor extends ActionBarAdvisor
{
	private IWorkbenchAction	aboutAction_;
	private IWorkbenchAction	quitAction_;
	private IWorkbenchAction	prefAction_;
	private IWorkbenchAction	newWizardDropDownAction_;

	public ApplicationActionBarAdvisor(IActionBarConfigurer configurer)
	{
		super(configurer);
	}

	@Override
	protected void makeActions(IWorkbenchWindow window)
	{
		// file menu
		newWizardDropDownAction_ = ActionFactory.NEW_WIZARD_DROP_DOWN.create(window);
		register(newWizardDropDownAction_);
		quitAction_ = ActionFactory.QUIT.create(window);
		register(quitAction_);

		// window menu
		prefAction_ = ActionFactory.PREFERENCES.create(window);
		register(prefAction_);

		// help menu
		aboutAction_ = ActionFactory.ABOUT.create(window);
		register(aboutAction_);
	}

	@Override
	protected void fillMenuBar(IMenuManager menuBar)
	{
		MenuManager fileMenu = new MenuManager(Messages.ApplicationActionBarAdvisor_0, "wesnoth.file"); //$NON-NLS-1$
		fileMenu.add(newWizardDropDownAction_);
		fileMenu.add(quitAction_);
		menuBar.add(fileMenu);

		MenuManager editMenu = new MenuManager(Messages.ApplicationActionBarAdvisor_2, IWorkbenchActionConstants.M_EDIT);
		menuBar.add(editMenu);

		// wesnoth
		menuBar.add(new GroupMarker(IWorkbenchActionConstants.MB_ADDITIONS));

		MenuManager projectMenu = new MenuManager(Messages.ApplicationActionBarAdvisor_3, IWorkbenchActionConstants.M_PROJECT);
		menuBar.add(projectMenu);

		MenuManager windowMenu = new MenuManager(Messages.ApplicationActionBarAdvisor_4, IWorkbenchActionConstants.M_WINDOW);
		windowMenu.add(prefAction_);
		menuBar.add(windowMenu);

		MenuManager helpMenu = new MenuManager(Messages.ApplicationActionBarAdvisor_5, "help"); //$NON-NLS-1$
		helpMenu.add(aboutAction_);
		menuBar.add(helpMenu);
	}
}
