/*******************************************************************************
 * Copyright (c) 2010 - 2011 by Timotei Dolean <timotei21@gmail.com>
 *
 * This program and the accompanying materials are made available
 * under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/
package org.wesnoth.utils;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.window.Window;
import org.eclipse.jface.wizard.WizardDialog;
import org.eclipse.swt.widgets.Shell;
import org.wesnoth.Activator;
import org.wesnoth.wizards.NewWizardTemplate;


public class WizardUtils
{
	/**
	 * Launches a new wizard
	 * @param wizard The wizard to launch
	 * @param shell The shell
	 * @param selection The current selection
	 * @return
	 */
	public static int launchWizard(NewWizardTemplate wizard, Shell shell,
					IStructuredSelection selection)
	{
		if (wizard == null)
			return Window.CANCEL;

		wizard.init(Activator.getDefault().getWorkbench(), selection);
		wizard.setForcePreviousAndNextButtons(true);

		WizardDialog wizardDialog = new WizardDialog(shell, wizard);
		wizardDialog.create();
		int x = shell.getBounds().x, y = shell.getBounds().y;
		x += ((shell.getBounds().width - wizardDialog.getShell().getBounds().width)/2);
		y += ((shell.getBounds().height - wizardDialog.getShell().getBounds().height)/2);
		wizardDialog.getShell().setLocation(x, y);
		wizardDialog.setHelpAvailable(true);
		Activator.getDefault().getWorkbench().getHelpSystem().setHelp(wizardDialog.getShell(),
				"Wesnoth_Eclipse_Plugin.wizardHelp"); //$NON-NLS-1$

		return wizardDialog.open();
	}
}
