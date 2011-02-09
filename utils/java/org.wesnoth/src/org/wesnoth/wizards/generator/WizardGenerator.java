/*******************************************************************************
 * Copyright (c) 2010 - 2011 by Timotei Dolean <timotei21@gmail.com>
 *
 * This program and the accompanying materials are made available
 * under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/
package org.wesnoth.wizards.generator;

import org.eclipse.jface.wizard.IWizardPage;
import org.wesnoth.Constants;
import org.wesnoth.schema.SchemaParser;
import org.wesnoth.schema.Tag;
import org.wesnoth.utils.StringUtils;
import org.wesnoth.wizards.NewWizardTemplate;


public class WizardGenerator extends NewWizardTemplate
{
	private String	tagName_;
	private byte	indent_;

	public WizardGenerator(String title, String tagName, byte indent) {
		SchemaParser.getInstance().parseSchema(false);
		setWindowTitle(title);
		Tag tagContent = SchemaParser.getInstance().getTags().get(tagName);

		tagName_ = tagName;
		indent_ = indent;
		if (tagContent == null)
			addPage(new WizardGeneratorPage404(tagName));
		else
		{
			// keys section
			int keysNr = tagContent.getKeyChildren().size();
			int startKey = 0, pgsKey = (keysNr / Constants.WIZ_MaxTextBoxesOnPage);
			WizardGeneratorPageKey tempPageKey;
			for (int i = 0; i < pgsKey; i++)
			{
				tempPageKey = new WizardGeneratorPageKey(tagName, tagContent.getKeyChildren(), startKey,
									startKey + Constants.WIZ_MaxTextBoxesOnPage, (byte) (indent_ + 1));
				startKey += Constants.WIZ_MaxTextBoxesOnPage;
				addPage(tempPageKey);
			}
			if (keysNr - 1 > 0)
			{
				tempPageKey = new WizardGeneratorPageKey(tagName, tagContent.getKeyChildren(),
												startKey, keysNr - 1, (byte) (indent_ + 1));
				addPage(tempPageKey);
			}

			// tags section
			int tagsNr = tagContent.getTagChildren().size();
			int startTag = 0, pgsTag = (tagsNr / Constants.WIZ_MaxGroupsOnPage);
			WizardGeneratorPageTag tempPageTag;
			for (int i = 0; i < pgsTag; i++)
			{
				tempPageTag = new WizardGeneratorPageTag(tagName, tagContent.getTagChildren(), startTag,
										startTag + Constants.WIZ_MaxGroupsOnPage, (byte) (indent_ + 1));
				startTag += Constants.WIZ_MaxTextBoxesOnPage;
				addPage(tempPageTag);
			}
			if (tagsNr - 1 > 0)
			{
				tempPageTag = new WizardGeneratorPageTag(tagName, tagContent.getTagChildren(),
											startTag, tagsNr - 1, (byte) (indent_ + 1));
				addPage(tempPageTag);
			}

			if (getPageCount() == 0)
			{
				addPage(new WizardGeneratorPage404(tagName));
			}
		}
	}

	public byte getIndent()
	{
		return indent_;
	}

	@Override
	public void addPages()
	{
		super.addPages();
	}

	@Override
	public boolean performFinish()
	{
		// logic
		String result = StringUtils.multiples("\t", indent_) + "[" + tagName_ + "]\n"; //$NON-NLS-1$ //$NON-NLS-2$ //$NON-NLS-3$
		StringBuilder keys = new StringBuilder();
		StringBuilder tags = new StringBuilder();
		for (IWizardPage page : getPages())
		{
			if (page instanceof WizardGeneratorPageKey)
				keys.append(((WizardGeneratorPageKey) page).getContent());
			else if (page instanceof WizardGeneratorPageTag)
				tags.append(((WizardGeneratorPageTag) page).getContent());
			else
				; // skip 404 pages
		}
		result += (keys.toString() + tags.toString());
		result += (StringUtils.multiples("\t", indent_) + "[/" + tagName_ + "]\n"); //$NON-NLS-1$ //$NON-NLS-2$ //$NON-NLS-3$
		data_ = result;
		// for now let's just return tag's name
		objectName_ = tagName_;
		isFinished_ = true;
		return true;
	}
}
