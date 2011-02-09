/*******************************************************************************
 * Copyright (c) 2010 - 2011 by Timotei Dolean <timotei21@gmail.com>
 *
 * This program and the accompanying materials are made available
 * under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/
package org.wesnoth.wizards.generator;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.wesnoth.Messages;
import org.wesnoth.wizards.NewWizardPageTemplate;


public class WizardGeneratorPage404 extends NewWizardPageTemplate
{
	public WizardGeneratorPage404(String tag) {
		super("wizardGeneratorPage404"); //$NON-NLS-1$
		setErrorMessage(String.format(Messages.WizardGeneratorPage404_1,tag));
		setTitle(Messages.WizardGeneratorPage404_3);
		setDescription(Messages.WizardGeneratorPage404_4);
	}

	@Override
	public void createControl(Composite parent)
	{
		super.createControl(parent);
		Composite container = new Composite(parent, SWT.NULL);

		setControl(container);

		Font font = new Font(Display.getDefault().getSystemFont().getDevice(),
				Display.getDefault().getSystemFont().getFontData()[0].getName(), 20, SWT.NORMAL);
		Label lblThisIsSooo = new Label(container, SWT.WRAP);
		lblThisIsSooo.setFont(font);
		lblThisIsSooo.setBounds(10, 89, 554, 137);
		lblThisIsSooo.setText(Messages.WizardGeneratorPage404_5);
	}
}
