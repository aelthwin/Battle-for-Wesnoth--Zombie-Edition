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
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Stack;

import org.xml.sax.Attributes;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.DefaultHandler;

/**
 * A sax handler that parses files that contain [preproc_define]s
 */
public class DefinesSAXHandler extends DefaultHandler
{
	private static Stack<String> stack_;
	private Map<String, Define> defines_;

	// indexes for different define properties
	private String name_;
	private String value_;
	private String textdomain_;
	private int linenum_;
	private String location_;
	private List<String> arguments_;

	public DefinesSAXHandler()
	{
		stack_ = new Stack<String>();
		defines_ = new HashMap<String, Define>();
		arguments_ = new ArrayList<String>();
	}

	@Override
	public void startElement(String uri, String localName, String qName,
			Attributes attributes) throws SAXException
	{
		super.startElement(uri, localName, qName, attributes);
		stack_.push(qName);
	}

	@Override
	public void endElement(String uri, String localName, String qName)
			throws SAXException
	{
		super.endElement(uri, localName, qName);
		stack_.pop();

		if (qName.equals("preproc_define")) //$NON-NLS-1$
		{
			// create the define
			defines_.put(name_, new Define(name_, value_, textdomain_,
					linenum_, location_, arguments_));
			// reset values
			resetValues();
		}
	}

	@Override
	public void characters(char[] ch, int start, int length)
			throws SAXException
	{
		super.characters(ch, start, length);
		if (stack_.isEmpty())
			return;
		String element = stack_.peek();

		if (element.equals("name")) //$NON-NLS-1$
		{
			// we have name at: 1 - preproc_define, 2 - argument
			if (stack_.get(stack_.size() - 2).equals("argument")) //$NON-NLS-1$
			{
				arguments_.add(new String(ch, start, length));
			}
			else
			{
				name_ = new String(ch, start, length);
			}
		}
		else if (element.equals("value")) //$NON-NLS-1$
		{
			value_ = new String(ch, start, length);
		}
		else if (element.equals("textdomain")) //$NON-NLS-1$
		{
			textdomain_ = new String(ch, start, length);
		}
		else if (element.equals("linenum")) //$NON-NLS-1$
		{
			linenum_ = Integer.valueOf(new String(ch, start, length));
		}
		else if (element.equals("location")) //$NON-NLS-1$
		{
			location_ = new String(ch, start, length);
		}
	}

	/**
	 * resets indexes for to be used by the next define
	 */
	private void resetValues()
	{
		name_ = ""; //$NON-NLS-1$
		value_ = ""; //$NON-NLS-1$
		linenum_ = 0;
		location_ = ""; //$NON-NLS-1$
		textdomain_ = ""; //$NON-NLS-1$
		arguments_ = new ArrayList<String>();
	}

	/**
	 * Gets the map of defines parsed
	 * @return
	 */
	public Map<String, Define> getDefines()
	{
		return defines_;
	}
}
