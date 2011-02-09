/*******************************************************************************
 * Copyright (c) 2010 - 2011 by Timotei Dolean <timotei21@gmail.com>
 *
 * This program and the accompanying materials are made available
 * under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/
package org.wesnoth.preprocessor;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.wesnoth.Logger;
import org.wesnoth.Messages;
import org.wesnoth.utils.ResourceUtils;


/**
 * This represents a WML preprocessor define:
 * [preproc_define]
 *		name = ""
 *		value = ""
 *		textdomain = ""
 *		linenum = ""
 *		location = ""
 * [/preproc_define]
 */
public class Define
{
	private String name_;
	private String value_;
	private String textdomain_;
	private int lineNum_;
	private String location_;
	private List<String> args_;

	public Define(String name, String value, String textdomain,
				int linenum, String location, List<String> args)
	{
		name_ = name;
		value_ = value;
		textdomain_ = textdomain;
		lineNum_ = linenum;
		location_ = location;
		args_ = args;

		// ensure no NullPointerException exists
		if (args_ == null)
			args_ = new ArrayList<String>();
	}

	public int getLineNum()
	{
		return lineNum_;
	}
	public String getLocation()
	{
		return location_;
	}
	public String getName()
	{
		return name_;
	}
	public String getTextdomain()
	{
		return textdomain_;
	}
	public String getValue()
	{
		return value_;
	}

	/**
	 * Gets the arguments of this macro
	 * @return
	 */
	public List<String> getArguments()
	{
		return args_;
	}

	/**
	 * Reads the defines from the specified file
	 * @param file
	 * @return
	 */
	public static Map<String, Define> readDefines(String file)
	{
		DefinesSAXHandler handler = (DefinesSAXHandler) ResourceUtils.
				getWMLSAXHandlerFromResource(file, new DefinesSAXHandler());

		Logger.getInstance().log(Messages.Define_0 + handler.getDefines().size() + Messages.Define_1 + file);
		return handler.getDefines();
	}

	/**
	 * Returns a string containing the current define, formatted
	 * @return
	 */
	//TODO: finish this
	public static String toStringFormatted()
	{
		StringBuilder res = new StringBuilder();
		res.append("[preproc_define]"); //$NON-NLS-1$
		//res.append("\tname=\"\"");
		res.append("[/preproc_define]"); //$NON-NLS-1$
		return res.toString();
	}
}
