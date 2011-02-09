/*******************************************************************************
 * Copyright (c) 2010 - 2011 by Timotei Dolean <timotei21@gmail.com>
 *
 * This program and the accompanying materials are made available
 * under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/
package org.wesnoth.action;

import org.eclipse.jface.action.IAction;
import org.wesnoth.utils.MapUtils;


public class ImportMap extends ObjectActionDelegate
{
	/**
	 * This method is runned *ONLY* if the user selected a "maps" folder
	 */
	@Override
	public void run(IAction action)
	{
		MapUtils.importMap();
	}
}
