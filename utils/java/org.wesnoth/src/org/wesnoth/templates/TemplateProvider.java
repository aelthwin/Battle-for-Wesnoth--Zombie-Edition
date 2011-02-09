/*******************************************************************************
 * Copyright (c) 2010 - 2011 by Timotei Dolean <timotei21@gmail.com>
 *
 * This program and the accompanying materials are made available
 * under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/
package org.wesnoth.templates;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.wesnoth.Constants;
import org.wesnoth.Logger;
import org.wesnoth.Messages;
import org.wesnoth.utils.Pair;
import org.wesnoth.utils.StringUtils;


public class TemplateProvider
{
	private static TemplateProvider		instance_;
	private final Map<String, String>	templates_	= new HashMap<String, String>();

	public static TemplateProvider getInstance()
	{
		if (instance_ == null)
		{
			instance_ = new TemplateProvider();
			instance_.loadTemplates();
		}
		return instance_;
	}

	/**
	 * Loads the templates from the file system
	 */
	public void loadTemplates()
	{
		try
		{
			Logger.getInstance().log(Messages.TemplateProvider_0 +
					Constants.PLUGIN_FULL_PATH + Constants.TEMPLATES_FILENAME);

			BufferedReader reader = new BufferedReader(
					new FileReader(Constants.PLUGIN_FULL_PATH + Constants.TEMPLATES_FILENAME));
			BufferedReader tmpReader;
			String line, tmpLine, content;

			// read the main "templatesIndex.txt" file
			while ((line = reader.readLine()) != null)
			{
				// comment
				if (line.startsWith("#") || line.matches("^[\t ]*$")) //$NON-NLS-1$ //$NON-NLS-2$
					continue;

				// 0 - template name | 1 - template file
				String[] tokensStrings = line.split(" "); //$NON-NLS-1$

				if (tokensStrings.length != 2)
					continue;

				content = ""; //$NON-NLS-1$

				if (new File(Constants.PLUGIN_FULL_PATH + tokensStrings[1]).exists())
				{
					tmpReader = new BufferedReader(
							new FileReader(Constants.PLUGIN_FULL_PATH + tokensStrings[1]));
					while ((tmpLine = tmpReader.readLine()) != null)
					{
						content += tmpLine + '\n';
					}
					templates_.put(tokensStrings[0], content);
					tmpReader.close();
				}
			}
			reader.close();
		} catch (Exception e)
		{
			Logger.getInstance().logException(e);
		}
	}

	/**
	 * Gets a string of the processed specified template
	 * @param templateName The name of the template to process
	 * @param parameters The parameters to replace into the template
	 * @return
	 */
	public String getProcessedTemplate(String templateName,
			List<ReplaceableParameter> parameters)
	{
		String tmpTemplate = TemplateProvider.getInstance().getTemplate(templateName);
		if (tmpTemplate == null || parameters == null)
			return null;

		String result = ""; //$NON-NLS-1$
		String[] template = StringUtils.getLines(tmpTemplate);
		boolean skipLine;

		for (int i = 0; i < template.length; ++i)
		{
			skipLine = false;
			for (ReplaceableParameter param : parameters)
			{
				if (template[i].contains(param.paramName))
				{
					template[i] = StringUtils.replaceWithIndent(template[i],
									param.paramName, param.paramValue);

					if (!templateName.equals("build_xml") && //$NON-NLS-1$
						(param.paramValue == null || param.paramValue.isEmpty()))
					{
						// we don't have any value supplied -
						// let's comment that line (if it's not already commented)
						// or remove it if it's empty
						if (!(StringUtils.startsWith(template[i], "#"))) //$NON-NLS-1$
						{
							template[i] = "#" + template[i]; //$NON-NLS-1$
							Pattern pattern = Pattern.compile("#[\t ]*"); //$NON-NLS-1$
							Matcher matcher = pattern.matcher(template[i]);
							if (matcher.matches())
							{
								skipLine = true;
							}
						}
					}
				}
			}

			if (skipLine == false)
				result += template[i] + "\n"; //$NON-NLS-1$
		}
		return result;
	}

	/**
	 * Returns the template with the specified name or empty string if none
	 * @param name
	 * @return
	 */
	public String getTemplate(String name)
	{
		if (templates_.get(name) == null)
			return ""; //$NON-NLS-1$
		return templates_.get(name);
	}

	/**
	 * Gets the lists of the specified structure template. The first return
	 * value is a list of <String, String> that consist of <Filename, Template
	 * used for file contents> and the second return value is a list of String
	 * with directories names
	 *
	 * @param structureTemplate the template
	 * @return
	 */
	public Pair<List<Pair<String, String>>, List<String>> getFilesDirectories(String structureTemplate)
	{
		List<Pair<String, String>> files = new ArrayList<Pair<String, String>>();
		List<String> dirs = new ArrayList<String>();

		for (String line : StringUtils.getLines(structureTemplate))
		{
			if (StringUtils.startsWith(line, "#")) //$NON-NLS-1$
				continue;

			if (line.contains(":")) // file with template //$NON-NLS-1$
			{
				String[] tmpLine = line.split(":"); //$NON-NLS-1$

				// oops. error
				if (tmpLine.length != 2)
				{
					Logger.getInstance().logError(
							String.format(Messages.TemplateProvider_15,
									structureTemplate, line));
					continue;
				}

				files.add(new Pair<String, String>(tmpLine[0].trim(), tmpLine[1].trim()));
			}
			else
			{
				dirs.add(line.trim());
			}
		}

		return new Pair<List<Pair<String, String>>, List<String>>(files, dirs);
	}
}
