/*******************************************************************************
 * Copyright (c) 2010 - 2011 by Timotei Dolean <timotei21@gmail.com>
 *
 * This program and the accompanying materials are made available
 * under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/
package org.wesnoth;

import java.util.Map.Entry;

import org.eclipse.core.resources.IProject;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.osgi.framework.BundleContext;
import org.wesnoth.preferences.Preferences;
import org.wesnoth.utils.GUIUtils;
import org.wesnoth.utils.PreprocessorUtils;
import org.wesnoth.utils.ProjectCache;
import org.wesnoth.utils.ProjectUtils;
import org.wesnoth.utils.WorkspaceUtils;


/**
 * The activator class controls the plug-in life cycle
 */
public class Activator extends AbstractUIPlugin
{
	// The shared instance
	private static Activator	plugin;
	// a switch for knowing if we already started checking conditions
	private static boolean checkingConditions = false;

	/**
	 * The constructor
	 */
	public Activator() {
	}

	@Override
	public void start(BundleContext context) throws Exception
	{
		super.start(context);
		plugin = this;
		Logger.getInstance().startLogger();
	}

	@Override
	public void stop(BundleContext context) throws Exception
	{
		// save the caches of the projects on disk
		for(Entry<IProject, ProjectCache> cache :
					ProjectUtils.getProjectCaches().entrySet())
		{
			cache.getValue().saveCache();
		}
		PreprocessorUtils.getInstance().saveTimestamps();

		Logger.getInstance().stopLogger();

		plugin = null;
		super.stop(context);
	}

	/**
	 * Returns the shared instance
	 *
	 * @return the shared instance
	 */
	public static Activator getDefault()
	{
		if (checkingConditions == false &&
			PlatformUI.isWorkbenchRunning())
		{
			if (!checkConditions())
			{
				GUIUtils.showInfoMessageBox(
						Messages.Activator_0 +
						Messages.Activator_1 +
						Messages.Activator_2 +
						Messages.Activator_3 +
						Messages.Activator_4 +
				Messages.Activator_5);
				WorkspaceUtils.setupWorkspace(true);
			}
		}
		return plugin;
	}

	/**
	 * Returns an image descriptor for the image file at the given plug-in
	 * relative path
	 *
	 * @param path the path
	 * @return the image descriptor
	 */
	public static ImageDescriptor getImageDescriptor(String path)
	{
		return imageDescriptorFromPlugin(Constants.PLUGIN_ID, path);
	}

	/**
	 * Returns the plugin's shell
	 *
	 * @return
	 */
	public static Shell getShell()
	{
		return plugin.getWorkbench().getDisplay().getActiveShell();
	}

	/**
	 * Checks if the user has set some needed preferences and they are valid
	 */
	public static boolean checkConditions()
	{
		checkingConditions = true;
		String execDir = Preferences.getString(Constants.P_WESNOTH_EXEC_PATH);
		String userDir = Preferences.getString(Constants.P_WESNOTH_USER_DIR);
		String wmltoolsDir = Preferences.getString(Constants.P_WESNOTH_WMLTOOLS_DIR);
		String workingDir = Preferences.getString(Constants.P_WESNOTH_WORKING_DIR);

		if (!WorkspaceUtils.validPath(execDir) || !WorkspaceUtils.validPath(userDir) ||
			!WorkspaceUtils.validPath(wmltoolsDir) || !WorkspaceUtils.validPath(workingDir))
		{
			return false;
		}
		return true;
	}

}
