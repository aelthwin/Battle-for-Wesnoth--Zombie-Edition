/*******************************************************************************
 * Copyright (c) 2010 - 2011 by Timotei Dolean <timotei21@gmail.com>
 *
 * This program and the accompanying materials are made available
 * under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/
package org.wesnoth.wizards.campaign;

import java.lang.reflect.InvocationTargetException;
import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IProjectDescription;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.jface.operation.IRunnableWithProgress;
import org.wesnoth.Logger;
import org.wesnoth.Messages;
import org.wesnoth.templates.ReplaceableParameter;
import org.wesnoth.templates.TemplateProvider;
import org.wesnoth.utils.Pair;
import org.wesnoth.utils.ProjectUtils;
import org.wesnoth.utils.ResourceUtils;
import org.wesnoth.wizards.NewWizardTemplate;


public class CampaignNewWizard extends NewWizardTemplate
{
	protected CampaignPage0 page0_;
	protected CampaignPage1 page1_;
	protected CampaignPage2 page2_;

	@Override
	public void addPages()
	{
		page0_ = new CampaignPage0();
		addPage(page0_);

		page1_ = new CampaignPage1();
		addPage(page1_);

		page2_ = new CampaignPage2();
		addPage(page2_);

		super.addPages();
	}

	public CampaignNewWizard() {
		setWindowTitle(Messages.CampaignNewWizard_0);
		setNeedsProgressMonitor(true);
	}

	@Override
	public boolean performFinish()
	{
		try
		{
			// let's create the project
			getContainer().run(false, false, new IRunnableWithProgress() {
				@Override
				public void run(IProgressMonitor monitor) throws InvocationTargetException, InterruptedException
				{
					createProject(monitor);
					monitor.done();
				}
			});
		} catch (Exception e)
		{
			Logger.getInstance().logException(e);
			return false;
		}
		return true;
	}

	public void createProject(IProgressMonitor monitor)
	{
		monitor.beginTask(Messages.CampaignNewWizard_1, 15);

		try
		{
			IProject currentProject = page0_.getProjectHandle();

			// the project
			if (page0_.getLocationPath().equals(ResourcesPlugin.getWorkspace().getRoot().getLocation()))
			{
				ProjectUtils.createWesnothProject(currentProject, null,
						true, !page1_.isDataCampaignsProject(), monitor);
			}
			else
			{
				IProjectDescription newDescription = ResourcesPlugin.getWorkspace().
				newProjectDescription(page0_.getProjectName());
				newDescription.setLocation(page0_.getLocationPath());
				ProjectUtils.createWesnothProject(currentProject, newDescription,
						true, !page1_.isDataCampaignsProject(), monitor);
			}
			monitor.worked(2);

			String campaignStructure = prepareTemplate("campaign_structure"); //$NON-NLS-1$
			if (campaignStructure == null)
				return;

			List<Pair<String, String>> files;
			List<String> dirs;
			Pair<List<Pair<String, String>>, List<String>> tmp = TemplateProvider.getInstance().getFilesDirectories(campaignStructure);
			files = tmp.First;
			dirs = tmp.Second;

			for (Pair<String, String> file : files)
			{
				if (file.Second.equals("pbl") && //$NON-NLS-1$
					page1_.getGeneratePBLFile() == false)
					continue;
				if (file.Second.equals("build_xml") && //$NON-NLS-1$
					page1_.isDataCampaignsProject())
					continue;

				ResourceUtils.createFile(currentProject, file.First, prepareTemplate(file.Second), true);
				monitor.worked(1);
			}
			for (String dir : dirs)
			{
				ResourceUtils.createFolder(currentProject, dir);
				monitor.worked(1);
			}

			// store some campaign-related info
			ProjectUtils.getPropertiesForProject(currentProject).put("difficulties", page2_.getDifficulties()); //$NON-NLS-1$
			ProjectUtils.saveCacheForProject(currentProject);
		} catch (CoreException e)
		{
			Logger.getInstance().logException(e);
		}

		monitor.done();
	}

	private String prepareTemplate(String templateName)
	{
		ArrayList<ReplaceableParameter> params = new ArrayList<ReplaceableParameter>();

		params.add(new ReplaceableParameter("$$campaign_name", page1_.getCampaignName())); //$NON-NLS-1$
		params.add(new ReplaceableParameter("$$author", page1_.getAuthor())); //$NON-NLS-1$
		params.add(new ReplaceableParameter("$$version", page1_.getVersion())); //$NON-NLS-1$
		params.add(new ReplaceableParameter("$$description", page1_.getCampaignDescription())); //$NON-NLS-1$
		params.add(new ReplaceableParameter("$$icon", page1_.getIconPath())); //$NON-NLS-1$
		params.add(new ReplaceableParameter("$$email", page1_.getEmail())); //$NON-NLS-1$
		params.add(new ReplaceableParameter("$$passphrase", page1_.getPassphrase())); //$NON-NLS-1$
		params.add(new ReplaceableParameter("$$translations_dir", page1_.getTranslationDir())); //$NON-NLS-1$

		params.add(new ReplaceableParameter("$$campaign_id", page2_.getCampaignId())); //$NON-NLS-1$
		params.add(new ReplaceableParameter("$$abrev", page2_.getAbbrev())); //$NON-NLS-1$
		params.add(new ReplaceableParameter("$$define", page2_.getDefine())); //$NON-NLS-1$
		params.add(new ReplaceableParameter("$$difficulties", page2_.getDifficulties())); //$NON-NLS-1$
		params.add(new ReplaceableParameter("$$first_scenario", page2_.getFirstScenario())); //$NON-NLS-1$

		params.add(new ReplaceableParameter("$$project_name", page0_.getProjectName())); //$NON-NLS-1$
		params.add(new ReplaceableParameter("$$project_dir_name", page0_.getProjectName())); //$NON-NLS-1$
		params.add(new ReplaceableParameter("$$type", page1_.isMultiplayer() ? "campaign_mp" : "campaign")); //$NON-NLS-1$ //$NON-NLS-2$ //$NON-NLS-3$

		return TemplateProvider.getInstance().getProcessedTemplate(templateName, params);
	}
}
