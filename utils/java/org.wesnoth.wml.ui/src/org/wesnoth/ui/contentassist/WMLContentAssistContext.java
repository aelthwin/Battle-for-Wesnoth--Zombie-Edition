/*******************************************************************************
 * Copyright (c) 2010 - 2011 by Timotei Dolean <timotei21@gmail.com>
 *
 * This program and the accompanying materials are made available
 * under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/
package org.wesnoth.ui.contentassist;

import org.eclipse.xtext.ui.editor.contentassist.ContentAssistContext;

import com.google.inject.Inject;

public class WMLContentAssistContext extends ContentAssistContext
{
	@Inject
	public WMLContentAssistContext()
	{
		super();
	}
}
