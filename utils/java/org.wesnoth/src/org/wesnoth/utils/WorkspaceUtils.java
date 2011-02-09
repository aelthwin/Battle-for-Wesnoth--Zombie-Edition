/*******************************************************************************
 * Copyright (c) 2010 - 2011 by Timotei Dolean <timotei21@gmail.com>
 *
 * This program and the accompanying materials are made available
 * under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/
package org.wesnoth.utils;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Date;
import java.util.List;

import org.eclipse.core.resources.FileInfoMatcherDescription;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IProjectDescription;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceFilterDescription;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.resources.WorkspaceJob;
import org.eclipse.core.runtime.AssertionFailedException;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.NullProgressMonitor;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.preference.PreferenceDialog;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.window.Window;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.IWorkingSet;
import org.eclipse.ui.IWorkingSetManager;
import org.eclipse.ui.dialogs.PreferencesUtil;
import org.wesnoth.Activator;
import org.wesnoth.Constants;
import org.wesnoth.Logger;
import org.wesnoth.Messages;
import org.wesnoth.preferences.Preferences;


public class WorkspaceUtils
{
	private static String	temporaryFolder_	= ""; //$NON-NLS-1$

	/**
	 * Gets the selected project or or null if none selected
	 * @return
	 */
	public static IProject getSelectedProject()
	{
		return getSelectedProject(WorkspaceUtils.getWorkbenchWindow());
	}

	/**
	 * Gets the selected project or null if none selected
	 * @param window The workbench windows from where to get the current selection
	 * @return
	 */
	public static IProject getSelectedProject(IWorkbenchWindow window)
	{
		IStructuredSelection selection = getSelectedStructuredSelection(window);
		if (selection == null || !(selection.getFirstElement() instanceof IProject))
			return null;

		return (IProject) selection.getFirstElement();
	}

	/**
	 * Gets the project from the workspace that has the specified name,
	 * or null if none existing
	 * @return
	 */
	public static IProject getProject(String name)
	{
		IProject proj = ResourcesPlugin.getWorkspace().getRoot().getProject(name);
		if (proj.exists())
			return proj;
		return null;
	}

	/**
	 * Gets the selected folder or null if none selected
	 * @return
	 */
	public static IFolder getSelectedFolder()
	{
		return getSelectedFolder(WorkspaceUtils.getWorkbenchWindow());
	}

	/**
	 * Gets the selected project or null if none selected
	 * @param window The workbench window from where to get the current selection
	 * @return
	 */
	public static IFolder getSelectedFolder(IWorkbenchWindow window)
	{
		IStructuredSelection selection = getSelectedStructuredSelection(window);
		if (selection == null || !(selection.getFirstElement() instanceof IFolder))
			return null;

		return (IFolder) selection.getFirstElement();
	}

	/**
	 * Gets the selected file or null if none selected
	 * @return
	 */
	public static IFile getSelectedFile()
	{
		return getSelectedFile(WorkspaceUtils.getWorkbenchWindow());
	}

	/**
	 * Gets the file selected or null if none selected
	 * @param window The workbench window from where to get the current selection
	 * @return
	 */
	public static IFile getSelectedFile(IWorkbenchWindow window)
	{
		IStructuredSelection selection = getSelectedStructuredSelection(window);
		if (selection == null || !(selection.getFirstElement() instanceof IFile))
			return null;
		return (IFile) selection.getFirstElement();
	}

	/**
	 * Gets the selected StructuredSelection or null if none selected
	 * @return
	 */
	public static IStructuredSelection getSelectedStructuredSelection()
	{
		return getSelectedStructuredSelection(WorkspaceUtils.getWorkbenchWindow());
	}
	/**
	 * Gets the selected StructuredSelection or null if none selected
	 * @param window The workbench windows from where to get the current selection
	 * @return
	 */
	public static IStructuredSelection getSelectedStructuredSelection(final IWorkbenchWindow window)
	{
		if (window == null)
			return null;
		MyRunnable<IStructuredSelection> runnable = new MyRunnable<IStructuredSelection>(){
			@Override
			public void run()
			{
				try{
					runnableObject_ = null;
					if (!(window.getSelectionService().getSelection() instanceof IStructuredSelection))
						return;
					runnableObject_ = (IStructuredSelection) window.getSelectionService().getSelection();
				}
				catch (Exception e) {
					e.printStackTrace();
				}
			}
		};
		Display.getDefault().syncExec(runnable);
		return runnable.runnableObject_;
	}

	/**
	 * Gets the selected container (folder/project) or null if none selected
	 * @return
	 */
	public static IContainer getSelectedContainer()
	{
		IStructuredSelection selection = getSelectedStructuredSelection();
		if (selection == null ||
			!(selection.getFirstElement() instanceof IFolder ||
			  selection.getFirstElement() instanceof IProject))
			return null;

		return (IContainer) selection.getFirstElement();
	}


	/**
	 * Gets the selected resource(file/folder/project) or null if none selected
	 * @return
	 */
	public static IResource getSelectedResource()
	{
		IResource res = getSelectedFile();
		if (res != null)
			return res;

		res = getSelectedFolder();
		if (res != null)
			return res;

		res = getSelectedProject();
		if (res != null)
			return res;
		return null;
	}

	/**
	 * Returns the first WorkbenchWindow available. This is not always the same
	 * with ActiveWorkbecnWindow
	 *
	 * @return
	 */
	public static IWorkbenchWindow getWorkbenchWindow()
	{
		if (Activator.getDefault().getWorkbench().getActiveWorkbenchWindow() != null)
			return Activator.getDefault().getWorkbench().getActiveWorkbenchWindow();
		if (Activator.getDefault().getWorkbench().getWorkbenchWindowCount() == 0)
			return null;
		return Activator.getDefault().getWorkbench().getWorkbenchWindows()[0];
	}

	/**
	 * Returns the current working set manager
	 * @return
	 */
	public static IWorkingSetManager getWorkingSetManager()
	{
		return getWorkbenchWindow().getWorkbench().getWorkingSetManager();
	}

	/**
	 * Returns the temporary folder where the plugin can write resources
	 *
	 * @return
	 */
	public static String getTemporaryFolder()
	{
		if (temporaryFolder_.isEmpty())
		{
			temporaryFolder_ = System.getProperty("java.io.tmpdir") + //$NON-NLS-1$
						Path.SEPARATOR + "wesnoth_plugin" + Path.SEPARATOR; //$NON-NLS-1$

			File tmpFile = new File(temporaryFolder_);
			if (!tmpFile.exists())
				tmpFile.mkdirs();
		}
		return temporaryFolder_;
	}

	/**
	 * Returns a random fileName generated from current time
	 * @return
	 */
	public static String getRandomFileName()
	{
		String result = ""; //$NON-NLS-1$
		SimpleDateFormat date = new SimpleDateFormat("yyyy_MM_dd_HH_mm_ss"); //$NON-NLS-1$
		result += date.format(new Date());
		return result;
	}

	/**
	 * Returns the resource path relative to the user directory
	 *
	 * @param resource the resource to be computed
	 * @return
	 */
	public static String getPathRelativeToUserDir(IResource resource)
	{
		if (resource == null)
			return null;

		String result = Preferences.getString(Constants.P_WESNOTH_USER_DIR) +
							Path.SEPARATOR + "data/add-ons/"; //$NON-NLS-1$
		result += (resource.getProject().getName() + Path.SEPARATOR);
		result += resource.getProjectRelativePath().toOSString();
		return result;
	}

	/**
	 * Setups the workspace, by checking:
	 * 1) The user has set all plugin's preferences.
	 * 	If not, the preferences window will open
	 */
	public static void setupWorkspace(final boolean guided)
	{
		if (!checkConditions(false))
		{
			PreferenceDialog pref = PreferencesUtil.createPreferenceDialogOn(
					Activator.getShell(), "wml_plugin_preferences", null, null); //$NON-NLS-1$
			if (pref.open() == Window.CANCEL || !checkConditions(true))
			{
				GUIUtils.showErrorMessageBox(Messages.WorkspaceUtils_7 +
						Messages.WorkspaceUtils_8);
				return;
			}

			if (guided)
			{
				GUIUtils.showInfoMessageBox(
						Messages.WorkspaceUtils_9 +
						Messages.WorkspaceUtils_10+
						Messages.WorkspaceUtils_11+
						Messages.WorkspaceUtils_12);
			}
		}

		WorkspaceJob job = new WorkspaceJob(Messages.WorkspaceUtils_13) {
			@Override
			public IStatus runInWorkspace(IProgressMonitor monitor)
			{
				try
				{
					// create a default working set
					IWorkingSetManager manager = getWorkingSetManager();
					IWorkingSet defaultSet = manager.getWorkingSet("Default"); //$NON-NLS-1$
					if (defaultSet == null)
					{
						// we could get an assert exception if too many
						// 'setup workspace' are running at a time
						try
						{
							defaultSet = manager.createWorkingSet("Default", new IAdaptable[0]); //$NON-NLS-1$
							manager.addWorkingSet(defaultSet);
						}
						catch(AssertionFailedException e)
						{
							Logger.getInstance().logWarn(e.getMessage());
						}
					}

					// automatically import 'special' folders as projects
					List<File> files = new ArrayList<File>();
					String addonsDir = Preferences.getString(Constants.P_WESNOTH_USER_DIR)+"/data/add-ons/"; //$NON-NLS-1$
					String campaignsDir = Preferences.getString(Constants.P_WESNOTH_WORKING_DIR) + "/data/campaigns/"; //$NON-NLS-1$

					File[] tmp = null;
					if (GUIUtils.showMessageBox(Messages.WorkspaceUtils_18 +
							Messages.WorkspaceUtils_19,
							SWT.ICON_QUESTION | SWT.YES | SWT.NO) == SWT.YES)
					{
						// useraddons/add-ons/data
						tmp = new File(addonsDir).listFiles();
						if (tmp != null)
							files.addAll(Arrays.asList(tmp));
					}

					if (GUIUtils.showMessageBox(Messages.WorkspaceUtils_20 +
							Messages.WorkspaceUtils_21,
							SWT.ICON_QUESTION | SWT.YES | SWT.NO) == SWT.YES)
					{
						// workingdir/data/campaigns
						tmp = new File(campaignsDir).listFiles();
						if (tmp != null)
							files.addAll(Arrays.asList(tmp));
					}

					monitor.beginTask(Messages.WorkspaceUtils_22, files.size() * 35);
					for(File file: files)
					{
						if (file.isDirectory() == false ||
							file.getName().startsWith(".")) //$NON-NLS-1$
							continue;

						String projectName = file.getName();
						if (StringUtils.normalizePath(file.getAbsolutePath()).contains(
							StringUtils.normalizePath(campaignsDir)))
						{
							projectName = "_Mainline_" + file.getName(); //$NON-NLS-1$
						}

						IProjectDescription description =
							ResourcesPlugin.getWorkspace().newProjectDescription(projectName);
						description.setLocation(new Path(file.getAbsolutePath()));

						IContainer container = ResourcesPlugin.getWorkspace().getRoot().
							getContainerForLocation(new Path(file.getAbsolutePath()));

						IProject project = ResourcesPlugin.getWorkspace().getRoot().getProject(projectName);
						// don't create the project if it exists already
						if (container == null)
						{
							ProjectUtils.createWesnothProject(project, description,
									true, false, monitor);
							container = project;
						}
						manager.addToWorkingSets(container, new IWorkingSet[] { defaultSet });
					}

					//TODO select the default working set manager as the active one
					if (guided)
					{
						GUIUtils.showInfoMessageBox(
								Messages.WorkspaceUtils_25 +
								Messages.WorkspaceUtils_26 +
								Messages.WorkspaceUtils_27);
					}
					else
					{
						Logger.getInstance().log(Messages.WorkspaceUtils_28,
								Messages.WorkspaceUtils_29);
					}
				} catch (Exception e)
				{
					Logger.getInstance().logException(e);
					GUIUtils.showErrorMessageBox(Messages.WorkspaceUtils_30);
				}

				monitor.done();
				return Status.OK_STATUS;
			}
		};
		job.schedule();
	}

	/**
	 * Creates and adds an ignore filter on the specified project
	 * for folders matching the specified name. The specified folder won't appear
	 * in any views of the current project's structure
	 * @param project The project where to create the filter
	 * @param folderName The foldername to ignore
	 * @throws CoreException
	 */
	public static void createIgnoreFilter(IProject project, String folderName)
		throws CoreException
	{
		// For details regarding the description see:
		// 	FileInfoAttributesMatcher.encodeArguments()

		// id = org.eclipse.ui.ide.multiFilter
		// args = 1.0-name-matches-false-false-Love_to_death
		project.createFilter(IResourceFilterDescription.EXCLUDE_ALL | IResourceFilterDescription.FOLDERS,
				new FileInfoMatcherDescription("org.eclipse.ui.ide.multiFilter", //$NON-NLS-1$
						"1.0-name-matches-false-false-" + folderName), //$NON-NLS-1$
				IResource.BACKGROUND_REFRESH, new NullProgressMonitor());
	}

	/**
	 * Checks if the user has set some needed preferences and if the workspace
	 * is setup
	 *
	 * @param displayWarning true to display a messagebox warning
	 * 		  the user if conditions are not met
	 */
	private static boolean checkConditions(boolean displayWarning)
	{
		String execDir = Preferences.getString(Constants.P_WESNOTH_EXEC_PATH);
		String userDir = Preferences.getString(Constants.P_WESNOTH_USER_DIR);
		String wmltoolsDir = Preferences.getString(Constants.P_WESNOTH_WMLTOOLS_DIR);
		String workingDir = Preferences.getString(Constants.P_WESNOTH_WORKING_DIR);

		if (!validPath(execDir) || !validPath(userDir) ||
			!validPath(wmltoolsDir) || !validPath(workingDir))
		{
			if (displayWarning)
				GUIUtils.showWarnMessageBox(Messages.WorkspaceUtils_33);
			return false;
		}
		return true;
	}

	/**
	 * Checks if the path is valid and the specified path's resource exists
	 * @param path the path to check
	 * @return
	 */
	public static boolean validPath(String path)
	{
		return !path.isEmpty() && new File(path).exists();
	}
}
